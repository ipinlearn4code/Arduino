#include <AccelStepper.h>

// STEP dan DIR pins untuk RAMPS 1.5
#define M1_STEP_PIN 54  // X (Kiri Depan)
#define M1_DIR_PIN  55

#define M2_STEP_PIN 60  // Y (Kiri Belakang)
#define M2_DIR_PIN  61

#define M3_STEP_PIN 36  // E1 (kanan Depan)
#define M3_DIR_PIN  34

#define M4_STEP_PIN 26  // E0 (kanan Belakang)
#define M4_DIR_PIN  28
//Note : Semua posisi stepper ada diatas
#define X_ENABLE_PIN 38
#define Y_ENABLE_PIN 56
#define E0_ENABLE_PIN 24
#define E1_ENABLE_PIN 30

AccelStepper motor1(AccelStepper::DRIVER, M1_STEP_PIN, M1_DIR_PIN); // X
AccelStepper motor2(AccelStepper::DRIVER, M2_STEP_PIN, M2_DIR_PIN); // Y
AccelStepper motor3(AccelStepper::DRIVER, M3_STEP_PIN, M3_DIR_PIN); // E1
AccelStepper motor4(AccelStepper::DRIVER, M4_STEP_PIN, M4_DIR_PIN); // E0

long posisiTarget = 8000;
bool forward = true;

float currentSpeed = 500; // Awal default
unsigned long lastPrint = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Aktifkan semua ENABLE pin
  pinMode(X_ENABLE_PIN, OUTPUT);
  pinMode(Y_ENABLE_PIN, OUTPUT);
  pinMode(E0_ENABLE_PIN, OUTPUT);
  pinMode(E1_ENABLE_PIN, OUTPUT);

  digitalWrite(X_ENABLE_PIN, LOW);
  digitalWrite(Y_ENABLE_PIN, LOW);
  digitalWrite(E0_ENABLE_PIN, LOW);
  digitalWrite(E1_ENABLE_PIN, LOW);

  setMaxSpeedAll(currentSpeed);
  setAccelerationAll(800); // Sesuaikan dengan motor kamu

  Serial.println("Masukkan nilai kecepatan (contoh: 1500), lalu tekan ENTER.");

  setAllTarget(posisiTarget);
}

void loop() {
  // Jalankan motor
  motor1.run();
  motor2.run();
  motor3.run();
  motor4.run();

  // Tampilkan status berkala
  if (millis() - lastPrint > 500) {
    Serial.print("Posisi X: ");
    Serial.print(motor1.currentPosition());
    Serial.print("\tSpeed: ");
    Serial.println(currentSpeed);
    lastPrint = millis();
  }

  // Ganti arah jika sampai tujuan
  if (motorsAtTarget()) {
    delay(300);
    forward = !forward;
    posisiTarget = forward ? 8000 : 0;
    setAllTarget(posisiTarget);
  }

  // Cek input kecepatan dari serial
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() > 0) {
      float newSpeed = input.toFloat();
      if (newSpeed > 0 && newSpeed <= 100000) {
        currentSpeed = newSpeed;
        setMaxSpeedAll(currentSpeed);
        Serial.print("✅ Kecepatan diperbarui: ");
        Serial.println(currentSpeed);
      } else {
        Serial.println("❌ Masukkan angka kecepatan antara 1 - 100000.");
      }
    }
  }
}

void setAllTarget(long pos) {
  motor1.moveTo(pos);
  motor2.moveTo(pos);
  motor3.moveTo(pos);
  motor4.moveTo(pos);
}

bool motorsAtTarget() {
  return motor1.distanceToGo() == 0 &&
         motor2.distanceToGo() == 0 &&
         motor3.distanceToGo() == 0 &&
         motor4.distanceToGo() == 0;
}

void setMaxSpeedAll(float speed) {
  motor1.setMaxSpeed(speed);
  motor2.setMaxSpeed(speed);
  motor3.setMaxSpeed(speed);
  motor4.setMaxSpeed(speed);
}

void setAccelerationAll(float accel) {
  motor1.setAcceleration(accel);
  motor2.setAcceleration(accel);
  motor3.setAcceleration(accel);
  motor4.setAcceleration(accel);
}
