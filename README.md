# music-box
Chiptune music player featuring SN76489 and ATTiny861

## Overview
This project grew out of a Wikipedia surfing session. In reading about the computers of my childhood,
I came across an article about the TI SN76489 sound chip, used by two of the machines I used as a kid
(the TI 99/4A and IBM PCjr). I wondered if you can readily buy this IC, and if you can talk to it
reasonably easily with modern hardware. The answer to both questions is yes!

## How it works
All eight pins of PORTA are connected to the SN76489's data lines. PB4 connects to /WE and latches
a value to the chip. PB5 (CLKO) outputs the ATtiny861's clock signal directly to the sound chip's
clock input. /OE is permanently tied low and READY is ignored. I'm not sharing a bus with anything 
else...

The MCU firmware plays music from an array of bytes stored in program memory (in songs.c). It's a
very simple binary format with three one-byte commands that play a note, play a noise, and sleep.
To keep things simple, I ignore note-off events and simply make each note decay at a constant
linear rate after it is struck. Much like an actual music box!

## Loading music
util/process_midi_file.cpp will take a MIDI file and output a set of arrays for songs.c. It makes
use of [light-midi-parser](https://github.com/imhcyx/light-midi-parser) as a submodule.

## See also
I've also created a more elaborate stereo two-chip player using the Raspberry Pi Pico, [pico-player](https://github.com/jstanley0/pico-player).
