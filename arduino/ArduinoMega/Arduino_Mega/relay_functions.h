#ifndef RELAY_FUNCTIONS_H
#define RELAY_FUNCTIONS_H
#include <Arduino.h>

void setupRelay(uint8_t pin);
void turnOnRelay(uint8_t relay);
void turnOffRelay(uint8_t relay);

#endif