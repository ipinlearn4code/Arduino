#include <WiFi.h>
#include <WebServer.h>

// Membuat server pada port 80
WebServer server(80);

// Fungsi helper untuk semua perintah
void handleCommand(String command) {
  Serial.println("Command received: " + command);
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);

  // Mode Access Point
  WiFi.softAP("ESP32_Remote", "12345678");
  IPAddress IP = WiFi.softAPIP();
  Serial.print("ESP32 AP IP address: ");
  Serial.println(IP);

  // —————— ENDPOINT ARAH ——————
  server.on("/forward",  []() { handleCommand("FORWARD");  });
  server.on("/backward", []() { handleCommand("BACKWARD"); });
  server.on("/left",     []() { handleCommand("LEFT");     });
  server.on("/right",    []() { handleCommand("RIGHT");    });

  // —————— ENDPOINT AUX BUTTONS ——————
  server.on("/1a", []() { handleCommand("AUX1A"); });
  server.on("/1b", []() { handleCommand("AUX1B"); });
  server.on("/2a", []() { handleCommand("AUX2A"); });
  server.on("/2b", []() { handleCommand("AUX2B"); });
  server.on("/3a", []() { handleCommand("AUX3A"); });
  server.on("/3b", []() { handleCommand("AUX3B"); });
  server.on("/4a", []() { handleCommand("AUX4A"); });
  server.on("/4b", []() { handleCommand("AUX4B"); });
  server.on("/5a", []() { handleCommand("AUX5A"); });
  server.on("/5b", []() { handleCommand("AUX5B"); });
  server.on("/6a", []() { handleCommand("AUX6A"); });
  server.on("/6b", []() { handleCommand("AUX6B"); });

  // —————— ENDPOINT SLIDER SPEED ——————
  server.on("/speed", []() {
    if (server.hasArg("value")) {
      String speedVal = server.arg("value");
      Serial.println("Speed set to: " + speedVal);
      server.send(200, "text/plain", "OK");
    } else {
      server.send(400, "text/plain", "Missing value");
    }
  });

  // Mulai Web Server
  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  // Tangani setiap HTTP request yang masuk
  server.handleClient();
}
