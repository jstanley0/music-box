// attinyX4 / TI SN76489 music box
// (c) 2021 by Jeremy Stanley

// port assignments (AVR -> TI SN76489):
// PA0..PA7 -> D7..D0
// PB4 -> /WE
// PB5 -> CLOCK (via CKOUT fuse)

// other pin assignments:
// PB0..3 -> blinkenlights
// PB6 -> tac switch

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "songs.h"

// hi-hat, ride cymbal, etc. don't play at full volume
#define ATTENUATED_PERCUSSION_LEVEL 4

void init_clock()
{
  // change clock divider from /8 to /4 so we run at 2MHz,
  // which is an acceptable clock frequency for the SN76489
  // (at 4MHz, we lose too much low range, and at 1MHz, high notes get out of tune)
  CLKPR = (1 << CLKPCE);  // Clock Prescaler Change Enable - this part is necessary
  CLKPR = (1 << CLKPS1);  // 4x divider
}

void init_io()
{
  DDRA  = 0xff;
  DDRB  = 0b00111111;
  PORTB = (1 << PORTB6) | (1 << PORTB4);
}

void send_byte(uint8_t byte)
{
  PORTA = byte;
  PORTB &= ~(1 << PORTB4);
  _delay_us(16);  // 32 clock cycles at 2MHz
  PORTB |= (1 << PORTB4);
}

void send_frequency(uint8_t voice, uint16_t freq)
{
  send_byte(0x80U | (voice << 5) | (freq & 0x0F));
  send_byte(freq >> 4);
}

void send_noise(uint8_t n)
{
  send_byte(0xE0U | n);
}

void send_attenuation(uint8_t voice, uint8_t atten)
{
  send_byte(0x90U | (voice << 5) | atten);
}

uint8_t playing_atten[4] = { 15, 15, 15, 15 };

void note(uint8_t note)
{
  // pick the quietest voice
  uint8_t v = 0;
  for(uint8_t i = 1; i < 3; ++i) {
    if (playing_atten[i] > playing_atten[v])
      v = i;
  }

  // play the note!
  playing_atten[v] = 0;
  send_frequency(v, pgm_read_word(&note_table[note]));
  send_attenuation(v, 0);
}

void noise(uint8_t n, uint8_t initial_attenuation)
{
  playing_atten[3] = initial_attenuation;
  send_noise(n);
  send_attenuation(3, initial_attenuation);
}

void decay()
{
  static volatile uint8_t *blinky_reg[3] = { &OCR0A, &OCR1A, &OCR1D };
  for(uint8_t i = 0; i < 3; ++i) {
    if (playing_atten[i] < 15) {
      send_attenuation(i, ++playing_atten[i]);
      *blinky_reg[i] = 0xff >> playing_atten[i];
    }
  }
}

void decay_noise()
{
  if (playing_atten[3] < 15) {
    send_attenuation(3, ++playing_atten[3]);
    OCR1B = 0xff >> playing_atten[3];
  }
}

void silence()
{
  for(int i = 0; i < 4; ++i)
    send_attenuation(i, 15);
}

// LED 3 off
ISR(TIMER1_COMPD_vect)
{
  PORTB &= 0b11111011;
}

// LED 3 on
ISR(TIMER1_OVF_vect)
{
  PORTB |= 0b00000100;
}

// LED 1 off
ISR(TIMER0_COMPA_vect)
{
  PORTB &= 0b11111110;
}

volatile uint8_t g_Tick = 0;

// LED 1 on, also, music heartbeat (@ 122 Hz)
ISR(TIMER0_OVF_vect)
{
  PORTB |= 0b00000001;
  g_Tick += 1;
}

void sleep_one_tick()
{
  while (g_Tick == 0) {
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    sleep_cpu();
  }
  g_Tick -= 1;
}

void init_interrupts()
{
  // this is gonna be a bit hacky because THERE ARE FOUR LIGHTS.

  // two of them (LEDs 2 and 4) are easy: they're on OC1A and OC1B pins, so we can use fast PWM mode
  TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << PWM1A) | (1 << PWM1B);

  // we can't use the third PWM channel (because its pin is shared with CLKO)
  // so we'll do software PWM for LED 3 with the compare-match D and overflow interrupts
  TIMSK = (1 << OCIE1D) | (1 << TOIE1);

  // set prescaler 1/64: 2MHz / 256 / 64 = 122Hz, a perfectly cromulent refresh rate
  TCCR1B = (1 << CS12) | (1 << CS11) | (1 << CS10);

  // and let's do software PWM with timer 0 too
  TCCR0A = 0;                            // normal mode, 8-bit
  TCCR0B = (1 << CS01) | (1 << CS00);    // 1/64 prescaler
  TIMSK |= (1 << OCIE0A) | (1 << TOIE0); // enable COMPA and overflow vectors
}

void play_song()
{
  uint16_t song_pos = 0;
  uint8_t wait_cycles = 50;
  uint8_t decay_cycles = 0;
  uint8_t noise_cycles = 0;
  int8_t noise_target = 0;

  // stop playing if the button is pressed
  while(PINB & (1 << PB6))
  {
    sleep_one_tick();

    if (++decay_cycles == 6) {
      decay_cycles = 0;
      decay();
    }
    if (++noise_cycles == noise_target) {
      noise_cycles = 0;
      decay_noise();
    }

    if (wait_cycles > 0) {
      --wait_cycles;
      continue;
    }

    // keep executing song commands until we hit a delay or end of song
    while(wait_cycles == 0) {
      uint8_t song_cmd = pgm_read_byte(&song_data[song_pos]);
      uint8_t noise_atten;
      switch(song_cmd & 0xC0) {
      case 0: // note
        note(song_cmd);
        break;
      case 0x40: // delay
        wait_cycles = song_cmd & 0x3F;
        break;
      case 0x80: // noise
        //  7  6  5  4  3  2  1  0
        //  1  0 A0 S1 S0 N2 N1 N0
        noise_target = 1 + ((song_cmd & 0x18) >> 3);
        noise_atten = (song_cmd & 0x20);
        noise_cycles = 0;
        noise(song_cmd & 0x07, noise_atten ? ATTENUATED_PERCUSSION_LEVEL : 0);
        break;
      case 0xC0: // end song
        return;
      }
      ++song_pos;
    }
  }
}

int main(void)
{
  init_clock();
  init_io();
  silence();
  init_interrupts();
  sei();

  play_song();

  silence();
  TCCR1B = 0;
  TCCR0B = 0;
  PORTB &= 0xF0;
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();
}
