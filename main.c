// attinyX4 / TI SN76489 music box
// 1MHz system clock

// port assignments (AVR -> TI SN76489)
// PA0..PA7 -> D7..D0
// PB0 -> (box switch)
// PB1 -> /WE
// PB2 -> CLOCK

// placed in the public domain by the author
// Jeremy Stanley, January 2021

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "songs.h"

// (not playing)
#define NP 64

void init_io()
{
  DDRA  = 0xff;
  DDRB  = 1 << DDB1;
  PORTB = (1 << PORTB0) | (1 << PORTB1);
}

unsigned char button_pressed()
{
  static unsigned char prev_button = (1 << PORTB0);
  unsigned char cur_button = PINB & (1 << PORTB0);
  unsigned char retval = cur_button && !prev_button;
  prev_button = cur_button;
  return retval;
}

void send_byte(uint8_t byte)
{
  PORTA = byte;
  PORTB &= ~(1 << PORTB1);
  _delay_us(30);
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

uint8_t playing_notes[3] = { NP, NP, NP };

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
  if (i == 3) {
    return;
  }

  // play the note!
  playing_notes[i] = note;
  send_frequency(i, pgm_read_word(&note_table[note]));
  send_attenuation(i, 0);
}

void note_off(uint8_t note)
{
  uint8_t i;
  for(i = 0; i < 3; ++i) {
    if (playing_notes[i] == note) {
      playing_notes[i] = NP;
      send_attenuation(i, 15);
    }
  }
}

void silence()
{
  for(int i = 0; i < 4; ++i)
    send_attenuation(i, 15);
}

int main(void)
{
  init_io();
  silence();

  uint16_t i = 0;
  uint8_t d;
  for(;;) {
    uint8_t cmd = pgm_read_byte(&song_data[i]);
    switch(cmd & 0xC0)
    {
    case 0x00: // note on
      note_on(cmd);
      break;
    case 0x40: // delay
      d = cmd & 0x3F;
      while(d > 0) {
        _delay_ms(10); // <- that has to be a compile-time constant. we will rewrite this to use interrupts
        --d;
      }
      break;
    case 0x80: // note off
      note_off(cmd & 0x3F);
      break;
    case 0xC0: // end of song; start over
      _delay_ms(500);
      i = 0;
      break;
    }
    ++i;
  }

  return 0;   // never reached
}
