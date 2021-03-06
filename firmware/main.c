// attiny861 / TI SN76489 music box
// (c) 2021 by Jeremy Stanley

// port assignments (ATtiny861 -> TI SN76489):
// PA0..PA7 -> D7..D0
// PB4 -> /WE
// PB5 -> CLOCK (via CKOUT fuse)

// other pin assignments:
// PB0..3 -> blinkenlights
// PB6 -> tac switch

// alternatively this can be compiled for ATtiny84
// using `make -e DEVICE=attiny84`
// which uses the following port assignments:
// PA0..PA7 -> D7..D0
// PB1 -> /WE
// PB2 -> CLOCK
// PB0 -> blinkenlight (just one)

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "songs.h"
const uint8_t *const songs[] PROGMEM = { st_elsewhere, lament, macgyver, brooklet, quantum_leap, nervous, hogans_heroes, charlie_brown };
const uint8_t NUM_SONGS = sizeof(songs) / sizeof(songs[0]);

#ifdef __AVR_ATtiny84__
#define WE_BIT PORTB1
#else
#define WE_BIT PORTB4
#endif

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
#ifdef __AVR_ATtiny84__
  DDRB  = 0b00000111;
  PORTB = (1 << WE_BIT);
#else
  DDRB  = 0b00111111;
  PORTB = (1 << PORTB6) | (1 << WE_BIT);
#endif
}

void send_byte(uint8_t byte)
{
  PORTA = byte;
  PORTB &= ~(1 << WE_BIT);
  _delay_us(16);  // 32 clock cycles at 2MHz
  PORTB |= (1 << WE_BIT);
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

#ifdef __AVR_ATtiny84__
volatile uint8_t OCR0A_buf = 0;

void update_blinkenlight()
{
  uint8_t blinkenlight = 0;
  for(uint8_t i = 0; i < 4; ++i) {
    blinkenlight += (63 >> playing_atten[i]);
  }
  OCR0A_buf = blinkenlight;
}
#else
static const uint8_t fade_table[16] PROGMEM = {254, 179, 125, 88, 61, 43, 30, 21, 15, 10, 7, 5, 4, 2, 1, 0};

volatile uint8_t OCR0A_buf = 0, OCR0B_buf = 0;
void set_led(uint8_t voice, uint8_t atten)
{
    static volatile uint8_t *intensity_reg[4] = { &OCR0A_buf, &OCR1A, &OCR0B_buf, &OCR1B };
    *intensity_reg[voice] = pgm_read_byte(&fade_table[playing_atten[voice]]);
}
#endif

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
#ifndef __AVR_ATtiny84__
  set_led(v, 0);
#endif
}

void noise(uint8_t n, uint8_t initial_attenuation)
{
  playing_atten[3] = initial_attenuation;
  send_noise(n);
  send_attenuation(3, initial_attenuation);
#ifndef __AVR_ATtiny84__
  set_led(3, 0);
#endif
}

void decay()
{
  for(uint8_t i = 0; i < 3; ++i) {
    if (playing_atten[i] < 15) {
      send_attenuation(i, ++playing_atten[i]);
#ifndef __AVR_ATtiny84__
      set_led(i, playing_atten[i]);
#endif
    }
  }
}

void decay_noise()
{
  if (playing_atten[3] < 15) {
    send_attenuation(3, ++playing_atten[3]);
#ifndef __AVR_ATtiny84__
    set_led(3, playing_atten[3]);
#endif
  }
}

void silence()
{
  for(int i = 0; i < 4; ++i)
    send_attenuation(i, 15);
}

volatile uint8_t g_Tick = 0;

#ifdef __AVR_ATtiny84__

ISR(TIM0_COMPA_vect)
{
  // LED off
  PORTB &= 0b11111110;
}

ISR(TIM0_OVF_vect)
{
  // LED on, maybe
  OCR0A = OCR0A_buf;
  if (OCR0A_buf > 0)
    PORTB |= 1;

  // music heartbeat
  g_Tick += 1;
}

#else

// LED 1 off
ISR(TIMER0_COMPA_vect)
{
  PORTB &= 0b11111110;
}

// LED 3 off
ISR(TIMER0_COMPB_vect)
{
  PORTB &= 0b11111011;
}

ISR(TIMER0_OVF_vect)
{
  // LED 1 on, maybe
  OCR0A = OCR0A_buf;
  if (OCR0A_buf > 0)
    PORTB |= 0b001;

  // LED 3
  OCR0B = OCR0B_buf;
  if (OCR0B_buf > 0)
    PORTB |= 0b100;

  // music heartbeat
  g_Tick += 1;
}

#endif

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
#ifdef __AVR_ATtiny84__
  // one LED with software PWM
  TCCR0A = 0;                            // normal mode, 8-bit
  TCCR0B = (1 << CS01) | (1 << CS00);    // 1/64 prescaler
  TIMSK0 = (1 << OCIE0A) | (1 << TOIE0); // enable COMPA and OVF vectors
#else
  // this is gonna be a bit hacky because THERE ARE FOUR LIGHTS.

  // two of them (LEDs 2 and 4) are easy: they're on OC1A and OC1B pins, so we can use fast PWM mode
  TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << PWM1A) | (1 << PWM1B);
  // set prescaler 1/64: 2MHz / 256 / 64 = 122Hz, a perfectly cromulent refresh rate
  TCCR1B = (1 << CS12) | (1 << CS11) | (1 << CS10);

  // we can't use the third PWM channel (because its pin is shared with CLKO)
  // and there is no fourth PWM channel anyway
  // so we'll use timer0 for LEDs 1 and 3
  TCCR0A = 0;                            // normal mode, 8-bit
  TCCR0B = (1 << CS01) | (1 << CS00);    // 1/64 prescaler
  TIMSK |= (1 << OCIE0A) | (1 << OCIE0B) | (1 << TOIE0); // enable COMPA, COMPB, and overflow vectors
#endif
}

void play_song(uint8_t song_num)
{
  const uint8_t *PROGMEM song_data = (uint8_t *)pgm_read_word(&songs[song_num]);
  uint16_t song_pos = 0;
  uint8_t wait_cycles = 50;
  uint8_t decay_cycles = 0;
  uint8_t noise_cycles = 0;
  int8_t noise_target = 0;
  int8_t fade_out = 0;

  for(;;)
  {
    sleep_one_tick();

#ifdef __AVR_ATtiny84__
    update_blinkenlight();
#else
    if (0 == (PINB & (1 << PB6))) {
        fade_out = 100;
    }
#endif

    if (++decay_cycles == 6) {
      decay_cycles = 0;
      decay();
    }
    if (++noise_cycles == noise_target) {
      noise_cycles = 0;
      decay_noise();
    }

    if (fade_out > 0) {
        if (--fade_out == 0)
            break;
        else
            continue;
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
        fade_out = 100;
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

  uint8_t song_num = 0;
  for(;;) {
    play_song(song_num);
    if (++song_num >= NUM_SONGS)
        song_num = 0;
  }
}
