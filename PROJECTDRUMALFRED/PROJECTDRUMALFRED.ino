#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

void setup() {
  Serial.begin(115200); // Inisialisasi komunikasi serial
}

void loop() {
  int threshold = 10;
  int kick = analogRead(A2);
  
  if (kick > threshold) {
    //int velo = sensorValue*3;
    int velo = 80;
    midiNoteOn(34, velo);
  }
  int velo = 80;
  midiNoteOn(34, velo);

  delay(1000);
}

void midiNoteOn(byte note, byte midiVelocity) {
  MIDI.sendNoteOn(note, midiVelocity, 10);
  Serial.println("MIDI Note On sent.");
}

void midiNoteOff(byte note, byte midiVelocity) {
  MIDI.sendNoteOff(note, midiVelocity, 10);
  Serial.println("MIDI Note Off sent.");
}
