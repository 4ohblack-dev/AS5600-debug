#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AS5600.h>

#define I2C1_SDA 21
#define I2C1_SCL 22

const int FIXED_PWM = 50;

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

struct currentState{
  double current_theta;
  double direction;
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

void feedback_move(MotorDrive &motor,Adafruit_AS5600 &as5600,double angle,int pwm,float tolerance_range){
  currentState state;
  state.current_theta = as5600.getRawAngle() * 360.0 / 4096.0;
  double difference = angle - state.current_theta;

  if(difference < 0) state.direction=0;
  else state.direction = 1;

  if(abs(difference) > tolerance_range){
    if(state.direction == 1){
      motor.drive(pwm);
    } else{
      motor.drive(-pwm);
    }
  } else{
    motor.drive(0);
  }
}

void loop() {

  feedback_move(
    theta_M,
    theta_as5600,
    90.0,   // 目標角度
    100,    // 通常PWM
    2.0     // 許容範囲 ±2°
  );

  uint16_t theta_raw = theta_as5600.getRawAngle();

  float theta_deg =
      theta_raw * 360.0 / 4096.0;

  Serial.print("Theta = ");
  Serial.print(theta_deg);
  Serial.println(" deg");

  delay(10);
}