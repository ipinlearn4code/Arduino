#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN     150
#define SERVOMAX     600
#define SERVO_DELAY   5    // ms antar langkah
#define DEBOUNCE_MS  50    // ms untuk debouncing
#define PRINT_INTERVAL 100 // ms interval cetak sudut

const int buttonUpPin   = 4;
const int buttonDownPin = 3;

struct ServoState {
  int channel;
  int currentAngle;
  unsigned long lastMoveTime;
  bool moving;
  int direction;
  unsigned long lastPrintTime;
};

ServoState servo;

// State tombol dengan debounce
bool lastUpRaw = LOW, lastDownRaw = LOW;
bool upPressed = false, downPressed = false;
unsigned long upLastChange = 0, downLastChange = 0;

void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(50);
  
  // Inisialisasi servo
  servo = { 
    .channel = 0, 
    .currentAngle = 90, 
    .lastMoveTime = 0,
    .moving = false,
    .direction = 0,
    .lastPrintTime = 0
  };

  pinMode(buttonUpPin,   INPUT);
  pinMode(buttonDownPin, INPUT);

  Serial.println("🚀 Ready!");
}

// Debounce tombol: bacaan stabil selama DEBOUNCE_MS
bool readDebounced(int pin, bool &lastRaw, unsigned long &lastChange) {
  bool raw = digitalRead(pin);
  if (raw != lastRaw) {
    lastChange = millis();
    lastRaw = raw;
  }
  if (millis() - lastChange > DEBOUNCE_MS) {
    return raw; // Stabil selama periode debounce
  }
  return false; // Belum stabil
}

void checkButtons() {
  bool upStable   = readDebounced(buttonUpPin,   lastUpRaw,   upLastChange);
  bool downStable = readDebounced(buttonDownPin, lastDownRaw, downLastChange);

  // Tentukan gerakan
  if (upStable && !downStable) {
    servo.moving   = true;
    servo.direction = 1;
  }
  else if (downStable && !upStable) {
    servo.moving   = true;
    servo.direction = -1;
  }
  else {
    servo.moving = false;
  }
}

void moveServoSmoothly() {
  if (!servo.moving) return;

  unsigned long now = millis();
  if (now - servo.lastMoveTime >= SERVO_DELAY) {
    servo.lastMoveTime = now;
    servo.currentAngle += servo.direction;
    servo.currentAngle = constrain(servo.currentAngle, 0, 180);

    int pulselen = map(servo.currentAngle, 0, 180, SERVOMIN, SERVOMAX);
    pwm.setPWM(servo.channel, 0, pulselen);
  }
}

void printStatus() {
  unsigned long now = millis();
  if (now - servo.lastPrintTime < PRINT_INTERVAL) return;
  servo.lastPrintTime = now;

  // Cetak sekali per PRINT_INTERVAL ms
  Serial.print("Btn+: ");
  Serial.print(servo.direction == 1 ? "ON " : "OFF");
  Serial.print("  Btn-: ");
  Serial.print(servo.direction == -1 ? "ON " : "OFF");
  Serial.print("  Angle: ");
  Serial.println(servo.currentAngle);
}

void loop() {
  checkButtons();
  moveServoSmoothly();
  printStatus();

  // Di sini bisa letakkan proses non-blocking lain
}
