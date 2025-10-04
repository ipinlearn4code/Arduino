#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

// Ganti dengan SSID dan password WiFi kamu
const char* ssid = "Alpinnn";
const char* password = "88888888";

// Pinout ESP32-CAM AI-Thinker
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

WebServer server(80);

// Fungsi untuk stream MJPEG
void handle_jpg_stream() {
  WiFiClient client = server.client();
  camera_fb_t *fb = NULL;

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: multipart/x-mixed-replace; boundary=frame");
  client.println();

  while (client.connected()) {
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Gagal mengambil frame kamera");
      break;
    }

    client.printf("--frame\r\n");
    client.printf("Content-Type: image/jpeg\r\n");
    client.printf("Content-Length: %u\r\n\r\n", fb->len);
    client.write(fb->buf, fb->len);
    client.println();
    esp_camera_fb_return(fb);
  }

  Serial.println("Client terputus dari stream");
}

// Mulai server kamera
void startCameraServer() {
  server.on("/stream", HTTP_GET, handle_jpg_stream);
  server.begin();
  Serial.println("Server stream dimulai di endpoint /stream");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n[SETUP] Memulai inisialisasi...");
  
  // Debug PSRAM
  if (psramInit()) {
    Serial.printf("[PSRAM] Ditemukan. Total: %d bytes\n", ESP.getPsramSize());
    Serial.printf("[PSRAM] Bebas:    %d bytes\n", ESP.getFreePsram());
  } else {
    Serial.println("[PSRAM] Tidak ditemukan!");
  }

  // Konfigurasi kamera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.frame_size = FRAMESIZE_VGA;     // VGA = 640x480
  config.jpeg_quality = 15;              // 0 (terbaik) - 63 (terburuk)
  config.fb_count = 2;

  if (psramFound()) {
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    config.fb_location = CAMERA_FB_IN_DRAM;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  }

  // Inisialisasi kamera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("[CAMERA] Gagal inisialisasi kamera: 0x%x\n", err);
    return;
  }
  Serial.println("[CAMERA] Inisialisasi berhasil");

  // Koneksi WiFi
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  Serial.print("[WiFi] Menyambungkan");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n[WiFi] Terhubung");
  Serial.print("[WiFi] IP Address: ");
  Serial.println(WiFi.localIP());

  startCameraServer();

  Serial.println("[INFO] Kamera siap! Akses di http://<ip_address>/stream");
}

void loop() {
  server.handleClient();
}
