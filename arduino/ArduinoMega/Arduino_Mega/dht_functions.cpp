#include "dht_functions.h"

DHT22 setupDHT(uint8_t pin) {
  return dht22(pin);
}

float getDHTTemperature(DHT22 dht) {
  return dht.getTemperature();
}

float get DHTHumidity(DHT22 dht) {
  return dht.getHumidity();
}