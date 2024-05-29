#ifndef SOIL_MOISTURE
#define SOIL_MOISTURE
#include <Arduino.h>

void setupSoilMoisture(uint8_t pin);
float getSoilMoisture(uint8_t pin);

#endif