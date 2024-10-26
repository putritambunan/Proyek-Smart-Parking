#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define INFRARED_IN 2    // Pin sensor infrared pintu masuk
#define INFRARED_OUT 3   // Pin sensor infrared pintu keluar

#define SERVO_IN_PIN 4   // Pin servo pintu masuk
#define SERVO_OUT_PIN 5  // Pin servo pintu keluar

#define PARKING_SLOTS 5 // Jumlah slot parkir

// Pin untuk sensor infrared di setiap slot parkir
const int parkingInfraredPins[PARKING_SLOTS] = {6, 7, 8, 9, 10};

Servo servoIn;
Servo servoOut;

LiquidCrystal_I2C lcd(0x27, 16, 2); // Alamat LCD: 0x27, Ukuran: 16x2

bool vehicleIn = false;
bool vehicleOut = false;

void setup() {
  Serial.begin(9600);
  pinMode(INFRARED_IN, INPUT);
  pinMode(INFRARED_OUT, INPUT);
  
  servoIn.attach(SERVO_IN_PIN);
  servoOut.attach(SERVO_OUT_PIN);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Slot Parkir: ");
}

void loop() {
  int availableSlots = countAvailableSlots();
  
  lcd.setCursor(12,0);
  lcd.print(availableSlots);

  // Tambahkan teks "Parkiran penuh!" pada baris kedua saat tidak ada slot parkir yang tersedia
  lcd.setCursor(0,1);
  if (availableSlots == 0) {
    lcd.print("Parkiran penuh!");
  } else {
    lcd.print("               "); // Membersihkan teks jika slot tersedia
  }

  // Jika tidak ada slot parkir yang tersedia, servo tidak akan bergerak
  if (availableSlots > 0) {
    // Check sensor infrared pintu masuk
    if (digitalRead(INFRARED_IN) == HIGH && !vehicleIn) {
      vehicleIn = true;
      openBarrier(true); // Buka palang pintu masuk
      lcd.clear(); // Membersihkan layar LCD
      lcd.setCursor(0, 0);
      lcd.print("Selamat Datang!"); // Menampilkan pesan selamat datang
      delay(2000); // Tunggu 2 detik
      lcd.clear(); // Membersihkan layar LCD
      lcd.setCursor(0, 0);
      lcd.print("Slot Parkir: "); // Menampilkan kembali pesan slot parkir
      lcd.setCursor(12,0);
      lcd.print(availableSlots);

    }
    else if (digitalRead(INFRARED_IN) == LOW && vehicleIn) {
      vehicleIn = false;
      closeBarrier(true); // Tutup palang pintu masuk
    }
  }

  // Check sensor infrared pintu keluar
  if (digitalRead(INFRARED_OUT) == HIGH && !vehicleOut) {
    vehicleOut = true;
    openBarrier(false); // Buka palang pintu keluar
  }
  else if (digitalRead(INFRARED_OUT) == LOW && vehicleOut) {
    vehicleOut = false;
    closeBarrier(false); // Tutup palang pintu keluar
  }

  delay(100);
}

// Menghitung slot parkir yang tersedia
int countAvailableSlots() {
  int availableSlots = PARKING_SLOTS; // Mulai dengan semua slot tersedia

  // Iterasi melalui setiap slot parkir
  for (int i = 0; i < PARKING_SLOTS; i++) {
    // Baca status sensor infrared di slot ini
    int infraredValue = digitalRead(parkingInfraredPins[i]);

    // Jika sensor mendeteksi kendaraan, kurangi slot yang tersedia
    if (infraredValue == LOW) {
      availableSlots--;
    }
  }

  return availableSlots;
}

// Buka palang pintu
void openBarrier(bool isIn) {
  if (isIn) {
    servoIn.write(0); // Rotasi servo pintu masuk
    delay(100);
  } else {
    servoOut.write(180); // Rotasi servo pintu keluar
    delay(100);
  }
}

// Tutup palang pintu
void closeBarrier(bool isIn) {
  if (isIn) {
    servoIn.write(90); // Rotasi servo pintu masuk ke posisi default
    delay(2000);
  } else {
    servoOut.write(90); // Rotasi servo pintu keluar ke posisi default
    delay(2000);
  }
}
