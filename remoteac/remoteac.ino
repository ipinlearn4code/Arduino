#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>


// Ganti dengan SSID dan Password WiFi kamu
const char* ssid = "Tselhome-7A14";
const char* password = "81190678";

const String baseURL = "http://192.168.8.106:8080/api/payloads/DEV017";

const unsigned long HTTP_TIMEOUT = 5000;
const unsigned long UPDATE_INTERVAL = 2000;

// Gunakan pin D2 (GPIO4) di ESP8266
const uint16_t kIrLedPin = 4;  // GPIO4 = D2 di NodeMCU
IRsend irsend(kIrLedPin);

struct IRCommand {
  const char* nama;
  uint32_t kode;
  const char* deskripsi;
};

IRCommand perintahPower[] = {
  { "on", 0xB21FC4, "Power ON" },
  { "off", 0xB27BE0, "Power OFF" },
};

IRCommand perintahMode[] = {
  { "auto", 0xB21F48, "Mode AUTO" },
  { "cool", 0xB25FC0, "Mode COOL" },
  { "dry", 0xB21FC4, "Mode DRY" },
  { "heat", 0xB25FCC, "Mode HEAT" },
};

IRCommand perintahSuhu[] = {
  { "17", 0xB21F08, "Suhu 17°C" },
  { "20", 0xB21F28, "Suhu 20°C" },
  { "22", 0xB21F78, "Suhu 22°C" },
  { "25", 0xB21FC8, "Suhu 25°C" },
};

// State terakhir
String powerState = "";
String modeState = "";
String tempState = "";

void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

String getPayload() {
  HTTPClient http;
  WiFiClient client;
  String payload = "";

  http.begin(client, baseURL);
  int httpCode = http.GET();

  // Handle HTTP response
  if (httpCode == HTTP_CODE_OK) {
    payload = http.getString();
    Serial.println("Received payload:");
    Serial.println(payload);
  } else {
    Serial.printf("HTTP request failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  return payload;
}

void processPayload(const String& payload) {
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.print("JSON parsing failed: ");
    Serial.println(error.c_str());
    Serial.println("Raw payload:");
    Serial.println(payload);
    return;
  }

  JsonArray devicesArray = doc.as<JsonArray>();

  for (JsonObject device : devicesArray) {
    String deviceId = device["deviceId"].as<String>();
    JsonObject data = device["data"];
    int value = data["value"];
    String status = data["status"].as<String>();

    Serial.println("=== Device Info ===");
    Serial.println("Device ID: " + deviceId);
    Serial.println("Value: " + String(value));
    Serial.println("Status: " + status);


    if (status == "off") {

      prosesPerintah("off");
    } else {
      prosesPerintah("on");
      delay(2000);
      if (value > 0) {
        prosesPerintah(String(value));
      }
    }
  }
}


bool cariDanKirim(const String& cmd, IRCommand* daftar, size_t jumlah, String& stateRef) {
  for (size_t i = 0; i < jumlah; i++) {
    if (cmd == daftar[i].nama) {
      if (cmd == stateRef) {
        Serial.print("Perintah ");
        Serial.print(cmd);
        Serial.println(" diabaikan (state sama)");
        return true;
      }
      irsend.sendCOOLIX(daftar[i].kode);
      Serial.print("Mengirim: ");
      Serial.println(daftar[i].deskripsi);
      stateRef = cmd;
      return true;
    }
  }
  return false;
}

void prosesPerintah(const String& cmd) {
  if (cariDanKirim(cmd, perintahPower, sizeof(perintahPower) / sizeof(perintahPower[0]), powerState)) return;
  if (cariDanKirim(cmd, perintahMode, sizeof(perintahMode) / sizeof(perintahMode[0]), modeState)) return;
  if (cariDanKirim(cmd, perintahSuhu, sizeof(perintahSuhu) / sizeof(perintahSuhu[0]), tempState)) return;

  Serial.println("Perintah tidak dikenali");
}

void setup() {
  Serial.begin(115200);
  irsend.begin();
  Serial.println("Remote AC Samsung via Serial Siap");
  Serial.println("Kategori:");
  Serial.println("- Power: on, off");
  Serial.println("- Mode: auto, cool, dry, heat");
  Serial.println("- Suhu: 17, 20, 22, 25");
  connectToWiFi();
}

void loop() {
  // if (Serial.available()) {
  //   String cmd = Serial.readStringUntil('\n');
  //   cmd.trim();
  //   cmd.toLowerCase();
  //   prosesPerintah(cmd);
  //   delay(1000);
  // }

  String payload = getPayload();

  processPayload(payload);

  delay(2000);
}
