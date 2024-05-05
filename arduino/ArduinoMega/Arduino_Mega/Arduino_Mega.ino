#include "relay_functions.h"
#define RELAY1 40
#define RELAY2 38

#include "light_sensor.h"
#define LIGHTSENSOR A1

void setup() {
  Serial.begin(9600);
  setupLightSensor(LIGHTSENSOR);
}

void loop() {

  // Serial.println(getLight(LIGHTSENSOR));
  // if (Serial.available() > 0) {
  //   // Citește caracterul primit
  //   String receivedString = Serial.readString();
  //   receivedString.trim();

  //   if (strcmp(receivedString.c_str(), "read") == 0) {
  //     reading = analogRead(LIGHTSENSOR);
  //     Serial.println(reading);
  //   }
  // }
}
