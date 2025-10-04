#include <ESP32PWM.h>

#define NUM_SERVOS 4

const int servoPins[NUM_SERVOS] = {15, 16, 17, 18};

// Kalibrasi min dan max pulse (dalam mikrodetik)
const int servoMin[NUM_SERVOS] = {1000, 450, 1200, 1500};
const int servoMax[NUM_SERVOS] = {2550, 950, 2000, 2000};

ESP32PWM servos[NUM_SERVOS];

const int pwmFreq = 50;
const int pwmRes = 16;

uint32_t usToDuty(int us, int freq, int resBits) {
  uint32_t maxDuty = (1 << resBits) - 1;
  uint32_t period = 1000000UL / freq;
  return (us * maxDuty) / period;
}

int angleToPulse(int servoIndex, int angle) {
  angle = constrain(angle, 0, 180);
  int us = map(angle, 0, 180, servoMin[servoIndex], servoMax[servoIndex]);
  return us;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Ketik: <index> <angle>");
  Serial.println("Contoh: 0 90");

  for (int i = 0; i < NUM_SERVOS; i++) {
    servos[i].attachPin(servoPins[i], pwmFreq, pwmRes);
    int mid = (servoMin[i] + servoMax[i]) / 2;
    servos[i].write(usToDuty(mid, pwmFreq, pwmRes)); // ke posisi tengah saat start
  }
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() == 0) return;

    int sepIndex = input.indexOf(' ');
    if (sepIndex == -1) {
      Serial.println("Format salah. Gunakan: <servoIndex> <angle>");
      return;
    }

    int servoIndex = input.substring(0, sepIndex).toInt();
    int angle = input.substring(sepIndex + 1).toInt();

    if (servoIndex >= 0 && servoIndex < NUM_SERVOS) {
      int pulse = angleToPulse(servoIndex, angle);
      servos[servoIndex].write(usToDuty(pulse, pwmFreq, pwmRes));
      Serial.print("Servo ");
      Serial.print(servoIndex);
      Serial.print(" ke sudut ");
      Serial.print(angle);
      Serial.print(" (");
      Serial.print(pulse);
      Serial.println(" us)");
    } else {
      Serial.println("Servo index di luar jangkauan (0–3).");
    }
  }
}
