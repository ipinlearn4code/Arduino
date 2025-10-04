#include <Wire.h>
#include <BH1750.h>
#include <LiquidCrystal_I2C.h>

// Inisialisasi sensor BH1750 (alamat 0x23)
BH1750 lightMeter(0x23);

// Inisialisasi LCD 20 kolom, 4 baris (alamat 0x27)
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  Serial.begin(115200);
  Wire.begin(); // Gunakan pin default ESP32 (SDA = 21, SCL = 22)

  // Inisialisasi sensor BH1750
  if (lightMeter.begin()) {
    Serial.println("BH1750 berhasil diinisialisasi.");
  } else {
    Serial.println("Gagal menginisialisasi BH1750.");
    while (1);
  }

  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Sensor Cahaya Siap");
  delay(1000);
  lcd.clear();
}

void loop() {
  float lux = lightMeter.readLightLevel();

  // Serial Monitor
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");

  // Tampilkan di LCD 20x4
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cahaya:");

  lcd.setCursor(0, 1);
  lcd.print("Lux: ");
  lcd.print(lux, 2); // Tampilkan dengan 2 digit desimal

  lcd.setCursor(0, 3);
  lcd.print("Update setiap 1 sec");

  delay(1000); // Update setiap detik
  lcd.clear();
  delay(1000);
}
