#include "servo_functions.h"

void setupServo(Servo servo, int pin) {
  servo.attach(pin);
}

void setServoAngle(Servo servo, int angle) {
  if (0 <= angle && 180 >= angle) {
    servo.write(angle);
  }
}

void closeServo(Servo servo) {
  servo.write(SERVO_CLOSE);
}

void openServo(Servo servo) {
  servo.write(SERVO_OPEN);
}