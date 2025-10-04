#include <PS4Controller.h>

#define DEADZONE 40

#define PWM_KIRI 18
#define PWM_KANAN 19

#define PWM_MIN 0
#define PWM_STOP 127
#define PWM_MAX 255

#define LED_BUILTIN 2

int deadzone = 40;

void analogWriteESP32(uint8_t pin, uint8_t value) {
  int channel = pin % 16;
  ledcSetup(channel, 5000, 8);
  ledcAttachPin(pin, channel);
  ledcWrite(channel, value);
}

void setup() {
  Serial.begin(115200);
  PS4.begin();
  pinMode(LED_BUILTIN, OUTPUT);

  analogWriteESP32(PWM_KIRI, PWM_STOP);
  analogWriteESP32(PWM_KANAN, PWM_STOP);

  Serial.println("Menunggu PS4 controller...");
}

void loop() {
  if (PS4.isConnected()) {
    digitalWrite(LED_BUILTIN, HIGH);

    int throttle = -PS4.LStickY();
    int steering = PS4.RStickX();

    if (abs(throttle) < deadzone) throttle = 0;
    if (abs(steering) < deadzone) steering = 0;

    int maxPWM = 100;
    if (PS4.L1()) maxPWM = 180;
    if (PS4.R1()) maxPWM = 255;

    throttle = map(throttle, 128, -127, -maxPWM, maxPWM);
    steering = map(steering, -128, 127, -maxPWM, maxPWM);

    int pwmKiri = constrain(throttle + steering, -maxPWM, maxPWM);
    int pwmKanan = constrain(throttle - steering, -maxPWM, maxPWM);

    int outKiri = map(pwmKiri, -255, 255, PWM_MIN, PWM_MAX);
    int outKanan = map(pwmKanan, -255, 255, PWM_MIN, PWM_MAX);

    if (throttle == 0 && steering == 0) {
      outKiri = PWM_STOP;
      outKanan = PWM_STOP;
    }

    analogWriteESP32(PWM_KIRI, outKiri);
    analogWriteESP32(PWM_KANAN, outKanan);

    Serial.print("Throttle: "); Serial
