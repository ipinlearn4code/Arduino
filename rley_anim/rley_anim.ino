const int relayPins[] = { 22, 21, 5, 4 };
const int numRelays = sizeof(relayPins) / sizeof(relayPins[0]);
const int pinled = 2;

void setup() {
  pinMode(pinled, OUTPUT);
  for (int i = 0; i < numRelays; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW);  // Make sure all relays are off initially
  }
}

void loop() {
  // Turn relays ON one by one
  for (int i = 0; i < numRelays; i++) {
    digitalWrite(relayPins[i], HIGH);  // Turn ON relay
    digitalWrite(pinled, HIGH);  // Turn ON relay

    delay(300);                        // Wait
    digitalWrite(relayPins[i], LOW);   // Turn OFF relay
  }

  // Reverse direction (optional for cooler animation)
  for (int i = numRelays - 1; i >= 0; i--) {
    digitalWrite(relayPins[i], HIGH);
    digitalWrite(pinled, LOW);
    delay(300);
    digitalWrite(relayPins[i], LOW);
  }

  delay(500);  // Small pause between loops
}
