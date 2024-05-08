#include "relay_functions.h"
#define RELAY1 40
#define RELAY2 38

#include "light_sensor.h"
#define LIGHTSENSOR A1

#include "servo_functions.h"
#define SERVOPIN 9

Servo servo;

void setup() {
  Serial.begin(9600);
  Serial.println("beginning");

  setupServo(servo, SERVOPIN);
  setServoAngle(servo, 90);

  delay(500);
  setServoAngle(servo, 180);

  delay(2500);
  setServoAngle(servo, 90);

  // myServo.write(180);
  // delay(1500);

  // myServo.write(90);
  // delay(1500);

  // myServo.write(0);
  // delay(1500);
}

void loop() {

  // delay(500);
  // Serial.println(analogRead(A1));
  // Serial.println(getLight(LIGHTSENSOR));
  // if (Serial.available() > 0) {
  //   // Citește caracterul primit
  //   String receivedString = Serial.readString();
  //   receivedString.trim();

  //   if (strcmp(receivedString.c_str(), "stanga usor") == 0) {
  //     Serial.println("da frate");
  //     myServo.write(113);
  //   }

  //   if (strcmp(receivedString.c_str(), "stanga tare") == 0) {
  //     myServo.write(180);
  //   }

  //   if (strcmp(receivedString.c_str(), "dreapta usor") == 0) {
  //     myServo.write(67);
  //   }

  //   if (strcmp(receivedString.c_str(), "dreapta tare") == 0) {
  //     myServo.write(0);
  //   }

  //   if (strcmp(receivedString.c_str(), "stop") == 0) {
  //     myServo.write(90);
  //   }
  // }
}
