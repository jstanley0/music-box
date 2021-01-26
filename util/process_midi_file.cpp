#include <iostream>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <set>
#include <cstdarg>
extern "C" {
#include "midi.h"
}

const double AVR_CLOCK = 2000000.0;
// the lowest achievable frequency is AVR_CLOCK / 32768 or 61Hz, just below B1
const int LOWEST_NOTE = 35; // B1
const int HIGHEST_NOTE = LOWEST_NOTE + 63; // D7

const int POLYPHONY = 3;

struct Event {
  int delay;
  std::set<int> notes;
  std::set<int> noise;

  explicit Event(int _delay = 0) : delay(_delay) {}
};

class Encoder {
  std::vector<Event> song_data;

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

  void write_note(int note) {
    note = process_note(note);
    write1(note);
  }

  void write_noise(int noise) {
    write1(0x80 | noise);
  }

  void write_delay(int cs) {
    while(cs > 63) {
      write1(0x7F);
      cs -= 63;
    }
    if (cs > 0)
      write1(0x40 | cs);
  }

  bool percussion_hit(const std::set<int> &perc, int dummy ...) {
    va_list args;
    va_start(args, dummy);
    int i = 0;
    while((i = va_arg(args, int)) > 0) {
      if (perc.end() != perc.find(i))
        return true;
    }
    return false;
  }

  int map_noise(const std::set<int> &perc) {
    // since we can only hear one noise at a time, these are prioritized
    if (percussion_hit(perc, 0, 37, 38, 39, 40, 49, 52, 55, 57 -1))
      return 4; // snare-ish
    else if (percussion_hit(perc, 0, 35, 36, 41, 45, -1))
      return 6; // bass drum-ish
    else if (percussion_hit(perc, 0, 47, 61, 64, 66, 68, 77, -1))
      return 2; // low tom, etc.
    else if (percussion_hit(perc, 0, 48, 60, 62, 63, 65, 67, 76, -1))
      return 1; // mid tom, etc.
    if (percussion_hit(perc, 0, 50, 56, 58, 71, 72, 80, 81, -1))
      return 0; // hi tom, etc.
    else if (percussion_hit(perc, 0, 42, 44, 46, 51, 53, 54, 55, 59, 70, -1))
      return 5; // misc (hi-hat, etc.)
    return -1;
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
      if (evt.delay > 0)
        write_delay(evt.delay);
      for(auto note : evt.notes)
        write_note(note);
      if (!evt.noise.empty()) {
        int noise = map_noise(evt.noise);
        if (noise >= 0)
          write_noise(noise);
      }
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

    // if there are too many notes to strike at once, cull some of them
    // leaving the highest and lowest notes alone
    while(event.notes.size() > POLYPHONY) {
      event.notes.erase(std::next(event.notes.begin()));
    }
  }

public:
  Encoder(std::ostream &s = std::cout) : os(s), min_note(128), max_note(0) {
  }

  void log_delay(int cs) {
    if (!song_data.empty() && song_data.back().notes.empty()) {
      song_data.back().delay += cs;
    } else {
      process_last_event();
      song_data.emplace_back(cs);
    }
  }

  void log_note(int channel, int note, int velocity) {
    if (velocity == 0)
      return; // some MIDI files use note-on events with velocity 0 as note-off events

    ensure_event();
    if (channel == 9) {
      song_data.back().noise.insert(note);
    } else {
      if (note < min_note) min_note = note;
      if (note > max_note) max_note = note;
      song_data.back().notes.insert(note);
    }
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
  midi_convert_abstime(trks);

  midi_hdr hdr;
  midi_get_header(midi, &hdr);

  int tempo = 500000;
  int last_timestamp = 0;

  Encoder encoder;
  for(midi_evt_node *node = trks->trk[0]; node != NULL; node = node->next) {
    int timestamp = ((int64_t)node->time * tempo) / hdr.division / 10000;  // ms to hundredths
    if (timestamp > last_timestamp) {
      encoder.log_delay(timestamp - last_timestamp);
      last_timestamp = timestamp;
    }
    switch(node->evt) {
    case midi_meta_evt:
      if (node->meta == midi_set_tempo) {
        tempo = node->param1;
      }
      break;
    case midi_noteon:
      encoder.log_note(node->chan, node->param1, node->param2);
      break;
    }
  }
  encoder.write_song();
  midi_free_tracks(trks);
  midi_close(midi);
  return 0;
}
