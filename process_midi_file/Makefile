CFLAGS := -g -I./light-midi-parser -Wno-switch
CXXFLAGS := -std=c++11
LFLAGS := -pthread

all: process_midi_file

process_midi_file: process_midi_file.o midi.o
	g++ $(LFLAGS) process_midi_file.o midi.o -o process_midi_file

midi.o: ./light-midi-parser/midi.c
	gcc $(CFLAGS) -c light-midi-parser/midi.c

process_midi_file.o: process_midi_file.cpp light-midi-parser/midi.c
	g++ $(CFLAGS) $(CXXFLAGS) -c process_midi_file.cpp

clean:
	rm -f *.o process_midi_file

