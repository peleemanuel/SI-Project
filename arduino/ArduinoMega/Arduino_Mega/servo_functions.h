#ifndef SERVO_FUNCTIONS
#define SERVO_FUNCTIONS
#include <Arduino.h>
#include <Servo.h>

void setupServo(Servo servo, int pin);
void setServoAngle(Servo servo, int angle);

#endif