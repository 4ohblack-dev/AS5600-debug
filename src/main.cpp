#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AS5600.h>

#define SDA2_pin 25
#define SCL2_pin 32

#define I2C1_SDA 21
#define I2C1_SCL 22

TwoWire I2C_2 = TwoWire(1);

Adafruit_AS5600 theta_as5600;
Adafruit_AS5600 length_as5600;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // I2Cバス1
  Wire.begin(I2C1_SDA, I2C1_SCL, 400000);

  // I2Cバス2
  I2C_2.begin(SDA2_pin, SCL2_pin, 400000);

  Serial.println("=== AS5600 TEST ===");

  // Theta側
  if (theta_as5600.begin(AS5600_DEFAULT_ADDR, &Wire)) {
    Serial.println("Theta AS5600: detected");
  } else {
    Serial.println("Theta AS5600: NOT detected");
  }

  // Length側
  if (length_as5600.begin(AS5600_DEFAULT_ADDR, &I2C_2)) {
    Serial.println("Length AS5600: detected");
  } else {
    Serial.println("Length AS5600: NOT detected");
  }

  // 磁石検出状態
  Serial.println();
  Serial.println("=== Magnet Check ===");

  if (theta_as5600.isMagnetDetected()) {
    Serial.println("Theta magnet: OK");
  } else {
    Serial.println("Theta magnet: NOT detected");
  }

  if (length_as5600.isMagnetDetected()) {
    Serial.println("Length magnet: OK");
  } else {
    Serial.println("Length magnet: NOT detected");
  }

  Serial.println();
  Serial.println("=== Angle Reading ===");
}

void loop() {
  // Theta AS5600
  uint16_t theta_raw = theta_as5600.getRawAngle();
  float theta_deg = theta_raw * 360.0 / 4096.0;

  // Length AS5600
  uint16_t length_raw = length_as5600.getRawAngle();
  float length_deg = length_raw * 360.0 / 4096.0;

  Serial.print("Theta : RAW = ");
  Serial.print(theta_raw);
  Serial.print("  Angle = ");
  Serial.print(theta_deg);
  Serial.println(" deg");

  Serial.print("Length: RAW = ");
  Serial.print(length_raw);
  Serial.print("  Angle = ");
  Serial.print(length_deg);
  Serial.println(" deg");

  Serial.println("--------------------");

  delay(200);
}