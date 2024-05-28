#ifndef LIGHT_SENSOR
#define LIGHT_SENSOR
#include <Arduino.h>

void setupLightSensor(uint8_t pin);
float getLight(uint8_t pin);

#endif