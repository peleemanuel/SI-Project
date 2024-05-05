#include "relay_functions.h"

void turnOnRelay(uint8_t relay) {
  digitalWrite(relay, LOW);
}

void turnOffRelay(uint8_t relay) {
  digitalWrite(relay, HIGH);
}