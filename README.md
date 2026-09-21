# 🚗 Smart Parking Barrier Gate System (Arduino Uno R3)

Sistem Palang Parkir Otomatis berbasis **Arduino Uno R3** yang memanfaatkan **Sensor Ultrasonik (3-Pin)** untuk mendeteksi keberadaan kendaraan, **Motor Servo SG90** sebagai penggerak palang pintu, serta **LCD 16x2 I2C**, **Active Buzzer**, dan **LED** sebagai indikator status interaktif [1-3].

---

## 📸 Skema & Alur Kerja Sistem

1. **Kondisi Standby**:
   * Palang Servo berada di posisi **0°** (tertutup) [4].
   * **LED Merah** menyala, **LED Hijau** mati [4].
   * LCD menampilkan status `"PALANG PARKIR / STATUS: STANDBY"`.
2. **Kondisi Kendaraan Terdeteksi (Jarak < 10 cm)**:
   * Sensor Ultrasonik memancarkan pulsa gelombang suara untuk mengukur jarak kendaraan mendekat [1, 3].
   * Buzzer berbunyi **bip singkat** dan **LED Hijau** menyala.
   * LCD menampilkan teks `"MOBIL TERDETEKSI / SILAKAN MASUK!"`.
   * Motor Servo berputar **90°** untuk mengangkat palang [1, 4].
   * Palang terbuka selama **4 detik**, lalu menutup kembali secara otomatis ke posisi **0°** [4].

---

## 🛠️ Daftar Komponen (Hardware)

* **Mikrokontroler**: Arduino Uno R3 [5]
* **Sensor Jarak**: Sensor Ultrasonik HC-SR04 / 3-Pin Module [1, 6]
* **Aktuator Palang**: Motor Servo SG90 (9G) [1]
* **Display**: LCD 16x2 dengan Modul I2C Backpack (PCF8574) [2, 7]
* **Indikator**: Active Buzzer 5V, LED Merah (220Ω), LED Hijau (220Ω)
* **Daya / Power**: Power Supply 5V DC External (Common Ground dengan Arduino)

---

## 🔌 Skema Pengabelan (Pinout Diagram)

| Komponen | Pin Modul | Pin Arduino Uno R3 | Catatan |
| :--- | :--- | :--- | :--- |
| **Sensor Ultrasonik (3-Pin)** | VCC | **5V** | Power 5V DC [6] |
| | GND | **GND** | Ground [6] |
| | SIG / Signal | **D2** | Pin Gabungan Trig & Echo [6] |
| **Motor Servo SG90** | VCC (Merah) | **5V / External 5V** | Power Servo [8] |
| | GND (Cokelat/Hitam) | **GND** | Common Ground [8] |
| | Signal (Oranye) | **D9** | Sinyal PWM Kontrol Sudut [8, 9] |
| **LCD 16x2 I2C** | VCC | **5V** | Power LCD [10] |
| | GND | **GND** | Ground |
| | SDA | **A4** | Serial Data I2C [11] |
| | SCL | **A5** | Serial Clock I2C [9] |
| **Active Buzzer** | (+) VCC | **D4** | Output Digital [9] |
| | (-) GND | **GND** | Ground [12] |
| **LED Merah** | Anoda (+) | **D5** | Resistor Seri 220Ω [9] |
| **LED Hijau** | Anoda (+) | **D6** | Resistor Seri 220Ω [9] |

> **Catatan Pengabelan Daya**: Jika menggunakan *power supply* 5V eksternal untuk Servo dan LCD, **wajib menghubungkan GND Power Supply Eksternal ke GND Arduino** (*Common Ground*) agar sinyal komunikasi data tetap stabil.

---

## 💻 Kode Program (`main.ino`)

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Inisialisasi Alamat I2C LCD (0x27 atau 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo barrierServo;

// Definisi Pin Komponen
const int signalPin = 2; // Pin gabungan Sensor Ultrasonik 3-Pin
const int buzzerPin = 4;
const int ledMerah = 5;
const int ledHijau = 6;
const int servoPin = 9;

void setup() {
  // Inisialisasi Pin Indikator
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledMerah, OUTPUT);
  pinMode(ledHijau, OUTPUT);

  // Inisialisasi Servo (0 Derajat = Tertutup)
  barrierServo.attach(servoPin);
  barrierServo.write(0);

  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" SMART BARRIER ");
  lcd.setCursor(0, 1);
  lcd.print(" GATE SYSTEM ");
  delay(2000);
  lcd.clear();
}

void loop() {
  long duration;
  float distance;

  // --- PEMANCARAN PULSA ULTRASONIK (OUTPUT) ---
  pinMode(signalPin, OUTPUT);
  digitalWrite(signalPin, LOW);
  delayMicroseconds(2);
  digitalWrite(signalPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(signalPin, LOW);

  // --- PEMBACAAN PANTULAN GEMA (INPUT) ---
  pinMode(signalPin, INPUT);
  duration = pulseIn(signalPin, HIGH);
  
  // Menghitung Jarak (Kecepatan Suara = 0.0343 cm/us)
  distance = (duration * 0.0343) / 2;

  // Logika Deteksi Kendaraan (Jarak < 10 cm)
  if (distance > 0 && distance < 10) {
    // Sinyal Suara & Lampu Indikator Masuk
    digitalWrite(ledMerah, LOW);
    digitalWrite(ledHijau, HIGH);
    tone(buzzerPin, 1000, 200); // Bip 200ms

    // Tampilan LCD
    lcd.setCursor(0, 0);
    lcd.print("MOBIL TERDETEKSI");
    lcd.setCursor(0, 1);
    lcd.print("SILAKAN MASUK! ");

    // Angkat Palang Servo ke 90 Derajat
    barrierServo.write(90);
    
    // Tahan Terbuka Selama 4 Detik
    delay(4000);

    // Tutup Kembali Palang
    barrierServo.write(0);
    lcd.clear();
  } else {
    // Kondisi Standby
    barrierServo.write(0);
    digitalWrite(ledMerah, HIGH);
    digitalWrite(ledHijau, LOW);

    lcd.setCursor(0, 0);
    lcd.print(" PALANG PARKIR ");
    lcd.setCursor(0, 1);
    lcd.print("STATUS: STANDBY ");
  }

  delay(100);
}
