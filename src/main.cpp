#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AS5600.h>

#define I2C1_SDA 21
#define I2C1_SCL 22

Adafruit_AS5600 theta_as5600;

class MotorDrive {
public:
  int dirpin;
  int motorpwm;
  int pwmch;

  MotorDrive(int pin1, int pin2, int ch) {
    dirpin = pin1;
    motorpwm = pin2;
    pwmch = ch;
  }

  void setup() {
    pinMode(dirpin, OUTPUT);
    pinMode(motorpwm, OUTPUT);
    ledcAttachPin(motorpwm, pwmch);
    ledcSetup(pwmch, 12800, 8);
  }

  void drive(int val) {
    val = constrain(val, -255, 255);

    if (val < 0) {
      digitalWrite(dirpin, HIGH);
      ledcWrite(pwmch, -val);
    }
    else if (val > 0) {
      digitalWrite(dirpin, LOW);
      ledcWrite(pwmch, val);
    }
    else {
      digitalWrite(dirpin, LOW);
      ledcWrite(pwmch, 0);
    }
  }
};

const int theta_pin = 18;
const int theta_pwm = 19;
const int theta_ch = 0;

MotorDrive theta_M{theta_pin, theta_pwm, theta_ch};

void setup() {
  Serial.begin(115200);
  delay(1000);

  // I2C
  Wire.begin(I2C1_SDA, I2C1_SCL, 400000);

  Serial.println("=== AS5600 TEST ===");

  // Theta側AS5600
  if (theta_as5600.begin(AS5600_DEFAULT_ADDR, &Wire)) {
    Serial.println("Theta AS5600: detected");
  }
  else {
    Serial.println("Theta AS5600: NOT detected");
  }

  // 磁石検出状態
  Serial.println();
  Serial.println("=== Magnet Check ===");

  if (theta_as5600.isMagnetDetected()) {
    Serial.println("Theta magnet: OK");
  }
  else {
    Serial.println("Theta magnet: NOT detected");
  }

  Serial.println();
  Serial.println("=== Angle Reading ===");
}

void loop() {

  // Theta AS5600
  uint16_t theta_raw = theta_as5600.getRawAngle();

  // Raw値 → 角度
  float theta_deg = theta_raw * 360.0 / 4096.0;

  // 表示
  Serial.print("Theta : RAW = ");
  Serial.print(theta_raw);

  Serial.print("  Angle = ");
  Serial.print(theta_deg);

  Serial.println(" deg");

  Serial.println("--------------------");

  delay(10);
}