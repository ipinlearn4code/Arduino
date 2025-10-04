#define TRIG_PIN 6
#define ECHO_PIN 5

long duration;
float distance;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  Serial.print("fasdkfjsdkf");
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH, 300); // tambahkan timeout
  if (duration > 0) {
    distance = duration * 0.034 / 2;
    Serial.print("Jarak: ");
    Serial.print(distance);
    Serial.println(" cm");
  } else {
    Serial.println("Tidak ada sinyal pantulan");
  }

  delay(500); // beri jeda agar tidak overload
}
