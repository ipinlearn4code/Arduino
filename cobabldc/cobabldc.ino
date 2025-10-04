#include <Servo.h>

Servo esc;

const int reverseFull = 1000;
const int stopNeutral = 1500;
const int forwardFull = 2000;
const int step = 100;

void setup() {
  esc.attach(9); // Sinyal ESC di pin 9
  Serial.begin(9600);

  // Mulai dari netral
  esc.writeMicroseconds(stopNeutral);
  Serial.println("Motor di netral...");
  delay(3000); // Tunggu ESC siap
}

void loop() {
  // ====== STEP FORWARD ======
  for (int speed = stopNeutral; speed <= forwardFull; speed += step) {
    esc.writeMicroseconds(speed);
    Serial.print("Maju speed: ");
    Serial.println(speed);
    delay(1000);
  }

  // Tahan di full speed
  Serial.println("Maju full speed...");
  delay(2000);

  // ====== TURUN KE NETRAL ======
  for (int speed = forwardFull; speed >= stopNeutral; speed -= step) {
    esc.writeMicroseconds(speed);
    Serial.print("Turun ke netral: ");
    Serial.println(speed);
    delay(1000);
  }

  // Tahan netral
  esc.writeMicroseconds(stopNeutral);
  Serial.println("Netral sebelum mundur...");
  delay(2000);

  // ====== STEP REVERSE ======
  for (int speed = stopNeutral; speed >= reverseFull; speed -= step) {
    esc.writeMicroseconds(speed);
    Serial.print("Mundur speed: ");
    Serial.println(speed);
    delay(1000);
  }

  // Tahan mundur full speed
  Serial.println("Mundur full speed...");
  delay(2000);

  // ====== NAIK KE NETRAL DARI MUNDUR ======
  for (int speed = reverseFull; speed <= stopNeutral; speed += step) {
    esc.writeMicroseconds(speed);
    Serial.print("Kembali ke netral: ");
    Serial.println(speed);
    delay(1000);
  }

  // Diam di netral
  Serial.println("Siklus selesai, diam di netral...");
  delay(3000);
}
