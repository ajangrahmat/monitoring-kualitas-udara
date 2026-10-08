#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// =========================
// WIFI
// =========================
const char* ssid = "Kelas Robot";
const char* password = "kumaha aa we";

// =========================
// TELEGRAM
// =========================
#define BOT_TOKEN "8246234453:AAEcoP9k5teEFOBovHwvB2_e2vAqL5FMXuQ"
#define CHAT_ID "5946869783"

WiFiClientSecure client;
UniversalTelegramBot bot("8246234453:AAEcoP9k5teEFOBovHwvB2_e2vAqL5FMXuQ", client);

// =========================
// PIN
// =========================
#define DHTPIN 4
#define DHTTYPE DHT11

#define MQ135_PIN 34
#define LED_PIN 2
#define BUZZER_PIN 25

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// =========================
// SENSOR & OLED
// =========================
DHT dht(DHTPIN, DHTTYPE);

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// =========================
// VARIABEL STATUS
// =========================
bool statusSebelumnya = false;
bool pertamaKali = true;

// =========================
// SETUP
// =========================
void setup() {

  Serial.begin(115200);

  delay(1000);

  // =========================
  // PIN
  // =========================
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Buzzer
  ledcSetup(0, 2000, 8);
  ledcAttachPin(BUZZER_PIN, 0);

  // DHT
  dht.begin();

  // I2C OLED
  Wire.begin(21, 22);

  // =========================
  // OLED
  // =========================
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {

    Serial.println("OLED tidak ditemukan!");

    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("MONITORING UDARA");

  display.setCursor(0, 20);
  display.println("Menghubungkan WiFi...");

  display.display();

  // =========================
  // WIFI
  // =========================
  WiFi.begin(ssid, password);

  Serial.print("Menghubungkan ke WiFi");

  unsigned long waktuMulai = millis();

  while (
    WiFi.status() != WL_CONNECTED &&
    millis() - waktuMulai < 15000
  ) {

    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("WiFi terhubung!");

    Serial.print("IP ESP32: ");
    Serial.println(WiFi.localIP());

    // Untuk koneksi HTTPS Telegram
    client.setInsecure();

    // Kirim pesan bahwa ESP32 sudah online
    bot.sendMessage(
      CHAT_ID,
      "ESP32 berhasil terhubung ke WiFi.\nMonitoring kualitas udara dimulai.",
      ""
    );

  } else {

    Serial.println("WiFi tidak terhubung.");
    Serial.println("ESP32 tetap menjalankan sensor.");
  }

  delay(1000);
}

// =========================
// LOOP
// =========================
void loop() {

  // =========================
  // BACA SENSOR
  // =========================
  float suhu = dht.readTemperature();
  float kelembapan = dht.readHumidity();

  int udara = analogRead(MQ135_PIN);

  // =========================
  // CEK DHT11
  // =========================
  if (isnan(suhu) || isnan(kelembapan)) {

    Serial.println("DHT11 gagal membaca!");

    digitalWrite(LED_PIN, LOW);
    ledcWriteTone(0, 0);

    return;
  }

  // =========================
  // STATUS KUALITAS UDARA
  // =========================
  bool tidakNormal = udara > 2000;

  String statusUdara;

  if (tidakNormal) {
    statusUdara = "TIDAK NORMAL";
  } else {
    statusUdara = "NORMAL";
  }

  // =========================
  // SERIAL MONITOR
  // =========================
  Serial.print("Suhu: ");
  Serial.print(suhu);

  Serial.print(" C | Kelembapan: ");
  Serial.print(kelembapan);

  Serial.print(" % | MQ-135: ");
  Serial.print(udara);

  Serial.print(" | Status: ");
  Serial.println(statusUdara);

  // =========================
  // LED & BUZZER
  // =========================
  if (tidakNormal) {

    digitalWrite(LED_PIN, HIGH);

    ledcWriteTone(0, 2000);

  } else {

    digitalWrite(LED_PIN, LOW);

    ledcWriteTone(0, 0);
  }

  // =========================
  // OLED
  // =========================
  display.clearDisplay();

  display.setCursor(0, 0);
  display.println("MONITORING UDARA");

  display.setCursor(0, 13);
  display.print("MQ135 : ");
  display.println(udara);

  display.setCursor(0, 26);
  display.print("Suhu  : ");
  display.print(suhu, 1);
  display.println(" C");

  display.setCursor(0, 39);
  display.print("Lembap: ");
  display.print(kelembapan, 1);
  display.println(" %");

  display.setCursor(0, 52);
  display.print("Status: ");
  display.println(statusUdara);

  display.display();

  // =========================
  // TELEGRAM
  // =========================
  if (WiFi.status() == WL_CONNECTED) {

    // Kirim hanya jika status berubah
    if (tidakNormal != statusSebelumnya || pertamaKali) {

      String pesan;

      if (tidakNormal) {

        pesan = "⚠️ PERINGATAN KUALITAS UDARA!\n\n";

      } else {

        pesan = "✅ KUALITAS UDARA NORMAL\n\n";
      }

      pesan += "MQ-135: ";
      pesan += String(udara);
      pesan += "\n";

      pesan += "Suhu: ";
      pesan += String(suhu, 1);
      pesan += " C\n";

      pesan += "Kelembapan: ";
      pesan += String(kelembapan, 1);
      pesan += " %\n";

      pesan += "Status: ";
      
      if (tidakNormal) {
        pesan += "TIDAK NORMAL";
      } else {
        pesan += "NORMAL";
      }

      bot.sendMessage(
        CHAT_ID,
        pesan,
        ""
      );

      Serial.println("Notifikasi Telegram dikirim.");

      statusSebelumnya = tidakNormal;
      pertamaKali = false;
    }
  }

  delay(2000);
}