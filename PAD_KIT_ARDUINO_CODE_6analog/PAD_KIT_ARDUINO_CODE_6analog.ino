#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

const int kickPin = A0;
const int snarePin = A1;
const int hihatPin = A2;
const int tomPin = A3;
const int floorPin = A4;
const int ridePin = A5;

int triggerValue [] = {}
int midiMap [] = {}

boolean isKickHitted = false;
boolean isSnareHitted = false;
boolean isHihatHitted = false;
boolean isTomHitted = false;
boolean isFloorlHitted = false;
boolean isRideHitted = false;
boolean isDelay = false;
int threshold = 5;

void setup() {
  Serial.begin(115200);
}

void loop() {
  int kickValue = analogRead(kickPin);
  int snareValue = analogRead(snarePin);
  int hihatValue = analogRead(hihatPin);
  int tomValue = analogRead(tomPin);
  int floorValue = analogRead(floorPin);
  int rideValue = analogRead(ridePin);

  //kick
  if (kickValue > threshold && !isKickHitted){
    MIDI.sendNoteOn(36, 2*kickValue, 10);
    isKickHitted = true;
    isDelay = true;
  }else{
    isKickHitted = false;
  }

  //snare
  if (snareValue > threshold && !isSnareHitted) {
    MIDI.sendNoteOn(38, 2*snareValue, 10);
    isSnareHitted = true;
    isDelay = true;
  }else {
    isSnareHitted = false;
  }

  //hihat
  if (hihatValue > threshold && !isHihatHitted) {
    MIDI.sendNoteOn(66, 2*hihatValue, 10);
    isHihatHitted = true;
    isDelay = true;
  }else {
    isHihatHitted = false;
  }

  //floor
  if (floorValue > threshold && !isFloorlHitted) {
    MIDI.sendNoteOn(43, 2*floorValue, 10);
    isFloorlHitted = true;
    isDelay = true;
  }else {
    isFloorlHitted = false;
  }

  //tom
  if (tomValue > threshold && !isTomHitted) {
    MIDI.sendNoteOn(45, 2*tomValue, 10);
    isTomHitted = true;
    isDelay = true;
  }else {
    isTomHitted = false;
  }

  //ride
  if (rideValue > threshold && !isRideHitted) {
    MIDI.sendNoteOn(52, 2*rideValue, 10);
    isRideHitted = true;
    isDelay = true;
  }else {
    isRideHitted = false;
  }

  if (isDelay) {
    delay(2);
  }
  isDelay = false;
}

bool func(int value,int map,bool isUsed){
   if(value > theshold){
        isUsed = true;
    }else{
        isUsed = false;
    }
    if(isUsed){
         MIDI.sendNoteOn(map, 2*value, 10);
    }
 return isUsed;
}