#include "light_sensor.h"

void setupLightSensor(uint8_t pin) {
  pinMode(pin, INPUT);
}

float getLight(uint8_t pin) {
  return analogRead(pin);
}