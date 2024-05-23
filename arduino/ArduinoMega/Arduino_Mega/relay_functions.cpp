#include "relay_functions.h"

void setupRelay(uint8_t pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
}

void turnOnRelay(uint8_t relay) {
  digitalWrite(relay, LOW);
}

void turnOffRelay(uint8_t relay) {
  digitalWrite(relay, HIGH);
}