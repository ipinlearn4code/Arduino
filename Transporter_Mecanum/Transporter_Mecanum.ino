#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver();

int delayTime = 500;  // default delay dalam ms
bool increasing = true;
int servoPos = 1000;

void setup() {
  Serial.begin(9600);
  pca.begin();
  pca.setPWMFreq(50);  // 50Hz cocok untuk servo
  Serial.println("Masukkan delay dalam ms (contoh: 300):");
}

void loop() {
  // Baca input dari Serial Monitor
  if (Serial.available()) {
    int input = Serial.parseInt();
    if (input > 0) {
      delayTime = input;
      Serial.print("Delay diatur ke: ");
      Serial.print(delayTime);
      Serial.println(" ms");
    }
  }

  // Kirim sinyal ke Channel 0 dan 1
  int tick = usToTicks(servoPos);
  pca.setPWM(0, 0, tick);  // Servo di Channel 0
  pca.setPWM(1, 0, tick);  // Servo di Channel 1

  // Pergerakan bolak-balik (wiper)
  if (increasing) {
    servoPos += 10;
    if (servoPos >= 2000) {
      servoPos = 2000;
      increasing = false;
    }
  } else {
    servoPos -= 10;
    if (servoPos <= 1000) {
      servoPos = 1000;
      increasing = true;
    }
  }

  delay(delayTime);  // jeda antar gerakan
}

// Konversi microsecond ke tick PCA9685 (1 tick ≈ 4.88us)
int usToTicks(int us) {
  return map(us, 1000, 2000, 205, 410);
}
