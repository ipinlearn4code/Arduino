// Pin motor
int IN1 = 5;   // Motor Kiri - maju
int IN2 = 6;   // Motor Kiri - mundur
int IN3 = 9;   // Motor Kanan - maju
int IN4 = 10;  // Motor Kanan - mundur

void setup() {
  // Atur pin sebagai output
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {
  // Gerak maju
  digitalWrite(IN1, HIGH); // Motor kiri maju
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); // Motor kanan maju
  digitalWrite(IN4, LOW);

  // Jalan selama 2 detik
  delay(2000);

  // Berhenti
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  delay(2000);
}
