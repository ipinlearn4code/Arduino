#include <WiFi.h>
#include <WebServer.h>
#include <ESP32PWM.h>

// DRV1 - KIRI
#define IN1_LB 13
#define IN2_LB 12
#define IN3_LF 14
#define IN4_LF 27

// DRV2 - KANAN
#define IN1_RB 32
#define IN2_RB 33
#define IN3_RF 25
#define IN4_RF 26

#define MAX_SPEED 255

// WiFi AP config
const char *ssid = "ESP32_MECANUM";
const char *password = "12345678";
WebServer server(80);

// PWM objek untuk masing-masing pin
ESP32PWM pwmIN1_LB, pwmIN2_LB, pwmIN3_LF, pwmIN4_LF;
ESP32PWM pwmIN1_RB, pwmIN2_RB, pwmIN3_RF, pwmIN4_RF;

void setup() {
  Serial.begin(115200);

  // Inisialisasi PWM timer
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  // Attach pin ke PWM
  pwmIN1_LB.attachPin(IN1_LB, 5000, 8);
  pwmIN2_LB.attachPin(IN2_LB, 5000, 8);
  pwmIN3_LF.attachPin(IN3_LF, 5000, 8);
  pwmIN4_LF.attachPin(IN4_LF, 5000, 8);

  pwmIN1_RB.attachPin(IN1_RB, 5000, 8);
  pwmIN2_RB.attachPin(IN2_RB, 5000, 8);
  pwmIN3_RF.attachPin(IN3_RF, 5000, 8);
  pwmIN4_RF.attachPin(IN4_RF, 5000, 8);

  // WiFi Access Point
  WiFi.softAP(ssid, password);
  Serial.print("[WIFI] IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.onNotFound(handleDynamicCommand);
  server.begin();
}

void loop() {
  server.handleClient();
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    parseSerialCommand(cmd);
  }
}

void parseSerialCommand(String cmd) {
  if (cmd.startsWith("f(")) {
    maju(cmd.substring(2, cmd.length() - 1).toInt());
  } else if (cmd.startsWith("b(")) {
    mundur(cmd.substring(2, cmd.length() - 1).toInt());
  } else if (cmd.startsWith("l(")) {
    kiri(cmd.substring(2, cmd.length() - 1).toInt());
  } else if (cmd.startsWith("r(")) {
    kanan(cmd.substring(2, cmd.length() - 1).toInt());
  } else if (cmd.startsWith("tl(")) {
    putarKiri(cmd.substring(3, cmd.length() - 1).toInt());
  } else if (cmd.startsWith("tr(")) {
    putarKanan(cmd.substring(3, cmd.length() - 1).toInt());
  } else if (cmd == "s") {
    stopAll();
  }
}

void motorControl(ESP32PWM &pin1, ESP32PWM &pin2, int speed, bool forward) {
  pin1.write(forward ? speed : 0);
  pin2.write(forward ? 0 : speed);
}

void maju(int speed) {
  motorControl(pwmIN1_LB, pwmIN2_LB, speed, true);
  motorControl(pwmIN3_LF, pwmIN4_LF, speed, true);
  motorControl(pwmIN1_RB, pwmIN2_RB, speed, true);
  motorControl(pwmIN3_RF, pwmIN4_RF, speed, true);
}

void mundur(int speed) {
  motorControl(pwmIN1_LB, pwmIN2_LB, speed, false);
  motorControl(pwmIN3_LF, pwmIN4_LF, speed, false);
  motorControl(pwmIN1_RB, pwmIN2_RB, speed, false);
  motorControl(pwmIN3_RF, pwmIN4_RF, speed, false);
}

void kiri(int speed) {
  motorControl(pwmIN1_LB, pwmIN2_LB, speed, false);
  motorControl(pwmIN3_LF, pwmIN4_LF, speed, true);
  motorControl(pwmIN1_RB, pwmIN2_RB, speed, true);
  motorControl(pwmIN3_RF, pwmIN4_RF, speed, false);
}

void kanan(int speed) {
  motorControl(pwmIN1_LB, pwmIN2_LB, speed, true);
  motorControl(pwmIN3_LF, pwmIN4_LF, speed, false);
  motorControl(pwmIN1_RB, pwmIN2_RB, speed, false);
  motorControl(pwmIN3_RF, pwmIN4_RF, speed, true);
}

void putarKiri(int speed) {
  motorControl(pwmIN1_LB, pwmIN2_LB, speed, false);
  motorControl(pwmIN3_LF, pwmIN4_LF, speed, false);
  motorControl(pwmIN1_RB, pwmIN2_RB, speed, true);
  motorControl(pwmIN3_RF, pwmIN4_RF, speed, true);
}

void putarKanan(int speed) {
  motorControl(pwmIN1_LB, pwmIN2_LB, speed, true);
  motorControl(pwmIN3_LF, pwmIN4_LF, speed, true);
  motorControl(pwmIN1_RB, pwmIN2_RB, speed, false);
  motorControl(pwmIN3_RF, pwmIN4_RF, speed, false);
}

void stopAll() {
  pwmIN1_LB.write(0); pwmIN2_LB.write(0);
  pwmIN3_LF.write(0); pwmIN4_LF.write(0);
  pwmIN1_RB.write(0); pwmIN2_RB.write(0);
  pwmIN3_RF.write(0); pwmIN4_RF.write(0);
}

void handleDynamicCommand() {
  String uri = server.uri();
  int lastSlash = uri.lastIndexOf("/");
  if (lastSlash < 0 || lastSlash >= uri.length() - 1) {
    server.send(404, "text/plain", "Invalid command");
    return;
  }

  String command = uri.substring(1, lastSlash);
  int speed = uri.substring(lastSlash + 1).toInt();

  if (command == "forward") maju(speed);
  else if (command == "backward") mundur(speed);
  else if (command == "left") kiri(speed);
  else if (command == "right") kanan(speed);
  else if (command == "turnleft") putarKiri(speed);
  else if (command == "turnright") putarKanan(speed);
  else {
    server.send(404, "text/plain", "Unknown command: " + command);
    return;
  }

  server.send(200, "text/plain", "OK speed=" + String(speed));
}
