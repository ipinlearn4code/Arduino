#include <IRremoteESP8266.h>
#include <IRsend.h>

const uint16_t kIrLedPin = D2; // IR LED dihubungkan ke pin D2
IRsend irsend(kIrLedPin);

void setup() {
  Serial.begin(115200);
  irsend.begin();
  Serial.println("Remote AC Samsung via Serial Siap");
  Serial.println("Ketik perintah: on, off, auto, cool, dry, heat, 17, 20, 22, 25");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim(); // hapus spasi atau newline

    if (cmd == "on") {
      irsend.sendCOOLIX(0xB21FC4);
      Serial.println("Mengirim: ON");
    } else if (cmd == "off") {
      irsend.sendCOOLIX(0xB27BE0);
      Serial.println("Matikan AC");
    } else if (cmd == "auto") {
      irsend.sendCOOLIX(0xB21F48);
      Serial.println("Mengirim: AUTO mode");
    } else if (cmd == "cool") {
      irsend.sendCOOLIX(0xB25FC0);
      Serial.println("Mengirim: COOL mode");
    } else if (cmd == "dry") {
      irsend.sendCOOLIX(0xB21FC4);
      Serial.println("Mengirim: DRY mode");
    } else if (cmd == "heat") {
      irsend.sendCOOLIX(0xB25FCC);
      Serial.println("Mengirim: HEAT mode");
    } else if (cmd == "17") {
      irsend.sendCOOLIX(0xB21F08);
      Serial.println("Mengirim: Suhu 17°C");
    } else if (cmd == "20") {
      irsend.sendCOOLIX(0xB21F28);
      Serial.println("Mengirim: Suhu 20°C");
    } else if (cmd == "22") {
      irsend.sendCOOLIX(0xB21F78);
      Serial.println("Mengirim: Suhu 22°C");
    } else if (cmd == "25") {
      irsend.sendCOOLIX(0xB21FC8);
      Serial.println("Mengirim: Suhu 25°C");
    } else {
      Serial.println("Perintah tidak dikenali");
    }

    delay(1000);
  }
}
