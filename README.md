# Sistem Monitoring Kualitas Udara Berbasis ESP32 dan Telegram

## Deskripsi
Sistem ini merupakan proyek IoT untuk memantau kualitas udara ruangan menggunakan ESP32. Sistem membaca kondisi udara menggunakan sensor MQ-135 serta dan kelembapan menggunakan sensor DHT11. 

Hasil pembacaan ditampilkan pada OLED dan sistem dapat memberikan informasi atau peringatan melalui Telegram.

## Komponen
-ESP32
-Sensor MQ-135
-Sensor DHT11
-OLED 128x64
-LED
-BUZZER
-Kabel Jumper

## Software
-Visual Studio Code
-PlatformIO
-Arduino C++
-Telegram Bot

## Cara Kerja
1. ESP32 terhubung ke jaringan WIFI.
2. MQ-135 membaca kondisi kualitas udara.
3. DHT11 membaca suhu dan kelembapan.
4. Data ditampilkan pada OLED.
5. Jika kualitas udara melewati batas yang ditentukan,LED dan BUZZER memberikan peringatan.
6. Informasi kondisi udara dikirim melalui Telegram.

## Batas Kualitas Udara
-Nilai MQ-135 < 2000  = Normal
-Nilai MQ-135 >= 2000 = Tidak Normal

## Struktur Project
''' text
Sistem-Monitoring-Kualitas-Udara/
|---- src/
|      |---- main.cpp
|----README.md
