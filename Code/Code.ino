#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Inisialisasi Alamat I2C LCD (0x27 atau 0x3F)
LiquidCrystal_I2C lcd(0x20, 16, 2);
Servo barrierServo;

// Definisi Pin Komponen
const int signalPin = 2; // Pin gabungan untuk Sensor Ultrasonik 3-Pin
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

  // --- PROSES PEMANCARAN PULSA (OUTPUT) ---
  pinMode(signalPin, OUTPUT);
  digitalWrite(signalPin, LOW);
  delayMicroseconds(2);
  digitalWrite(signalPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(signalPin, LOW);

  // --- PROSES PEMBACAAN PANTULAN (INPUT) ---
  pinMode(signalPin, INPUT);
  duration = pulseIn(signalPin, HIGH);
  
  // Menghitung Jarak dalam cm (Kecepatan Suara = 0.0343 cm/us)
  distance = (duration * 0.0343) / 2;

  // Logika Deteksi Kendaraan (Jarak < 10 cm)
  if (distance > 0 && distance < 10) {
    // Sinyal Suara & Lampu Indikator Masuk
    digitalWrite(ledMerah, LOW);
    digitalWrite(ledHijau, HIGH);
    tone(buzzerPin, 1000, 200); // Bunyi bip 200ms

    // Tampilan LCD
    lcd.setCursor(0, 0);
    lcd.print("MOBIL TERDETEKSI");
    lcd.setCursor(0, 1);
    lcd.print("SILAKAN MASUK! ");

    // Buka Palang Servo ke 90 Derajat
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