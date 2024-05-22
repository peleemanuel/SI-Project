#include "relay_functions.h"

void setupRelay(uint8_t pin) {
  pinMode(pin, OUTPUT);
}

void turnOnRelay(uint8_t relay) {
  digitalWrite(relay, LOW);
}

void turnOffRelay(uint8_t relay) {
  digitalWrite(relay, HIGH);
}