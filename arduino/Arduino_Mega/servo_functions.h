#ifndef SERVO_FUNCTIONS
#define SERVO_FUNCTIONS
#include <Arduino.h>
#include <Servo.h>
#include "constants.h"
void setupServo(Servo servo, int pin);
void setServoAngle(Servo servo, int angle);
void closeServo(Servo servo);
void openServo(Servo servo);
#endif