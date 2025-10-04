// Konfigurasi pin
const int ENA = 5;
const int IN1 = 6;
const int IN2 = 7;
const int IN3 = 8;
const int IN4 = 9;
const int ENB = 10;

// Nilai PWM statis (0 - 255)
int pwmSpeed = 255;

void setup() {
  // Inisialisasi pin
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Inisialisasi Serial
  Serial.begin(9600);
  Serial.println("Siap menerima perintah (maju/mundur/kiri/kanan/stop)");
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim(); // hapus spasi atau newline

    if (command == "maju") {
      maju();
    } else if (command == "mundur") {
      mundur();
    } else if (command == "kiri") {
      belokKiri();
    } else if (command == "kanan") {
      belokKanan();
    } else if (command == "stop") {
      berhenti();
    }
  }
}

void maju() {
  analogWrite(ENA, pwmSpeed);
  analogWrite(ENB, pwmSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void mundur() {
  analogWrite(ENA, pwmSpeed);
  analogWrite(ENB, pwmSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void belokKiri() {
  analogWrite(ENA, pwmSpeed);
  analogWrite(ENB, pwmSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH); // motor kiri mundur
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);  // motor kanan maju
}

void belokKanan() {
  analogWrite(ENA, pwmSpeed);
  analogWrite(ENB, pwmSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);  // motor kiri maju
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH); // motor kanan mundur
}

void berhenti() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
