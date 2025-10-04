#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <dht.h>

const char* ssid = "TECNO POVA 5";      // Ganti dengan SSID Wi-Fi kamu
const char* password = "pakaiiniaja";  // Ganti dengan password Wi-Fi kamu

WiFiClient client;
const int ledPin = LED_BUILTIN;

struct Feature {
  int id;
  String service;
  String tipe;
  String value;
  String command;
};


Feature features[100];
int numFeatures = 0;

unsigned long previousSyncMillis = 0;
unsigned long syncInterval = 500;  // Sync every 500ms

unsigned long previousValueUpdateMillis = 0;
unsigned long valueUpdateInterval = 500;  // Value type updates every 1000ms

unsigned long triggerStartMillis = 0;
bool triggerActive = false;
unsigned long triggerDuration = 1000;  // 5 seconds for trigger to turn off


const int led = 5;
const int led2 = 13;
const int led3 = 16;
const int sensor = 14;
const int buzzer = 15;


Feature temp;
dht DHT;
double humidity, suhu;


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  pinMode(ledPin, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(buzzer,OUTPUT);
  digitalWrite(ledPin, HIGH);

  // Tunggu hingga koneksi berhasil
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Menghubungkan ke Wi-Fi...");
    digitalWrite(ledPin, LOW);
  }
  digitalWrite(ledPin, HIGH);
  Serial.println("Terhubung ke Wi-Fi!");
  syncData();
}

void loop() {


  // // Handle trigger input and reset after 5 seconds
  // handleTrigger();
  syncData();
  delay(250);

  DHT.read11(sensor);
  humidity = DHT.humidity;
  suhu = DHT.temperature;
  if(suhu>35){
    digitalWrite(buzzer,HIGH);
  }else{
    digitalWrite(buzzer,LOW);
  }

  handgleValue();
  delay(250);

 
  
}

void handgleValue() {
  for (int j = 0; j < numFeatures; j++) {
    temp = features[j];
    if (temp.service.equals("partyLamp")) {
      switcher(led, temp.command);
    } else if (temp.service.equals("bedLamp")) {
      switcher(led2, temp.command);
    } else if (temp.service.equals("frontLamp")) {
      switcher(led3, temp.command);
    } else if(temp.service.equals("suhu")){
      String val = "Suhu "+String(suhu)+" Hum "+String(humidity);
      updateValue(temp.id,val);
    } else {
      Serial.print("bukan");
    }

    
  }
}

void switcher(int pin, String command) {
  if (command.equals("1")) {
    digitalWrite(pin, HIGH);
  } else {
    digitalWrite(pin, LOW);
  }
}



void updateValue(int id, String newValue) {
  if (WiFi.status() == WL_CONNECTED) {
    // Menggunakan struktur dengan file apiv2.php
    // String serverPath = "http://api.efrosine.my.id/www/apiv2.php/features/" + String(id) + "/value";

    String serverPath = "http://api.efrosine.my.id/www/apiv2.php/api/features/" + String(id) + "/value";
    Serial.println(serverPath);


    HTTPClient http;
    http.begin(client, serverPath);
    http.addHeader("Content-Type", "application/json");

    // Data JSON yang akan dikirim
    String jsonPayload = "{\"value\":\"" + newValue + "\"}";
    Serial.println(jsonPayload);

    int httpResponseCode = http.PUT(jsonPayload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Response: ");
      Serial.println(response);
      digitalWrite(ledPin, HIGH);
    } else {
      Serial.print("Error updating value: ");
      Serial.println(httpResponseCode);
      http.end();
      digitalWrite(ledPin, LOW);
    }
    http.end();
  } else {
    Serial.println("Koneksi Wi-Fi terputus.");
    // WiFi.begin(ssid, password);
  }
}

void syncData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    // String serverPath = "http://api.efrosine.my.id/www/apiv2.php/api/sync/";

    String serverPath = "http://api.efrosine.my.id/www/apiv2.php/api/sync/";


    http.begin(client, serverPath);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String payload = http.getString();
      Serial.println(payload);

      // Parsing string manually (simplified JSON parsing)
      parsePayload(payload);

    } else {
      Serial.print("Error pada GET request: ");
      Serial.println(httpResponseCode);
      http.end();
    }

    http.end();
  } else {
    Serial.println("Koneksi Wi-Fi terputus.");
    // WiFi.begin(ssid, password);
  }
}

void parsePayload(String payload) {
  int index = 0;
  numFeatures = 0;

  // Assume payload contains multiple JSON-like objects for simplicity (basic parsing)
  while (payload.indexOf("{", index) > 0) {
    int start = payload.indexOf("{", index);
    int end = payload.indexOf("}", start);

    // Extract JSON substring
    String featureData = payload.substring(start, end + 1);

    // Parse individual fields (id, service, tipe, command)
    features[numFeatures].id = extractIntValue(featureData, "\"id\":");
    features[numFeatures].service = extractStringValue(featureData, "\"service\":\"");
    features[numFeatures].tipe = extractStringValue(featureData, "\"tipe\":\"");
    features[numFeatures].value = extractStringValue(featureData, "\"value\":\"");
    features[numFeatures].command = extractStringValue(featureData, "\"command\":\"");

    // Print the parsed values
    Serial.print("ID: ");
    Serial.println(features[numFeatures].id);
    Serial.print("Service: ");
    Serial.println(features[numFeatures].service);
    Serial.print("Command: ");
    Serial.println(features[numFeatures].command);

    numFeatures++;
    index = end + 1;
  }
}

String extractStringValue(String data, const char* fieldName) {
  int start = data.indexOf(fieldName);
  if (start >= 0) {
    start += strlen(fieldName);
    int end = data.indexOf("\"", start);
    return data.substring(start, end);  // Return the extracted string directly
  }
  return "";  // Return an empty string if the field is not found
}


int extractIntValue(String data, const char* fieldName) {
  int start = data.indexOf(fieldName);
  if (start >= 0) {
    start += strlen(fieldName);  // Move past the fieldName
    int end = data.indexOf(",", start);
    if (end == -1) end = data.indexOf("}", start);       // If there's no comma, look for the closing curly brace
    String extractedIdStr = data.substring(start, end);  // Extract the string representation of the integer
    extractedIdStr.replace("\"", "");                    // Remove double quotes
    // Serial.println("Extracted ID string (after removing quotes): " + extractedIdStr);  // For debugging

    return extractedIdStr.toInt();  // Convert the cleaned substring to an integer
  }
  return -1;  // Return -1 if field not found
}
