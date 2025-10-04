#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca9685 = Adafruit_PWMServoDriver();

#define SERVO_CHANNEL 0  // Ubah ke channel servo yang ingin dikalibrasi

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);  // Default I2C ESP32
  pca9685.begin();
  pca9685.setPWMFreq(50); // 50Hz untuk servo

  Serial.println("Masukkan nilai PWM (tick) antara 100 - 600 untuk kalibrasi:");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    int pwmTick = input.toInt();

    // Validasi batas aman untuk servo
//    if (pwmTick >= 100 && pwmTick <= 600) {
    if (true) {
      pca9685.setPWM(SERVO_CHANNEL, 0, pwmTick);
      Serial.print("PWM Tick diset ke: ");
      Serial.println(pwmTick);
    } else {
      Serial.println("❌ Nilai harus antara 100 - 600");
    }
  }
}
