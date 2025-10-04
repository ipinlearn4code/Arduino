#include <PS4Controller.h>

// Pin motor kiri
#define RPWM_KIRI 18
#define LPWM_KIRI 19

// Pin motor kanan
#define RPWM_KANAN 16
#define LPWM_KANAN 17

// Kecepatan maksimum default
#define DEFAULT_MAX_PWM 100
#define MEDIUM_MAX_PWM 180
#define MAX_PWM 255
#define TURN_SPEED 180  // Speed tetap untuk belok

void setup() {
  Serial.begin(115200);
  PS4.begin();
  Serial.println("Menunggu PS4 controller...");

  // Inisialisasi pin
  pinMode(RPWM_KIRI, OUTPUT);
  pinMode(LPWM_KIRI, OUTPUT);
  pinMode(RPWM_KANAN, OUTPUT);
  pinMode(LPWM_KANAN, OUTPUT);
}

void loop() {
  if (PS4.isConnected()) {
    int ly = PS4.LStickY();  // -128 (maju) sampai 127 (mundur)
    int lx = PS4.LStickX();  // -128 (kiri) sampai 127 (kanan)
    int ry = PS4.RStickY();  // -128 (maju) sampai 127 (mundur)
    int rx = PS4.RStickX();  // -128 (kiri) sampai 127 (kanan)

    // Tentukan batas max pwm sesuai tombol L1/R1
    int maxPWMCurrent = DEFAULT_MAX_PWM;
    if (PS4.L1()) {
      maxPWMCurrent = MEDIUM_MAX_PWM;
    }
    if (PS4.R1()) {
      maxPWMCurrent = MAX_PWM;
    }

    if (abs(rx) > 30) {
      Serial.println("Belok");
      // Belok kiri / kanan (speed tetap)
      if (rx < -30) belokKiri();
      else if (rx > 30) belokKanan();
    } else if (abs(ly) > 30) {
      // Maju / mundur dengan PWM dari stick dan batas maxPWMCurrent
      gerakPWM(ly, maxPWMCurrent);
    } else {
      berhenti();
      Serial.println("stop");
    }

    delay(50);
  }
}

// === Method kendali motor ===

void gerakPWM(int nilaiY, int maxPWMCurrent) {
  int pwm = map(abs(nilaiY), 0, 128, 0, maxPWMCurrent);

  Serial.print("PWM kiri dan kanan: ");
  Serial.println(pwm);

  if (nilaiY > 0) {
    // Maju
    Serial.println("maju");
    motorKiri(pwm, 1);
    motorKanan(pwm, 1);
  } else {
    Serial.println("mundur");
    // Mundur
    motorKiri(pwm, -1);
    motorKanan(pwm, -1);
  }
}

void belokKiri() {
  motorKiri(TURN_SPEED, -1);
  motorKanan(TURN_SPEED, 1);
}

void belokKanan() {
  motorKiri(TURN_SPEED, 1);
  motorKanan(TURN_SPEED, -1);
}

void berhenti() {
  motorKiri(0, 0);
  motorKanan(0, 0);
}

void motorKiri(int pwm, int arah) {
  if (arah == 1) {  // maju
    analogWrite(RPWM_KIRI, pwm);
    analogWrite(LPWM_KIRI, 0);
  } else if (arah == -1) {  // mundur
    analogWrite(RPWM_KIRI, 0);
    analogWrite(LPWM_KIRI, pwm);
  } else {
    analogWrite(RPWM_KIRI, 0);
    analogWrite(LPWM_KIRI, 0);
  }
}

void motorKanan(int pwm, int arah) {
  if (arah == 1) {
    analogWrite(RPWM_KANAN, pwm);
    analogWrite(LPWM_KANAN, 0);
  } else if (arah == -1) {
    analogWrite(RPWM_KANAN, 0);
    analogWrite(LPWM_KANAN, pwm);
  } else {
    analogWrite(RPWM_KANAN, 0);
    analogWrite(LPWM_KANAN, 0);
  }
}
