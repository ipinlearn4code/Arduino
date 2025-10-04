#include <MIDI.h>

void setup() {
  MIDI.begin(1);
}

void loop() {
  MIDI.sendNoteOn(60, 127, 1);  // Note C4, velocity 127, channel 1
  delay(1000);
  MIDI.sendNoteOff(60, 0, 1);
  delay(1000);
}

