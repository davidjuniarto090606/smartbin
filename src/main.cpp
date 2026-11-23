
#include <Arduino.h>
#include "HX711.h"

// Pin HC-SR04
#define TRIG_PIN 25
#define ECHO_PIN 26

// Pin HX711
#define HX711_DT 27
#define HX711_SCK 14

// Konfigurasi sensor
#define TINGGI_TEMPAT_SAMPAH 30.0

// Ganti dengan hasil kalibrasi Load Cell kalian
#define CALIBRATION_FACTOR -7050.0

HX711 scale;

// Membaca jarak menggunakan HC-SR04
float bacaJarak() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long durasi = pulseIn(ECHO_PIN, HIGH, 30000);

    if (durasi == 0) {
        return -1;
    }

    float jarak = durasi * 0.0343 / 2.0;
    return jarak;
}

// Menghitung persentase kepenuhan
float hitungKepenuhan(float jarak) {
    if (jarak < 0) {
        return -1;
    }

    float tinggiSampah =
        TINGGI_TEMPAT_SAMPAH - jarak;

    float persen =
        (tinggiSampah / TINGGI_TEMPAT_SAMPAH) * 100.0;

    return constrain(persen, 0.0, 100.0);
}

// Membaca berat menggunakan HX711
float bacaBerat() {
    if (!scale.is_ready()) {
        return -1;
    }

    float berat = scale.get_units(5);

    if (berat < 0) {
        berat = 0;
    }

    return berat;
}

void setup() {
    Serial.begin(115200);

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    scale.begin(HX711_DT, HX711_SCK);

    Serial.println();
    Serial.println("================================");
    Serial.println("       SMARTBIN SYSTEM");
    Serial.println("  IoT Electronic Waste System");
    Serial.println("================================");

    if (scale.is_ready()) {
        scale.set_scale(CALIBRATION_FACTOR);
        scale.tare();

        Serial.println("HX711 berhasil diinisialisasi");
    } else {
        Serial.println("Peringatan: HX711 belum terdeteksi");
    }

    Serial.println("Sistem siap membaca sensor.");
}

void loop() {
    float jarak = bacaJarak();
    float kepenuhan = hitungKepenuhan(jarak);
    float berat = bacaBerat();

    Serial.println("--------------------------------");

    if (jarak >= 0) {
        Serial.print("Jarak Sampah     : ");
        Serial.print(jarak, 2);
        Serial.println(" cm");

        Serial.print("Kepenuhan Bin    : ");
        Serial.print(kepenuhan, 1);
        Serial.println(" %");
    } else {
        Serial.println("Sensor HC-SR04 tidak terbaca");
    }

    if (berat >= 0) {
        Serial.print("Berat Sampah     : ");
        Serial.print(berat, 2);
        Serial.println(" kg");
    } else {
        Serial.println("Sensor HX711 tidak terbaca");
    }

    Serial.println("--------------------------------");

    delay(2000);
}
