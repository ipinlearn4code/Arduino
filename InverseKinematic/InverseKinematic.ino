#include <AccelStepper.h>
#include <math.h>

// === Konfigurasi Parameter untuk Tuning ===
#define FRAME_LENGTH 715.0    // Panjang frame (X) dalam mm
#define FRAME_WIDTH 360.0     // Lebar frame (Y) dalam mm
#define FRAME_HEIGHT 370.0    // Tinggi frame (Z) dalam mm
// #define PLATE_SIZE 100.0      // Ukuran plat 10x10 cm dalam mm
#define PLATE_OFFSET 50.0     // Offset dari tengah plat ke sudut (phytagoras 5*5*x mm)

#define PULLEY_DIAMETER 10.0  // Diameter pulley dalam mm
#define STEPS_PER_REV 1600  // Steps per revolusi (200 steps *  microsteps)
#define MAX_STEPS_PER_MOVE 100000  // Batas maksimum langkah per gerakan
#define MOTOR_MAX_SPEED 1000      // Kecepatan maksimum motor (steps/s)
#define MOTOR_ACCELERATION 500    // Akselerasi motor (steps/s^2)
#define DEBUG_INTERVAL 500        // Interval debugging posisi (ms)
#define MAX_COMMAND_LENGTH 20     // Batas panjang perintah

// Pin motor stepper
#define M1_STEP_PIN 54  // Kiri Depan
#define M1_DIR_PIN 55
#define M2_STEP_PIN 60  // Kiri Belakang
#define M2_DIR_PIN 61
#define M3_STEP_PIN 36  // Kanan Depan
#define M3_DIR_PIN 34
#define M4_STEP_PIN 26  // Kanan Belakang
#define M4_DIR_PIN 28
#define X_ENABLE_PIN 38
#define Y_ENABLE_PIN 56
#define E0_ENABLE_PIN 24
#define E1_ENABLE_PIN 30

// === Konstanta Turunan ===
const float MM_PER_REV = PULLEY_DIAMETER * PI;  // Keliling pulley
const float STEPS_PER_MM = STEPS_PER_REV / MM_PER_REV;  // Langkah per mm

// Posisi anchor (motor) di keempat sudut atas
const float anchors[4][3] = {
  {0, 0, FRAME_HEIGHT},           // M1: Kiri Depan
  {0, FRAME_WIDTH, FRAME_HEIGHT}, // M2: Kiri Belakang
  {FRAME_LENGTH, 0, FRAME_HEIGHT},// M3: Kanan Depan
  {FRAME_LENGTH, FRAME_WIDTH, FRAME_HEIGHT} // M4: Kanan Belakang
};

// Struktur untuk menyimpan posisi bernama
struct NamedPosition {
  String name;
  float x, y, z;
};
NamedPosition savedPositions[10];  // Maksimum 10 posisi tersimpan
int posCount = 0;

// Posisi saat ini (tengah plat)
float currentPos[3] = {0, 0, 0};
float currentCableLengths[4] = {0, 0, 0, 0};

// Inisialisasi motor
AccelStepper motor1(AccelStepper::DRIVER, M1_STEP_PIN, M1_DIR_PIN);
AccelStepper motor2(AccelStepper::DRIVER, M2_STEP_PIN, M2_DIR_PIN);
AccelStepper motor3(AccelStepper::DRIVER, M3_STEP_PIN, M3_DIR_PIN);
AccelStepper motor4(AccelStepper::DRIVER, M4_STEP_PIN, M4_DIR_PIN);

// Buffer untuk perintah serial
String serialBuffer = "";
unsigned long lastCharTime = 0;
unsigned long lastDebugTime = 0;
const unsigned long COMMAND_TIMEOUT = 2000; // Timeout 2 detik jika tidak ada input baru

void setup() {
  // Inisialisasi Serial
  Serial.begin(115200);
  
  // Konfigurasi pin enable
  pinMode(X_ENABLE_PIN, OUTPUT);
  pinMode(Y_ENABLE_PIN, OUTPUT);
  pinMode(E0_ENABLE_PIN, OUTPUT);
  pinMode(E1_ENABLE_PIN, OUTPUT);
  digitalWrite(X_ENABLE_PIN, LOW);
  digitalWrite(Y_ENABLE_PIN, LOW);
  digitalWrite(E0_ENABLE_PIN, LOW);
  digitalWrite(E1_ENABLE_PIN, LOW);

  // Konfigurasi motor
  motor1.setMaxSpeed(MOTOR_MAX_SPEED);
  motor1.setAcceleration(MOTOR_ACCELERATION);
  motor2.setMaxSpeed(MOTOR_MAX_SPEED);
  motor2.setAcceleration(MOTOR_ACCELERATION);
  motor3.setMaxSpeed(MOTOR_MAX_SPEED);
  motor3.setAcceleration(MOTOR_ACCELERATION);
  motor4.setMaxSpeed(MOTOR_MAX_SPEED);
  motor4.setAcceleration(MOTOR_ACCELERATION);

  // Inisialisasi posisi awal di pojok kiri tengah atas (0, FRAME_WIDTH / 2, FRAME_HEIGHT)
  currentPos[0] = PLATE_OFFSET + 71;                 // x = 0 mm
  currentPos[1] = FRAME_WIDTH / 2;   // y = 150 mm
  currentPos[2] = FRAME_HEIGHT;      // z = 250 mm
  updateCableLengths();

  // Cetak panjang kabel awal untuk debugging
  Serial.println("Initial cable lengths (mm):");
  for (int i = 0; i < 4; i++) {
    Serial.print("Motor "); Serial.print(i + 1); Serial.print(": ");
    Serial.println(currentCableLengths[i]);
  }

  // Pindah ke center point atas (350, 150, 250)
  moveToPosition(FRAME_LENGTH / 2, FRAME_WIDTH / 2, FRAME_HEIGHT / 2);
  Serial.println("System ready at center position (top)");
}

void loop() {
  // Proses perintah serial
  while (Serial.available() > 0) {
    char c = Serial.read();
    Serial.print("Received: "); Serial.println(c); // Debug input
    serialBuffer += c;
    lastCharTime = millis();
    if (serialBuffer.length() >= MAX_COMMAND_LENGTH) {
      processCommand(serialBuffer);
      serialBuffer = "";
    }
  }

  // Proses perintah jika timeout tercapai (2 detik tanpa input baru)
  if (serialBuffer.length() > 0 && (millis() - lastCharTime >= COMMAND_TIMEOUT)) {
    processCommand(serialBuffer);
    serialBuffer = "";
  }

  // Jalankan motor
  motor1.run();
  motor2.run();
  motor3.run();
  motor4.run();

  // Cetak posisi saat ini dan panjang kabel setiap DEBUG_INTERVAL
  unsigned long currentTime = millis();
  if (currentTime - lastDebugTime >= DEBUG_INTERVAL) {
    Serial.println("Current position (mm): (" + String(currentPos[0]) + "," + 
                   String(currentPos[1]) + "," + String(currentPos[2]) + ")");
    Serial.println("Current cable lengths (mm):");
    for (int i = 0; i < 4; i++) {
      Serial.print("Motor "); Serial.print(i + 1); Serial.print(": ");
      Serial.println(currentCableLengths[i]);
    }
    Serial.println("Motor distances to go (steps):");
    Serial.print("Motor 1: "); Serial.println(motor1.distanceToGo());
    Serial.print("Motor 2: "); Serial.println(motor2.distanceToGo());
    Serial.print("Motor 3: "); Serial.println(motor3.distanceToGo());
    Serial.print("Motor 4: "); Serial.println(motor4.distanceToGo());
    lastDebugTime = currentTime;
  }
}

// Fungsi untuk menghitung panjang kabel dengan mempertimbangkan offset plat
void updateCableLengths() {
  // Offset sudut plat relatif terhadap posisi tengah (currentPos)
  float cornerOffsets[4][2] = {
    {-PLATE_OFFSET, -PLATE_OFFSET}, // Sudut kiri depan (M1)
    {-PLATE_OFFSET, PLATE_OFFSET},  // Sudut kiri belakang (M2)
    {PLATE_OFFSET, -PLATE_OFFSET},  // Sudut kanan depan (M3)
    {PLATE_OFFSET, PLATE_OFFSET}    // Sudut kanan belakang (M4)
  };

  for (int i = 0; i < 4; i++) {
    float x = currentPos[0] + cornerOffsets[i][0];
    float y = currentPos[1] + cornerOffsets[i][1];
    float z = currentPos[2];
    float dx = x - anchors[i][0];
    float dy = y - anchors[i][1];
    float dz = z - anchors[i][2];
    currentCableLengths[i] = sqrt(dx*dx + dy*dy + dz*dz);
  }
}

// Fungsi inverse kinematics dan gerakan motor
void moveToPosition(float x, float y, float z) {
  if (x < 0 || x > FRAME_LENGTH || y < 0 || y > FRAME_WIDTH || z < 0 || z > FRAME_HEIGHT) {
    Serial.println("Error: Position out of bounds");
    return;
  }

  float newCableLengths[4];
  float cornerOffsets[4][2] = {
    {-PLATE_OFFSET, -PLATE_OFFSET},
    {-PLATE_OFFSET, PLATE_OFFSET},
    { PLATE_OFFSET, -PLATE_OFFSET},
    { PLATE_OFFSET, PLATE_OFFSET}
  };

  // Hitung panjang kabel baru untuk tiap sudut
  for (int i = 0; i < 4; i++) {
    float xCorner = x + cornerOffsets[i][0];
    float yCorner = y + cornerOffsets[i][1];
    float zCorner = z;

    float dx = xCorner - anchors[i][0];
    float dy = yCorner - anchors[i][1];
    float dz = zCorner - anchors[i][2];

    newCableLengths[i] = sqrt(dx * dx + dy * dy + dz * dz);
  }

  // Hitung delta langkah
  long deltaSteps[4];
  long maxSteps = 0;
  for (int i = 0; i < 4; i++) {
    float deltaLength = newCableLengths[i] - currentCableLengths[i];
    deltaSteps[i] = (long)(deltaLength * STEPS_PER_MM);
    if (abs(deltaSteps[i]) > MAX_STEPS_PER_MOVE) {
      Serial.print("Error: Motor "); Serial.print(i + 1);
      Serial.println(" exceeds maximum steps per move");
      return;
    }
    if (abs(deltaSteps[i]) > maxSteps) {
      maxSteps = abs(deltaSteps[i]);
    }
  }

  // Set kecepatan motor secara proporsional
  for (int i = 0; i < 4; i++) {
    float speedFactor = (maxSteps == 0) ? 0 : (float)abs(deltaSteps[i]) / maxSteps;
    float speed = speedFactor * MOTOR_MAX_SPEED;

    // Tetapkan kecepatan minimum agar tetap jalan meski langkah kecil
    if (speed < 100 && abs(deltaSteps[i]) > 0) speed = 100;

    switch (i) {
      case 0: motor1.setMaxSpeed(speed); break;
      case 1: motor2.setMaxSpeed(speed); break;
      case 2: motor3.setMaxSpeed(speed); break;
      case 3: motor4.setMaxSpeed(speed); break;
    }
  }

  // Perintahkan langkah target
  motor1.moveTo(motor1.currentPosition() + deltaSteps[0]);
  motor2.moveTo(motor2.currentPosition() + deltaSteps[1]);
  motor3.moveTo(motor3.currentPosition() + deltaSteps[2]);
  motor4.moveTo(motor4.currentPosition() + deltaSteps[3]);

  // Jalankan hingga semua selesai
  while (motor1.distanceToGo() != 0 || motor2.distanceToGo() != 0 ||
         motor3.distanceToGo() != 0 || motor4.distanceToGo() != 0) {
    motor1.run();
    motor2.run();
    motor3.run();
    motor4.run();
  }

  // Simpan posisi baru
  currentPos[0] = x;
  currentPos[1] = y;
  currentPos[2] = z;
  updateCableLengths();

  Serial.println("Moved to (" + String(x) + "," + String(y) + "," + String(z) + ")");
}




// Fungsi untuk menghentikan semua motor
void stopMotors() {
  motor1.stop();
  motor2.stop();
  motor3.stop();
  motor4.stop();
  digitalWrite(X_ENABLE_PIN, HIGH);
  digitalWrite(Y_ENABLE_PIN, HIGH);
  digitalWrite(E0_ENABLE_PIN, HIGH);
  digitalWrite(E1_ENABLE_PIN, HIGH);
  Serial.println("All motors stopped");
}

// Fungsi untuk shutdown
void shutdown() {
  // Kembali ke (0,150,250)
  moveToPosition(PLATE_OFFSET + 71, FRAME_WIDTH / 2, FRAME_HEIGHT);
  Serial.println("Returning to (0,150,250) and shutting down");
  
  // Matikan semua motor
  digitalWrite(X_ENABLE_PIN, HIGH);
  digitalWrite(Y_ENABLE_PIN, HIGH);
  digitalWrite(E0_ENABLE_PIN, HIGH);
  digitalWrite(E1_ENABLE_PIN, HIGH);
  
  // Delay untuk memastikan pesan terkirim
  delay(100);
  
  // Reset Arduino (khusus untuk Arduino Mega)
  asm volatile ("jmp 0");
}

// Fungsi untuk memproses perintah serial
void processCommand(String cmd) {
  cmd.trim();
  Serial.print("Processing command: "); Serial.println(cmd); // Debug perintah
  if (cmd.startsWith("move(")) {
    cmd.replace("move(", "");
    cmd.replace(")", "");
    int comma1 = cmd.indexOf(",");
    int comma2 = cmd.indexOf(",", comma1 + 1);
    if (comma1 != -1 && comma2 != -1) {
      float x = cmd.substring(0, comma1).toFloat();
      float y = cmd.substring(comma1 + 1, comma2).toFloat();
      float z = cmd.substring(comma2 + 1).toFloat();
      moveToPosition(x, y, z);
    } else {
      Serial.println("Error: Invalid move command format. Use move(x,y,z)");
    }
  } else if (cmd.startsWith("curPos(")) {
    cmd.replace("curPos(", "");
    cmd.replace(")", "");
    if (posCount < 10) {
      savedPositions[posCount].name = cmd;
      savedPositions[posCount].x = currentPos[0];
      savedPositions[posCount].y = currentPos[1];
      savedPositions[posCount].z = currentPos[2];
      posCount++;
      Serial.println("Position saved as " + cmd);
    } else {
      Serial.println("Error: Maximum position storage reached");
    }
  } else if (cmd.startsWith("moveTo(")) {
    cmd.replace("moveTo(", "");
    cmd.replace(")", "");
    for (int i = 0; i < posCount; i++) {
      if (savedPositions[i].name == cmd) {
        moveToPosition(savedPositions[i].x, savedPositions[i].y, savedPositions[i].z);
        return;
      }
    }
    Serial.println("Error: Position " + cmd + " not found");
  } else if (cmd == "shutdown") {
    shutdown();
  } else if (cmd == "stop") {
    stopMotors();
  } else {
    Serial.println("Error: Unknown command");
  }
}