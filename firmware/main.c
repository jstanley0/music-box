// attinyX4 / TI SN76489 music box
// (c) 2021 by Jeremy Stanley

// port assignments (AVR -> TI SN76489):
// PA0..PA7 -> D7..D0
// PB1 -> /WE
// PB2 -> CLOCK (via CKOUT fuse)

// other pin assignments:
// PB0 -> blinkenlight

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "songs.h"

// notes are hit at full volume (0) and decay to this point where they will sustain
// until note-off, then fade out
#define SUSTAIN_LEVEL 10

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
  DDRB  = (1 << DDB0) | (1 << DDB1);
  PORTB = (1 << PORTB1);
}

void send_byte(uint8_t byte)
{
  PORTA = byte;
  PORTB &= ~(1 << PORTB1);
  _delay_us(16);  // 32 clock cycles at 2MHz
  PORTB |= (1 << PORTB1);
}

void send_frequency(uint8_t voice, uint16_t freq)
{
  send_byte(0x80U | (voice << 5) | (freq & 0x0F));
  send_byte(freq >> 4);
}

void send_attenuation(uint8_t voice, uint8_t atten)
{
  send_byte(0x90U | (voice << 5) | atten);
}

// (not playing)
#define NP 64

uint8_t playing_notes[3] = { NP, NP, NP };
uint8_t playing_atten[3] = { 15, 15, 15 };

void note_on(uint8_t note)
{
  uint8_t i;

  // see if this note is already playing, and re-use its voice
  for(i = 0; i < 3; ++i) {
    if (playing_notes[i] == note) {
      break;
    }
  }

  // otherwise find an available voice
  if (i == 3) {
    for(i = 0; i < 3; ++i) {
      if (playing_notes[i] == NP)
        break;
    }
  }

  // if none, drop the note :(
  // (this should not actually happen if the midi preprocessor did its job)
  if (i == 3) {
    return;
  }

  // play the note!
  playing_notes[i] = note;
  playing_atten[i] = 0;
  send_frequency(i, pgm_read_word(&note_table[note]));
  send_attenuation(i, 0);
}

void note_off(uint8_t note)
{
  for(uint8_t i = 0; i < 3; ++i) {
    if (playing_notes[i] == note) {
      playing_notes[i] = NP;
      break;
    }
  }
}

// handle note envelope modulation
// and piggyback blinkenlight intensity while we're at it
void decay()
{
  uint8_t blinkenlight = 0;
  for(uint8_t i = 0; i < 3; ++i) {
    uint8_t atten_target = (playing_notes[i] == NP) ? 15 : SUSTAIN_LEVEL;
    if (playing_atten[i] < atten_target) {
      send_attenuation(i, ++playing_atten[i]);
    }
    blinkenlight += (64 >> playing_atten[i]);
  }
  OCR0A = blinkenlight;
}

void silence()
{
  for(int i = 0; i < 4; ++i)
    send_attenuation(i, 15);
}

// blinkenlight off
ISR(TIM0_COMPA_vect)
{
  PORTB &= ~(1 << PORTB0);
}

// blinkenlight on
ISR(TIM0_OVF_vect)
{
  PORTB |= (1 << PORTB0);
}

// song progress; fired every 10ms
ISR(TIM1_COMPA_vect)
{
  static uint16_t song_pos = 0;
  static uint8_t wait_cycles = 50;
  static uint8_t decay_cycles = 0;

  if (++decay_cycles == 5) {
    decay_cycles = 0;
    decay();
  }

  if (wait_cycles > 0) {
    --wait_cycles;
    return;
  }

  // keep executing song commands until we hit a delay or end of song
  for(;;) {
    uint8_t song_cmd = pgm_read_byte(&song_data[song_pos]);
    switch(song_cmd & 0xC0) {
    case 0: // note on
      note_on(song_cmd);
      break;
    case 0x80: // note off
      note_off(song_cmd & 0x3F);
      break;
    case 0x40: // delay
      wait_cycles = song_cmd & 0x3F;
      ++song_pos;
      return;
    case 0xC0: // end song (for now)
      song_pos = 0;
      wait_cycles = 200;
      return;
    }
    ++song_pos;
  }
}

void init_interrupts()
{
  // blinkenlight: PWM on OCR0A
  TCCR0A = (1 << WGM01) | (1 << WGM00);  // fast PWM mode
  TCCR0B = (1 << CS01) | (1 << CS00);    // 2MHz / 256 / 64 = 122Hz, a perfectly cromulent refresh rate
  TIMSK0 = (1 << OCIE0A) | (1 << TOIE0); // enable COMPA and OVF interrupts

  // music progress: fire every 10ms
  TCCR1A = 0;                            // normal mode
  OCR1A = 2500;                          // 250kHz / 2500 = 100 Hz
  TCCR1B = (1 << CS11) | (1 << WGM12);   // 1/8 prescaler, ticks at 250kHz; CTC on OCR1A
  TIMSK1 = (1 << OCIE1A);                // enable COMPA vector
}

int main(void)
{
  init_clock();
  init_io();
  silence();
  init_interrupts();
  sei();

  // main loop is empty; everything is interrupt-driven
  for(;;){
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    sleep_cpu();
  }
  return 0;   // never reached

}
