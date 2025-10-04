// Pin LED built-in pada ESP32 biasanya di GPIO 2
#define LED_BUILTIN 2

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  // Set pin LED sebagai output
}

void loop() {
  for (int i = 0; i < 6; i++) {
    digitalWrite(LED_BUILTIN, HIGH);  // Nyalakan LED
    delay(150);                       // Tunggu 1 detik
    digitalWrite(LED_BUILTIN, LOW);   // Matikan LED
    delay(150);                       // Tunggu 1 detik
  }
  delay(250);
}
