#include <avr/pgmspace.h>
#include "songs.h"

// song format

// note (N == midi_note - 35)
//  7  6  5  4  3  2  1  0
//  0  0 N5 N4 N3 N2 N1 N0

// delay (D == delay_in_ms / 10)
//  7  6  5  4  3  2  1  0
//  0  1 D5 D4 D3 D2 D1 D0

// noise (TODO)
//  7  6  5  4  3  2  1  0
//  1  0  0  0  0 N2 N1 N0

// end song
//  7  6  5  4  3  2  1  0
//  1  1  1  1  1  1  1  1

const uint16_t note_table[64] PROGMEM = {1012,955,901,851,803,758,715,675,637,601,568,536,506,477,450,425,401,379,357,337,318,300,284,268,253,238,225,212,200,189,178,168,159,150,142,134,126,119,112,106,100,94,89,84,79,75,71,67,63,59,56,53,50,47,44,42,39,37,35,33,31,29,28,26,};
const uint8_t song_data[] PROGMEM = {0x24,0x4a,0x22,0x4a,0x21,0x4b,0x22,0x4a,0x16,0x25,0x55,0x19,0x1d,0x54,0x19,0x1d,0x27,0x4b,0x25,0x4a,0x19,0x1d,0x24,0x4a,0x25,0x4b,0x16,0x29,0x54,0x19,0x1d,0x55,0x19,0x1d,0x2a,0x4a,0x29,0x4b,0x19,0x1d,0x28,0x4a,0x29,0x4a,0x16,0x30,0x4b,0x2e,0x4a,0x19,0x1d,0x2d,0x4a,0x2e,0x4b,0x16,0x30,0x4a,0x2e,0x4a,0x19,0x1d,0x2d,0x4b,0x2e,0x4a,0x16,0x31,0x55,0x19,0x1d,0x55,0x19,0x1d,0x2e,0x54,0x19,0x1d,0x31,0x55,0x11,0x2c,0x45,0x2e,0x45,0x30,0x4b,0x18,0x2b,0x2e,0x54,0x18,0x29,0x2c,0x55,0x18,0x2b,0x2e,0x55,0x11,0x2c,0x45,0x2e,0x45,0x30,0x4a,0x18,0x2b,0x2e,0x55,0x18,0x29,0x2c,0x55,0x18,0x2b,0x2e,0x54,0x11,0x2c,0x46,0x2e,0x45,0x30,0x4a,0x18,0x2b,0x2e,0x55,0xc,0x29,0x2c,0x54,0x18,0x28,0x2b,0x55,0x11,0x29,0x6a,0x24,0x4a,0x22,0x4a,0x21,0x4b,0x22,0x4a,0x16,0x25,0x55,0x19,0x1d,0x54,0x19,0x1d,0x27,0x4b,0x25,0x4a,0x19,0x1d,0x24,0x4a,0x25,0x4b,0x16,0x29,0x54,0x19,0x1d,0x55,0x19,0x1d,0x2a,0x4a,0x29,0x4b,0x19,0x1d,0x28,0x4a,0x29,0x4a,0x16,0x30,0x4b,0x2e,0x4a,0x19,0x1d,0x2d,0x4a,0x2e,0x4b,0x16,0x30,0x4a,0x2e,0x4b,0x19,0x1d,0x2d,0x4a,0x2e,0x4a,0x16,0x31,0x55,0x19,0x1d,0x55,0x19,0x1d,0x2e,0x54,0x19,0x1d,0x31,0x55,0x11,0x2c,0x45,0x2e,0x45,0x30,0x4b,0x18,0x2b,0x2e,0x54,0x18,0x29,0x2c,0x55,0x18,0x2b,0x2e,0x55,0x11,0x2c,0x45,0x2e,0x45,0x30,0x4a,0x18,0x2b,0x2e,0x55,0x18,0x29,0x2c,0x55,0x18,0x2b,0x2e,0x54,0x11,0x2c,0x46,0x2e,0x45,0x30,0x4a,0x18,0x2b,0x2e,0x55,0xc,0x29,0x2c,0x55,0x18,0x28,0x2b,0x54,0x11,0x29,0x6a,0x25,0x29,0x54,0x27,0x2a,0x55,0xd,0x29,0x2c,0x55,0x19,0x29,0x2c,0x54,0x11,0x2e,0x4b,0x2c,0x4a,0x1d,0x2a,0x4a,0x29,0x4b,0x14,0x24,0x27,0x54,0x20,0x55,0x25,0x29,0x55,0x27,0x2a,0x55,0xd,0x29,0x2c,0x54,0x19,0x29,0x2c,0x55,0x11,0x2e,0x4a,0x2c,0x4b,0x1d,0x2a,0x4a,0x29,0x4a,0x14,0x24,0x27,0x6a,0x22,0x25,0x54,0x24,0x27,0x55,0xa,0x25,0x29,0x55,0x16,0x25,0x29,0x54,0xd,0x2a,0x4b,0x29,0x4a,0x19,0x27,0x4a,0x25,0x4b,0x11,0x21,0x24,0x54,0x1d,0x55,0x22,0x25,0x55,0x24,0x27,0x55,0xa,0x25,0x29,0x54,0x16,0x25,0x29,0x55,0xd,0x2a,0x4a,0x29,0x4b,0x19,0x27,0x4a,0x25,0x4a,0x11,0x21,0x24,0x6a,0x24,0x4a,0x22,0x4a,0x21,0x4b,0x22,0x4a,0x16,0x25,0x55,0x19,0x1d,0x54,0x19,0x1d,0x27,0x4b,0x25,0x4a,0x19,0x1d,0x24,0x4a,0x25,0x4b,0x16,0x29,0x55,0x19,0x1d,0x54,0x19,0x1d,0x2a,0x4b,0x29,0x4a,0x19,0x1d,0x28,0x4a,0x29,0x4b,0x16,0x30,0x4a,0x2e,0x4a,0x19,0x1d,0x2d,0x4b,0x2e,0x4a,0x16,0x30,0x4a,0x2e,0x4b,0x19,0x1d,0x2d,0x4a,0x2e,0x4a,0x12,0x31,0x55,0x16,0x1c,0x55,0x16,0x1c,0x2e,0x54,0x16,0x1c,0x30,0x55,0x11,0x31,0x55,0x16,0x1d,0x30,0x54,0xf,0x2e,0x55,0x12,0x18,0x2d,0x55,0xd,0x2e,0x55,0x11,0x16,0x29,0x54,0xf,0x2a,0x55,0x12,0x18,0x27,0x55,0x11,0x16,0x25,0x54,0x11,0x16,0x55,0x11,0x15,0x25,0x45,0x24,0x45,0x25,0x46,0x24,0x45,0x11,0x15,0x25,0x45,0x24,0x45,0x22,0x45,0x24,0x45,0xa,0x16,0x22,0x6a,0x25,0x29,0x54,0x27,0x2a,0x55,0xd,0x29,0x2c,0x55,0x19,0x29,0x2c,0x55,0x11,0x2e,0x4a,0x2c,0x4a,0x1d,0x2a,0x4b,0x29,0x4a,0x14,0x24,0x27,0x55,0x20,0x54,0x25,0x29,0x55,0x27,0x2a,0x55,0xd,0x29,0x2c,0x54,0x19,0x29,0x2c,0x55,0x11,0x2e,0x4a,0x2c,0x4b,0x1d,0x2a,0x4a,0x29,0x4a,0x14,0x24,0x27,0x6a,0x22,0x25,0x54,0x24,0x27,0x55,0xa,0x25,0x29,0x55,0x16,0x25,0x29,0x55,0xd,0x2a,0x4a,0x29,0x4a,0x19,0x27,0x4b,0x25,0x4a,0x11,0x21,0x24,0x55,0x1d,0x54,0x22,0x25,0x55,0x24,0x27,0x55,0xa,0x25,0x29,0x54,0x16,0x25,0x29,0x55,0xd,0x2a,0x4a,0x29,0x4b,0x19,0x27,0x4a,0x25,0x4a,0x11,0x21,0x24,0x6a,0x24,0x4a,0x22,0x4b,0x21,0x4a,0x22,0x4a,0x16,0x25,0x55,0x19,0x1d,0x55,0x19,0x1d,0x27,0x4a,0x25,0x4a,0x19,0x1d,0x24,0x4b,0x25,0x4a,0x16,0x29,0x55,0x19,0x1d,0x54,0x19,0x1d,0x2a,0x4b,0x29,0x4a,0x19,0x1d,0x28,0x4a,0x29,0x4b,0x16,0x30,0x4a,0x2e,0x4a,0x19,0x1d,0x2d,0x4b,0x2e,0x4a,0x16,0x30,0x4a,0x2e,0x4b,0x19,0x1d,0x2d,0x4a,0x2e,0x4a,0x12,0x31,0x55,0x16,0x1c,0x55,0x16,0x1c,0x2e,0x55,0x16,0x1c,0x30,0x54,0x11,0x31,0x55,0x16,0x1d,0x30,0x55,0xf,0x2e,0x54,0x12,0x18,0x2d,0x55,0xd,0x2e,0x55,0x11,0x16,0x29,0x54,0xf,0x2a,0x55,0x12,0x18,0x27,0x55,0x11,0x16,0x25,0x54,0x11,0x16,0x55,0x11,0x15,0x25,0x45,0x24,0x45,0x25,0x46,0x24,0x45,0x11,0x15,0x25,0x45,0x24,0x45,0x22,0x45,0x24,0x46,0xa,0x16,0x22,0x69,0x22,0x2e,0x55,0x24,0x30,0x54,0x16,0x26,0x32,0x55,0x16,0x55,0x16,0x22,0x2e,0x54,0x16,0x24,0x30,0x55,0x16,0x26,0x32,0x55,0x16,0x24,0x30,0x54,0x16,0x22,0x2e,0x55,0x16,0x21,0x2d,0x55,0xf,0x1f,0x2b,0x54,0xf,0x21,0x2d,0x55,0x10,0x22,0x2e,0x55,0x10,0x24,0x30,0x55,0x11,0x21,0x2d,0x54,0x11,0x1d,0x29,0x55,0x11,0x22,0x2e,0x55,0x11,0x24,0x30,0x54,0x16,0x26,0x32,0x55,0x16,0x55,0x16,0x22,0x2e,0x54,0x16,0x24,0x30,0x55,0x16,0x26,0x32,0x55,0x16,0x24,0x30,0x54,0x16,0x22,0x2e,0x55,0x16,0x21,0x2d,0x55,0xf,0x1f,0x2b,0x55,0xf,0x24,0x30,0x54,0x11,0x21,0x2d,0x55,0x11,0x1d,0x29,0x55,0xa,0x22,0x2e,0x69,0x22,0x2e,0x55,0x24,0x30,0x54,0x16,0x26,0x32,0x55,0x16,0x55,0x16,0x22,0x2e,0x54,0x16,0x24,0x30,0x55,0x16,0x26,0x32,0x55,0x16,0x24,0x30,0x54,0x16,0x22,0x2e,0x55,0x16,0x21,0x2d,0x55,0xf,0x1f,0x2b,0x55,0xf,0x21,0x2d,0x54,0x10,0x22,0x2e,0x55,0x10,0x24,0x30,0x55,0x11,0x21,0x2d,0x54,0x11,0x1d,0x29,0x55,0x11,0x22,0x2e,0x55,0x11,0x24,0x30,0x54,0x16,0x26,0x32,0x55,0x16,0x55,0x16,0x22,0x2e,0x54,0x16,0x24,0x30,0x55,0x16,0x26,0x32,0x55,0x16,0x24,0x30,0x55,0x16,0x22,0x2e,0x54,0x16,0x21,0x2d,0x55,0xf,0x1f,0x2b,0x55,0xf,0x24,0x30,0x54,0x11,0x21,0x2d,0x55,0x11,0x1d,0x29,0x55,0xa,0x22,0x2e,0x69,0x32,0x4a,0x33,0x4b,0x32,0x4a,0x30,0x4a,0x13,0x2e,0x4b,0x30,0x4a,0x16,0x1a,0x2e,0x4a,0x2d,0x4b,0x16,0x1a,0x2b,0x4a,0x2e,0x4a,0x16,0x1a,0x2d,0x4b,0x2b,0x4a,0x15,0x2a,0x4b,0x2b,0x4a,0x18,0x1a,0x2d,0x4a,0x2a,0x4b,0x18,0x1a,0x26,0x4a,0x28,0x4a,0x18,0x1a,0x2a,0x4b,0x26,0x4a,0x13,0x2b,0x4a,0x2a,0x4b,0x16,0x1a,0x2b,0x4a,0x2d,0x4a,0x16,0x1a,0x2e,0x4b,0x2d,0x4a,0x16,0x1a,0x2e,0x4a,0x30,0x4b,0x12,0x32,0x4a,0x31,0x4a,0x15,0x1a,0x32,0x4b,0x31,0x4a,0x15,0x1a,0x32,0x4a,0x33,0x4b,0x15,0x1a,0x32,0x4a,0x30,0x4a,0x13,0x2e,0x4b,0x30,0x4a,0x16,0x1a,0x2e,0x4a,0x2d,0x4b,0x16,0x1a,0x2b,0x4a,0x2e,0x4b,0x16,0x1a,0x2d,0x4a,0x2b,0x4a,0x15,0x29,0x4b,0x2b,0x4a,0x1a,0x1d,0x2d,0x4a,0x29,0x4b,0x1a,0x1d,0x26,0x4a,0x28,0x4a,0x1a,0x1d,0x29,0x4b,0x26,0x4a,0x15,0x28,0x4a,0x29,0x4b,0x1c,0x1f,0x2b,0x4a,0x28,0x4a,0x1c,0x1f,0x25,0x4b,0x26,0x4a,0x1c,0x1f,0x28,0x4a,0x25,0x4b,0x1a,0x1d,0x26,0x69,0x32,0x4a,0x33,0x4b,0x32,0x4a,0x30,0x4a,0x13,0x2e,0x4b,0x30,0x4a,0x16,0x1a,0x2e,0x4b,0x2d,0x4a,0x16,0x1a,0x2b,0x4a,0x2e,0x4b,0x16,0x1a,0x2d,0x4a,0x2b,0x4a,0x15,0x2a,0x4b,0x2b,0x4a,0x18,0x1a,0x2d,0x4a,0x2a,0x4b,0x18,0x1a,0x26,0x4a,0x28,0x4a,0x18,0x1a,0x2a,0x4b,0x26,0x4a,0x13,0x2b,0x4a,0x2a,0x4b,0x16,0x1a,0x2b,0x4a,0x2d,0x4a,0x16,0x1a,0x2e,0x4b,0x2d,0x4a,0x16,0x1a,0x2e,0x4a,0x30,0x4b,0x12,0x32,0x4a,0x31,0x4a,0x15,0x1a,0x32,0x4b,0x31,0x4a,0x15,0x1a,0x32,0x4a,0x33,0x4b,0x15,0x1a,0x32,0x4a,0x30,0x4b,0x13,0x2e,0x4a,0x30,0x4a,0x16,0x1a,0x2e,0x4b,0x2d,0x4a,0x16,0x1a,0x2b,0x4a,0x2e,0x4b,0x16,0x1a,0x2d,0x4a,0x2b,0x4a,0x15,0x29,0x4b,0x2b,0x4a,0x1a,0x1d,0x2d,0x4a,0x29,0x4b,0x1a,0x1d,0x26,0x4a,0x28,0x4a,0x1a,0x1d,0x29,0x4b,0x26,0x4a,0x15,0x28,0x4a,0x29,0x4b,0x1c,0x1f,0x2b,0x4a,0x28,0x4a,0x1c,0x1f,0x25,0x4b,0x26,0x4a,0x1c,0x1f,0x28,0x4a,0x25,0x4b,0x1a,0x1d,0x26,0x69,0x29,0x4b,0x27,0x4a,0x26,0x4a,0x24,0x4b,0x16,0x22,0x4a,0x24,0x4a,0x1a,0x1d,0x26,0x4b,0x27,0x4a,0x1a,0x1d,0x29,0x4a,0x2b,0x4b,0x1a,0x1d,0x2d,0x4a,0x2e,0x4a,0x18,0x2e,0x4b,0x2d,0x4a,0x1b,0x1d,0x2b,0x4a,0x29,0x4b,0x15,0x29,0x4a,0x27,0x4a,0x1b,0x1d,0x26,0x4b,0x24,0x4a,0x16,0x22,0x4a,0x24,0x4b,0x1a,0x1d,0x26,0x4a,0x27,0x4a,0x1a,0x1d,0x29,0x4b,0x2b,0x4a,0x1a,0x1d,0x2d,0x4a,0x2e,0x4b,0x11,0x2f,0x55,0x15,0x1b,0x30,0x54,0x15,0x1b,0x29,0x4b,0x27,0x4a,0x15,0x1b,0x26,0x4a,0x24,0x4b,0x16,0x22,0x4a,0x24,0x4a,0x1a,0x1d,0x26,0x4b,0x27,0x4a,0x1a,0x1d,0x29,0x4a,0x2b,0x4b,0x1a,0x1d,0x2d,0x4a,0x2e,0x4a,0x18,0x2e,0x4b,0x2d,0x4a,0x1b,0x1d,0x2b,0x4a,0x29,0x4b,0x15,0x29,0x4a,0x27,0x4a,0x1b,0x1d,0x26,0x4b,0x24,0x4a,0x16,0x26,0x4a,0x29,0x4b,0x13,0x22,0x4a,0x26,0x4a,0xf,0x24,0x4b,0x27,0x4a,0x11,0x21,0x4b,0x24,0x4a,0xa,0x22,0x55,0x16,0x54,0x32,0x4b,0x33,0x4a,0x32,0x4a,0x30,0x4b,0x13,0x2e,0x4a,0x30,0x4a,0x16,0x1a,0x2e,0x4b,0x2d,0x4a,0x16,0x1a,0x2b,0x4a,0x2e,0x4b,0x16,0x1a,0x2d,0x4a,0x2b,0x4a,0x15,0x2a,0x4b,0x2b,0x4a,0x18,0x1a,0x2d,0x4a,0x2a,0x4b,0x18,0x1a,0x26,0x4a,0x28,0x4a,0x18,0x1a,0x2a,0x4b,0x26,0x4a,0x13,0x2b,0x4a,0x2a,0x4b,0x16,0x1a,0x2b,0x4a,0x2d,0x4b,0x16,0x1a,0x2e,0x4a,0x2d,0x4a,0x16,0x1a,0x2e,0x4b,0x30,0x4a,0x12,0x32,0x4a,0x31,0x4b,0x15,0x1a,0x32,0x4a,0x31,0x4a,0x14,0x1a,0x32,0x4b,0x30,0x4a,0x13,0x1a,0x32,0x4a,0x2f,0x4b,0xc,0x33,0x4a,0x32,0x4a,0x13,0x18,0x33,0x4b,0x32,0x4a,0x13,0x18,0x33,0x4a,0x32,0x4b,0x13,0x18,0x33,0x4a,0x32,0x4a,0xc,0x33,0x4b,0x32,0x4a,0x15,0x18,0x30,0x4a,0x2e,0x4b,0x15,0x18,0x2d,0x4a,0x2e,0x4a,0x15,0x18,0x30,0x4b,0x2d,0x4a,0xe,0x2e,0x4b,0x30,0x4a,0x13,0x16,0x32,0x4a,0x2b,0x4b,0xe,0x2a,0x4a,0x2b,0x4a,0x15,0x18,0x2d,0x4b,0x2a,0x4a,0x13,0x16,0x2b,0x69,0x29,0x4b,0x27,0x4a,0x26,0x4a,0x24,0x4b,0x16,0x22,0x4a,0x24,0x4a,0x1a,0x1d,0x26,0x4b,0x27,0x4a,0x1a,0x1d,0x29,0x4a,0x2b,0x4b,0x1a,0x1d,0x2d,0x4a,0x2e,0x4a,0x18,0x2e,0x4b,0x2d,0x4a,0x1b,0x1d,0x2b,0x4a,0x29,0x4b,0x15,0x29,0x4a,0x27,0x4b,0x1b,0x1d,0x26,0x4a,0x24,0x4a,0x16,0x22,0x4b,0x24,0x4a,0x1a,0x1d,0x26,0x4a,0x27,0x4b,0x1a,0x1d,0x29,0x4a,0x2b,0x4a,0x1a,0x1d,0x2d,0x4b,0x2e,0x4a,0x11,0x2f,0x55,0x15,0x1b,0x30,0x54,0x15,0x1b,0x29,0x4b,0x27,0x4a,0x15,0x1b,0x26,0x4a,0x24,0x4b,0x16,0x22,0x4a,0x24,0x4a,0x1a,0x1d,0x26,0x4b,0x27,0x4a,0x1a,0x1d,0x29,0x4a,0x2b,0x4b,0x1a,0x1d,0x2d,0x4a,0x2e,0x4a,0x18,0x2e,0x4b,0x2d,0x4a,0x1b,0x1d,0x2b,0x4b,0x29,0x4a,0x15,0x29,0x4a,0x27,0x4b,0x1b,0x1d,0x26,0x4a,0x24,0x4a,0x16,0x26,0x4b,0x29,0x4a,0x13,0x22,0x4a,0x26,0x4b,0xf,0x24,0x4a,0x27,0x4a,0x11,0x21,0x4b,0x24,0x4a,0xa,0x22,0x55,0x16,0x54,0x32,0x4b,0x33,0x4a,0x32,0x4a,0x30,0x4b,0x13,0x2e,0x4a,0x30,0x4a,0x16,0x1a,0x2e,0x4b,0x2d,0x4a,0x16,0x1a,0x2b,0x4a,0x2e,0x4b,0x16,0x1a,0x2d,0x4a,0x2b,0x4b,0x15,0x2a,0x4a,0x2b,0x4a,0x18,0x1a,0x2d,0x4b,0x2a,0x4a,0x18,0x1a,0x26,0x4a,0x28,0x4b,0x18,0x1a,0x2a,0x4a,0x26,0x4a,0x13,0x2b,0x4b,0x2a,0x4a,0x16,0x1a,0x2b,0x4a,0x2d,0x4b,0x16,0x1a,0x2e,0x4a,0x2d,0x4a,0x16,0x1a,0x2e,0x4b,0x30,0x4a,0x12,0x32,0x4a,0x31,0x4b,0x15,0x1a,0x32,0x4a,0x31,0x4a,0x14,0x1a,0x32,0x4b,0x30,0x4a,0x13,0x1a,0x32,0x4a,0x2f,0x4b,0xc,0x33,0x4a,0x32,0x4a,0x13,0x18,0x33,0x4b,0x32,0x4a,0x13,0x18,0x33,0x4b,0x32,0x4a,0x13,0x18,0x33,0x4a,0x32,0x4b,0xc,0x33,0x4a,0x32,0x4a,0x15,0x18,0x30,0x4b,0x2e,0x4a,0x15,0x18,0x2d,0x4a,0x2e,0x4b,0x15,0x18,0x30,0x4a,0x2d,0x4a,0xe,0x2e,0x4b,0x30,0x4a,0x13,0x16,0x32,0x4a,0x2b,0x4b,0xe,0x2a,0x4a,0x2b,0x4a,0x15,0x18,0x2d,0x4b,0x2a,0x4a,0x13,0x16,0x2b,0x69,0x22,0x2e,0x55,0x24,0x30,0x55,0x16,0x26,0x32,0x55,0x16,0x54,0x16,0x22,0x2e,0x55,0x16,0x24,0x30,0x55,0x16,0x26,0x32,0x54,0x16,0x24,0x30,0x55,0x16,0x22,0x2e,0x55,0x16,0x21,0x2d,0x54,0xf,0x1f,0x2b,0x55,0xf,0x21,0x2d,0x55,0x10,0x22,0x2e,0x54,0x10,0x24,0x30,0x55,0x11,0x21,0x2d,0x55,0x11,0x1d,0x29,0x54,0x11,0x22,0x2e,0x55,0x11,0x24,0x30,0x55,0x16,0x26,0x32,0x55,0x16,0x54,0x16,0x22,0x2e,0x55,0x16,0x24,0x30,0x55,0x16,0x26,0x32,0x54,0x16,0x24,0x30,0x55,0x16,0x22,0x2e,0x55,0x16,0x21,0x2d,0x54,0xf,0x1f,0x2b,0x55,0xf,0x24,0x30,0x55,0x11,0x21,0x2d,0x54,0x11,0x1d,0x29,0x55,0xa,0x22,0x2e,0x6a,0x22,0x2e,0x54,0x24,0x30,0x55,0x16,0x26,0x32,0x55,0x16,0x54,0x16,0x22,0x2e,0x55,0x16,0x24,0x30,0x55,0x16,0x26,0x32,0x54,0x16,0x24,0x30,0x55,0x16,0x22,0x2e,0x55,0x16,0x21,0x2d,0x54,0xf,0x1f,0x2b,0x55,0xf,0x21,0x2d,0x55,0x10,0x22,0x2e,0x54,0x10,0x24,0x30,0x55,0x11,0x21,0x2d,0x55,0x11,0x1d,0x29,0x55,0x11,0x22,0x2e,0x54,0x11,0x24,0x30,0x55,0x16,0x26,0x32,0x55,0x16,0x54,0x16,0x22,0x2e,0x55,0x16,0x24,0x30,0x55,0x16,0x26,0x32,0x54,0x16,0x24,0x30,0x55,0x16,0x22,0x2e,0x55,0x16,0x21,0x2d,0x54,0xf,0x1f,0x2b,0x55,0xf,0x24,0x30,0x55,0x11,0x21,0x2d,0x55,0x11,0x1d,0x29,0x54,0xa,0x22,0x2e,0x6a,0x24,0x4a,0x22,0x4a,0x21,0x4b,0x22,0x4a,0x16,0x25,0x55,0x19,0x1d,0x54,0x19,0x1d,0x27,0x4b,0x25,0x4a,0x19,0x1d,0x24,0x4a,0x25,0x4b,0x16,0x29,0x54,0x19,0x1d,0x55,0x19,0x1d,0x2a,0x4a,0x29,0x4b,0x19,0x1d,0x28,0x4a,0x29,0x4a,0x16,0x30,0x4b,0x2e,0x4a,0x19,0x1d,0x2d,0x4b,0x2e,0x4a,0x16,0x30,0x4a,0x2e,0x4b,0x19,0x1d,0x2d,0x4a,0x2e,0x4a,0x16,0x31,0x55,0x19,0x1d,0x55,0x19,0x1d,0x2e,0x54,0x19,0x1d,0x31,0x55,0x11,0x2c,0x45,0x2e,0x45,0x30,0x4b,0x18,0x2b,0x2e,0x54,0x18,0x29,0x2c,0x55,0x18,0x2b,0x2e,0x55,0x11,0x2c,0x45,0x2e,0x45,0x30,0x4a,0x18,0x2b,0x2e,0x55,0x18,0x29,0x2c,0x55,0x18,0x2b,0x2e,0x55,0x11,0x2c,0x45,0x2e,0x45,0x30,0x4a,0x18,0x2b,0x2e,0x55,0xc,0x29,0x2c,0x55,0x18,0x28,0x2b,0x54,0x11,0x29,0x6a,0x24,0x4a,0x22,0x4a,0x21,0x4b,0x22,0x4a,0x16,0x25,0x55,0x19,0x1d,0x54,0x19,0x1d,0x27,0x4b,0x25,0x4a,0x19,0x1d,0x24,0x4a,0x25,0x4b,0x16,0x29,0x54,0x19,0x1d,0x55,0x19,0x1d,0x2a,0x4b,0x29,0x4a,0x19,0x1d,0x28,0x4a,0x29,0x4b,0x16,0x30,0x4a,0x2e,0x4a,0x19,0x1d,0x2d,0x4b,0x2e,0x4a,0x16,0x30,0x4a,0x2e,0x4b,0x19,0x1d,0x2d,0x4a,0x2e,0x4a,0x16,0x31,0x55,0x19,0x1d,0x55,0x19,0x1d,0x2e,0x54,0x19,0x1d,0x31,0x55,0x11,0x2c,0x45,0x2e,0x45,0x30,0x4b,0x18,0x2b,0x2e,0x54,0x18,0x29,0x2c,0x55,0x18,0x2b,0x2e,0x55,0x11,0x2c,0x45,0x2e,0x45,0x30,0x4b,0x18,0x2b,0x2e,0x54,0x18,0x29,0x2c,0x55,0x18,0x2b,0x2e,0x55,0x11,0x2c,0x45,0x2e,0x45,0x30,0x4a,0x18,0x2b,0x2e,0x55,0xc,0x29,0x2c,0x55,0x18,0x28,0x2b,0x54,0x11,0x29,0x6a,0x25,0x29,0x54,0x27,0x2a,0x55,0xd,0x29,0x2c,0x55,0x19,0x29,0x2c,0x54,0x11,0x2e,0x4b,0x2c,0x4a,0x1d,0x2a,0x4b,0x29,0x4a,0x14,0x24,0x27,0x55,0x20,0x54,0x25,0x29,0x55,0x27,0x2a,0x55,0xd,0x29,0x2c,0x54,0x19,0x29,0x2c,0x55,0x11,0x2e,0x4a,0x2c,0x4b,0x1d,0x2a,0x4a,0x29,0x4a,0x14,0x24,0x27,0x6a,0x22,0x25,0x54,0x24,0x27,0x55,0xa,0x25,0x29,0x55,0x16,0x25,0x29,0x55,0xd,0x2a,0x4a,0x29,0x4a,0x19,0x27,0x4b,0x25,0x4a,0x11,0x21,0x24,0x55,0x1d,0x54,0x22,0x25,0x55,0x24,0x27,0x55,0xa,0x25,0x29,0x54,0x16,0x25,0x29,0x55,0xd,0x2a,0x4a,0x29,0x4b,0x19,0x27,0x4a,0x25,0x4a,0x11,0x21,0x24,0x6a,0x24,0x4a,0x22,0x4a,0x21,0x4b,0x22,0x4a,0x16,0x25,0x55,0x19,0x1d,0x55,0x19,0x1d,0x27,0x4a,0x25,0x4a,0x19,0x1d,0x24,0x4b,0x25,0x4a,0x16,0x29,0x55,0x19,0x1d,0x54,0x19,0x1d,0x2a,0x4b,0x29,0x4a,0x19,0x1d,0x28,0x4a,0x29,0x4b,0x16,0x30,0x4a,0x2e,0x4a,0x19,0x1d,0x2d,0x4b,0x2e,0x4a,0x16,0x30,0x4a,0x2e,0x4b,0x19,0x1d,0x2d,0x4a,0x2e,0x4a,0x12,0x31,0x55,0x16,0x1c,0x55,0x16,0x1c,0x2e,0x55,0x16,0x1c,0x30,0x54,0x11,0x31,0x55,0x16,0x1d,0x30,0x55,0xf,0x2e,0x54,0x12,0x18,0x2d,0x55,0xd,0x2e,0x55,0x11,0x16,0x29,0x54,0xf,0x2a,0x55,0x12,0x18,0x27,0x55,0x11,0x16,0x25,0x54,0x11,0x16,0x55,0x11,0x15,0x25,0x45,0x24,0x45,0x25,0x46,0x24,0x45,0x11,0x15,0x25,0x45,0x24,0x45,0x22,0x45,0x24,0x45,0xa,0x16,0x22,0x6a,0x25,0x29,0x55,0x27,0x2a,0x54,0xd,0x29,0x2c,0x55,0x19,0x29,0x2c,0x55,0x11,0x2e,0x4a,0x2c,0x4a,0x1d,0x2a,0x4b,0x29,0x4a,0x14,0x24,0x27,0x55,0x20,0x54,0x25,0x29,0x55,0x27,0x2a,0x55,0xd,0x29,0x2c,0x54,0x19,0x29,0x2c,0x55,0x11,0x2e,0x4a,0x2c,0x4b,0x1d,0x2a,0x4a,0x29,0x4b,0x14,0x24,0x27,0x69,0x22,0x25,0x55,0x24,0x27,0x54,0xa,0x25,0x29,0x55,0x16,0x25,0x29,0x55,0xd,0x2a,0x4a,0x29,0x4a,0x19,0x27,0x4b,0x25,0x4a,0x11,0x21,0x24,0x55,0x1d,0x54,0x22,0x25,0x55,0x24,0x27,0x55,0xa,0x25,0x29,0x54,0x16,0x25,0x29,0x55,0xd,0x2a,0x4b,0x29,0x4a,0x19,0x27,0x4a,0x25,0x4b,0x11,0x21,0x24,0x69,0x24,0x4a,0x22,0x4b,0x21,0x4a,0x22,0x4a,0x16,0x25,0x55,0x19,0x1d,0x55,0x19,0x1d,0x27,0x4a,0x25,0x4a,0x19,0x1d,0x24,0x4b,0x25,0x4a,0x16,0x29,0x55,0x19,0x1d,0x54,0x19,0x1d,0x2a,0x4b,0x29,0x4a,0x19,0x1d,0x28,0x4a,0x29,0x4b,0x16,0x30,0x4a,0x2e,0x4b,0x19,0x1d,0x2d,0x4a,0x2e,0x4a,0x16,0x30,0x4b,0x2e,0x4a,0x19,0x1d,0x2d,0x4a,0x2e,0x4b,0x12,0x31,0x54,0x16,0x1c,0x55,0x16,0x1c,0x2e,0x55,0x16,0x1c,0x30,0x54,0x11,0x31,0x55,0x16,0x1d,0x30,0x55,0xf,0x2e,0x54,0x12,0x18,0x2d,0x55,0xd,0x2e,0x55,0x11,0x16,0x29,0x54,0xf,0x2a,0x55,0x12,0x18,0x27,0x55,0x11,0x16,0x25,0x55,0x11,0x16,0x54,0x11,0x15,0x25,0x45,0x24,0x46,0x25,0x45,0x24,0x45,0x11,0x15,0x25,0x45,0x24,0x45,0x22,0x45,0x24,0x46,0xa,0x16,0x22,0x69,0x22,0x4a,0x2e,0x4b,0x24,0x4a,0x30,0x4a,0x16,0x26,0x4b,0x32,0x4a,0x16,0x55,0x16,0x22,0x4a,0x2e,0x4a,0x16,0x24,0x4b,0x30,0x4a,0x16,0x26,0x4a,0x32,0x4b,0x16,0x24,0x4a,0x30,0x4b,0x16,0x22,0x4a,0x2e,0x4a,0x16,0x21,0x4b,0x2d,0x4a,0xf,0x1f,0x4a,0x2b,0x4b,0xf,0x21,0x4a,0x2d,0x4a,0x10,0x22,0x4b,0x2e,0x4a,0x10,0x24,0x4a,0x30,0x4b,0x11,0x21,0x4a,0x2d,0x4a,0x11,0x1d,0x4b,0x29,0x4a,0x11,0x22,0x4a,0x2e,0x4b,0x11,0x24,0x4a,0x30,0x4a,0x16,0x26,0x4b,0x32,0x4a,0x16,0x55,0x16,0x22,0x4a,0x2e,0x4a,0x16,0x24,0x4b,0x30,0x4a,0x16,0x26,0x4b,0x32,0x4a,0x16,0x24,0x4a,0x30,0x4b,0x16,0x22,0x4a,0x2e,0x4a,0x16,0x21,0x4b,0x2d,0x4a,0xf,0x1f,0x4a,0x2b,0x4b,0xf,0x24,0x4a,0x30,0x4a,0x11,0x21,0x4b,0x2d,0x4a,0x11,0x1d,0x4a,0x29,0x4b,0xa,0x22,0x2e,0x69,0x22,0x4a,0x2e,0x4b,0x24,0x4a,0x30,0x4a,0x16,0x26,0x4b,0x32,0x4a,0x16,0x55,0x16,0x22,0x4a,0x2e,0x4b,0x16,0x24,0x4a,0x30,0x4a,0x16,0x26,0x4b,0x32,0x4a,0x16,0x24,0x4a,0x30,0x4b,0x16,0x22,0x4a,0x2e,0x4a,0x16,0x21,0x4b,0x2d,0x4a,0xf,0x1f,0x4a,0x2b,0x4b,0xf,0x21,0x4a,0x2d,0x4a,0x10,0x22,0x4b,0x2e,0x4a,0x10,0x24,0x4a,0x30,0x4b,0x11,0x21,0x4a,0x2d,0x4a,0x11,0x1d,0x4b,0x29,0x4a,0x11,0x22,0x4a,0x2e,0x4b,0x11,0x24,0x4a,0x30,0x4a,0x16,0x26,0x4b,0x32,0x4a,0x16,0x55,0x16,0x22,0x4a,0x2e,0x4b,0x16,0x24,0x4a,0x30,0x4a,0x16,0x26,0x4b,0x32,0x4a,0x16,0x24,0x4a,0x30,0x4b,0x16,0x22,0x4a,0x2e,0x4a,0x16,0x21,0x4b,0x2d,0x4a,0xf,0x1f,0x4a,0x2b,0x4b,0xf,0x24,0x4a,0x30,0x4a,0x11,0x21,0x4b,0x2d,0x4a,0x11,0x1d,0x4a,0x29,0x4b,0x16,0x22,0x2e,0x54,0x16,0x55,0x16,0x26,0x32,0x55,0x16,0x4a,0x32,0x4b,0x16,0x26,0x43,0x29,0x43,0x2e,0x44,0x32,0x4a,0x16,0x55,0x16,0x55,0x16,0x54,0x16,0x26,0x44,0x29,0x43,0x2e,0x44,0x32,0x4a,0x16,0x55,0x16,0x54,0x16,0x55,0x16,0x33,0x4a,0x32,0x4b,0x16,0x30,0x4a,0x32,0x4a,0x16,0x33,0x4b,0x32,0x4a,0x16,0x30,0x4a,0x32,0x4b,0xf,0x2e,0x33,0x54,0xf,0x55,0xf,0x55,0xf,0x4f,0x33,0x46,0x16,0x2e,0x32,0x4f,0x33,0x45,0x16,0x2e,0x32,0x50,0x33,0x45,0x16,0x2e,0x32,0x4f,0x33,0x46,0x16,0x2e,0x32,0x54,0x11,0x2d,0x30,0x55,0x11,0x55,0x11,0x54,0x11,0x35,0x55,0x16,0x26,0x44,0x29,0x43,0x2e,0x43,0x32,0x4b,0x16,0x54,0x16,0x55,0x16,0x55,0x16,0x26,0x43,0x29,0x44,0x2e,0x43,0x32,0x4b,0x16,0x54,0x16,0x55,0x16,0x55,0x16,0x33,0x4a,0x32,0x4a,0x16,0x30,0x4b,0x32,0x4a,0x16,0x33,0x4a,0x32,0x4b,0x16,0x30,0x4a,0x32,0x4a,0xf,0x2e,0x33,0x55,0xf,0x55,0xf,0x54,0xf,0x50,0x33,0x45,0x16,0x2e,0x32,0x55,0x16,0x54,0x16,0x55,0x16,0x50,0x32,0x45,0x11,0x2d,0x30,0x4f,0x32,0x46,0x11,0x2d,0x30,0x4f,0x32,0x45,0x11,0x2d,0x30,0x50,0x32,0x45,0x11,0x2d,0x30,0x55,0x16,0x2e,0x4a,0x1d,0x4a,0x1a,0x4b,0x1d,0x4a,0x16,0x29,0x4a,0x1d,0x2e,0x4b,0x1a,0x32,0x4a,0x1d,0x32,0x4a,0x16,0x29,0x4b,0x1d,0x2e,0x4a,0x1a,0x32,0x4a,0x1d,0x4b,0x16,0x4a,0x1d,0x4a,0x1a,0x4b,0x1d,0x4a,0x16,0x29,0x4a,0x1d,0x2e,0x4b,0x1a,0x32,0x4a,0x1d,0x4b,0x16,0x4a,0x1d,0x4a,0x1a,0x4b,0x1d,0x4a,0x16,0x33,0x4a,0x1d,0x32,0x4b,0x1a,0x30,0x4a,0x1d,0x32,0x4a,0x16,0x33,0x4b,0x1d,0x32,0x4a,0x1a,0x30,0x4a,0x1d,0x32,0x4b,0x16,0x33,0x4a,0x1f,0x4a,0x1b,0x4b,0x1f,0x4a,0x16,0x4a,0x1f,0x4b,0x1b,0x4a,0x1f,0x45,0x33,0x45,0x16,0x32,0x4b,0x1d,0x45,0x33,0x45,0x1a,0x32,0x4a,0x1d,0x46,0x33,0x45,0x16,0x32,0x4a,0x1d,0x45,0x33,0x45,0x1a,0x32,0x4b,0x1d,0x4a,0x11,0x30,0x4b,0x1d,0x4a,0x18,0x4a,0x1d,0x4b,0x11,0x4a,0x1d,0x4a,0x18,0x35,0x4b,0x1d,0x4a,0x16,0x26,0x43,0x29,0x44,0x2e,0x43,0x32,0x4b,0x16,0x54,0x16,0x55,0x16,0x55,0x16,0x26,0x43,0x29,0x44,0x2e,0x43,0x32,0x4a,0x16,0x55,0x16,0x55,0x16,0x54,0x16,0x33,0x4b,0x32,0x4a,0x16,0x30,0x4a,0x32,0x4b,0x16,0x33,0x4a,0x32,0x4b,0x16,0x30,0x4a,0x32,0x4a,0xf,0x2e,0x33,0x55,0xf,0x55,0xf,0x54,0xf,0x50,0x33,0x45,0x16,0x2e,0x32,0x55,0x16,0x54,0x16,0x55,0x16,0x50,0x32,0x45,0x11,0x2d,0x30,0x4f,0x32,0x45,0x11,0x2d,0x30,0x50,0x32,0x45,0x11,0x2d,0x30,0x50,0x32,0x45,0x11,0x2d,0x30,0x54,0x16,0x29,0x2e,0x55,0x16,0x55,0x16,0x55,0x16,0x26,0x32,0x54,0x16,0x22,0x2e,0x55,0x16,0x55,0x16,0x54,0x16,0x29,0x35,0x55,0x16,0x22,0x2e,0x55,0x16,0x54,0x16,0x55,0x16,0x26,0x32,0x55,0x16,0x22,0x2e,0x54,0x16,0x26,0x32,0x55,0x16,0x22,0x2e,0x55,0x16,0x29,0x35,0x55,0xa,0x22,0x2e,0x69,0xa,0x29,0x2e,0x69,0xa,0x29,0x2e,0x6a,0xff};
