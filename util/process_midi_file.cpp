#include <iostream>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <set>
extern "C" {
#include "midi.h"
}

const double AVR_CLOCK = 2000000.0;
// the lowest achievable frequency is AVR_CLOCK / 32768 or 61Hz, just below B1
const int LOWEST_NOTE = 35; // B1
const int HIGHEST_NOTE = LOWEST_NOTE + 63; // D7

struct Event {
  int delay;
  std::set<int> notes_off;
  std::set<int> notes_on;

  explicit Event(int _delay = 0) : delay(_delay) {}
};

class Encoder {
  std::vector<Event> song_data;
  std::set<int> notes_playing;

  std::ostream &os;
  int min_note, max_note;

  void write1(int data) {
    os << "0x" << std::hex << data << ",";
  }

  int process_note(int note) {
    // with 6 bits we are limited to a bit more than 5 octaves
    // so we'll just shift notes that are out of range into an octave we have
    while(note < LOWEST_NOTE)
      note += 12;
    while(note > HIGHEST_NOTE)
      note -= 12;
    return note - LOWEST_NOTE;
  }

  void write_note_on(int note) {
    note = process_note(note);
    write1(note);
  }

  void write_delay(int ms) {
    ms = ((ms + 5) / 10);
    while(ms > 63) {
      write1(0x40 | ms);
      ms -= 63;
    }
    write1(0x40 | ms);
  }

  void write_note_off(int note) {
    note = process_note(note);
    write1(0x80 | note);
  }

  void write_note_table() {
    os << "const uint16_t note_table[64] PROGMEM = {";
    for(int i = 0; i < 64; ++i) {
      os << (int)(AVR_CLOCK / (32 * 440.0 * pow(2, ((i + LOWEST_NOTE) - 69.0) / 12))) << ",";
    }
    os << "};\n";
  }

  void write_song_data() {
    process_last_event();
    os << "const uint8_t song_data[] PROGMEM = {";
    for(auto&& evt : song_data) {
      write_delay(evt.delay);
      for(auto note : evt.notes_off)
        write_note_off(note);
      // for notes on, prioritize higher notes
      for(auto ni = evt.notes_on.rbegin(); ni != evt.notes_on.rend(); ++ni)
        write_note_on(*ni);
    }
    os << "0xff};\n";
  }

  void ensure_event() {
    if (song_data.empty()) {
      song_data.emplace_back(0);
    }
  }

  void process_last_event() {
    if (song_data.empty())
      return;
    auto &event = song_data.back();

    // process note-off events
    for(auto ni = event.notes_off.begin(); ni != event.notes_off.end();) {
      auto pi = notes_playing.find(*ni);
      if (pi != notes_playing.end()) {
        notes_playing.erase(pi);
        ++ni;
      } else {
        // this note was previously dropped, so remove the now spurious note-off event
        ni = event.notes_off.erase(ni);
      }
    }

    // if no notes are being added in this event, we're done
    if (event.notes_on.empty())
      return;

    // if there are too many notes to actually turn on right now,
    // cull some of them (TODO: try to also be smart about notes that are already playing)
    while(event.notes_on.size() > 3) {
      event.notes_on.erase(std::next(event.notes_on.begin()));
    }

    // insert the new notes into the playing set
    for(int note : event.notes_on) {
      notes_playing.insert(note);
    }
  }

public:
  Encoder(std::ostream &s = std::cout) : os(s), min_note(128), max_note(0) {
  }

  void log_delay(int ms) {
    process_last_event();
    song_data.emplace_back(ms);
  }

  void log_note_off(int channel, int note, int velocity) {
    ensure_event();
    song_data.back().notes_off.insert(note);
  }

  void log_note_on(int channel, int note, int velocity) {
    if (channel == 9)
      return; // TODO: map percussion events to the noise channel
    if (velocity == 0) {
      // some MIDI files use note-on events with velocity 0 as note-off events
      log_note_off(channel, note, velocity);
      return;
    }
    ensure_event();
    if (note < min_note) min_note = note;
    if (note > max_note) max_note = note;
    song_data.back().notes_on.insert(note);
  }

  void write_song() {
    write_note_table();
    write_song_data();
  }

  ~Encoder() {
    std::cerr << "encoder stats:\n";
    std::cerr << "note range " << min_note << " - " << max_note << " (" << max_note - min_note + 1 << ")\n";
  }
};

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: playmidi file.mid" << std::endl;
    return 1;
  }

  midi_file *midi = midi_open_file(argv[1]);
  if (!midi) {
    std::cerr << "Failed to open file " << argv[1] << std::endl;
    return 1;
  }

  midi_trks *trks = midi_parse_tracks(midi);
  if (!trks) {
    std::cerr << "Failed to parse MIDI file" << std::endl;
    return 1;
  }

  midi_combine_tracks(trks);
  midi_convert_deltatime(trks);

  midi_hdr hdr;
  midi_get_header(midi, &hdr);

  int tempo = 500000;

  Encoder encoder;
  for(midi_evt_node *node = trks->trk[0]; node != NULL; node = node->next) {
    if (node->time > 0) {
      int us = (node->time * tempo) / hdr.division;
      encoder.log_delay(us / 1000);
    }
    switch(node->evt) {
    case midi_meta_evt:
      if (node->meta == midi_set_tempo) {
        tempo = node->param1;
      }
      break;
    case midi_noteoff:
      encoder.log_note_off(node->chan, node->param1, node->param2);
      break;
    case midi_noteon:
      encoder.log_note_on(node->chan, node->param1, node->param2);
      break;
    }
  }
  encoder.write_song();
  midi_free_tracks(trks);
  midi_close(midi);
  return 0;
}
