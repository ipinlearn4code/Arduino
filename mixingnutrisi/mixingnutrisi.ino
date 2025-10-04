#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Pin relay 4 channel (pastikan sesuai dengan wiring fisik)
const String devices[] = { "DEV013", "DEV018" };

const int nutrientPump = 22;
const int waterPump = 21;
const int irigationPump = 19;

const char* ssid = "Tselhome-7A14";
const char* password = "81190678";
const String baseURL = "http://192.168.8.106:8080/api/actuators/payloads/";

// Timeout untuk HTTP request (ms)
const unsigned long HTTP_TIMEOUT = 10000;
const unsigned long UPDATE_INTERVAL = 2000;

unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 10000;

void blinkLED(int intervalCount = 1) {
  if (intervalCount <= 0) {
    intervalCount = 1;
  }

  for (int i = 0; i < intervalCount; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(nutrientPump, OUTPUT);
  digitalWrite(nutrientPump, HIGH);

  pinMode(waterPump, OUTPUT);
  digitalWrite(waterPump, HIGH);

  pinMode(irigationPump, OUTPUT);
  digitalWrite(irigationPump, HIGH);

  connectToWiFi();
  blinkLED(2);
  delay(500);
}

void loop() {
  static unsigned long lastUpdate = 0;

  // if (WiFi.status() != WL_CONNECTED) {
  //   unsigned long now = millis();
  //   if (now - lastReconnectAttempt >= reconnectInterval) {
  //     Serial.println("Attempting to reconnect to WiFi...");
  //     WiFi.disconnect();  // Penting agar WiFi.begin() bisa sukses
  //     WiFi.begin(ssid, password);
  //     lastReconnectAttempt = now;
  //   }
  //   blinkLED(5);
  //   return;  // jangan lanjut ambil payload kalau WiFi belum nyambung
  // }
  if (millis() - lastUpdate >= UPDATE_INTERVAL) {
    String payload = getPayload();  //blink 1
    if (!payload.isEmpty()) {
      processPayload(payload);
      // printRelayStates();
    }
    lastUpdate = millis();
  }
}

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
  String payload = "";

  http.begin(baseURL);
  int httpCode = http.GET();

  // Handle HTTP response
  if (httpCode == HTTP_CODE_OK) {
    payload = http.getString();
    Serial.println("Received payload:");
    // Serial.println(payload);
    blinkLED(1);
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

  Serial.println("Processing devices:");
  for (JsonObject device : devicesArray) {
    String deviceId = device["deviceId"].as<String>();
    String status = device["status"].as<String>();  // Diubah ke struktur JSON baru

    Serial.print("Found device: ");
    Serial.print(deviceId);
    Serial.print(" with status: ");
    Serial.println(status);

    // Cari index device yang sesuai
    for (int i = 0; i < 4; i++) {
      if (deviceId == devices[i]) {
        Serial.print("Matched device index: ");
        Serial.println(i);
        if (status.equalsIgnoreCase("on")) {
          if (deviceId == "DEV018") {
            Serial.println("Calling mixNutrient()");
            mixNutrient();
          } else if (deviceId == "DEV013") {
            Serial.println("Calling anotherAction()");
            siram();
          }
        } else {
          allOff();
        }
      }
    }
  }
}

void siram() {
  Serial.print("Siram");
  digitalWrite(irigationPump, LOW);
  delay(1500);
  
}

void allOff() {
  Serial.print("Standby");
  digitalWrite(nutrientPump, HIGH);
  digitalWrite(waterPump, HIGH);
  digitalWrite(irigationPump, HIGH);
}

void mixNutrient() {
  Serial.println("Mixing Nutrisi Dijalankan");
  digitalWrite(waterPump, LOW);  //air masuk reservoir
  delay(3000);
  digitalWrite(nutrientPump, LOW);  //nutrisi masuk reservoir
  delay(800);
  digitalWrite(nutrientPump, HIGH);  //nutrisi berhenti
  delay(4000);
  digitalWrite(waterPump, HIGH);  //air masuk berhenti
  delay(10000);
  digitalWrite(irigationPump, LOW);  //air masuk berhenti
  delay(5000);
  digitalWrite(irigationPump, HIGH);  //air masuk irigasi
  blinkLED(10);
  delay(36000);
  digitalWrite(irigationPump, LOW);  //air masuk berhenti
  delay(5000);
  digitalWrite(irigationPump, HIGH);  //air masuk irigasi
}