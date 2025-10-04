#define LED_PIN 18

void setup() {
  ledcSetup(0, 5000, 8);       // channel 0, 5 kHz, 8-bit
  ledcAttachPin(LED_PIN, 0);   // pin 18 ke channel 0
  ledcWrite(0, 128);           // set duty cycle 50%
}

void loop() {}
