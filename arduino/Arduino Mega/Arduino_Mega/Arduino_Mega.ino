#define RELAY1 40
#define RELAY2 38
#include "relay_functions.h"

void setup() {
  
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);

  // digitalWrite(RELAY1, LOW);
  // digitalWrite(RELAY2, LOW);
  turnOffRelay(RELAY1);
  turnOffRelay(RELAY2);
  delay(1500);

  turnOnRelay(RELAY1);

  delay(2000);

  turnOnRelay(RELAY2);

  delay(1500);
  turnOffRelay(RELAY1);
  turnOffRelay(RELAY2);
}

void loop() {
}
