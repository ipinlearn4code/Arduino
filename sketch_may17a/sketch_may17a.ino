int servoPin = 10; // Gunakan pin 10 untuk Servo 1
int pulseWidth = 1500; // Default 1500 µs (90°)

void setup() {
  pinMode(servoPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("Masukkan nilai pulseWidth (us), misalnya 1000 - 2000:");
}

void loop() {
  // Kirim pulsa PWM ke servo
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(pulseWidth);
  digitalWrite(servoPin, LOW);
  delay(20 - pulseWidth / 1000); // Total periode 20ms

  // Baca input serial
  if (Serial.available()) {
    int val = Serial.parseInt();
    if (val >= 500 && val <= 2500) { // batas aman
      pulseWidth = val;
      Serial.print("PWM diatur ke: ");
      Serial.print(pulseWidth);
      Serial.println(" us");
    } else {
      Serial.println("Nilai di luar jangkauan (500 - 2500 us)");
    }
  }
}
