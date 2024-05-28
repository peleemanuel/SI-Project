#include "constants.h"
#include "pins.h"
#include "relay_functions.h"
#include "DHT22.h"
#include "light_sensor.h"
#include "servo_functions.h"
#include "soil_moisture.h"

DHT22 dhtInterior(DHT1PIN);
DHT22 dhtExterior(DHT2PIN);

Servo servo;

float soil_hum1;
float soil_hum2;
float soil_hum3;
float soil_hum4;

unsigned long relayWater1Timer = 0;  // Stores the last time RELAY_WATER_1 was turned on
bool relayWater1Active = false;      // Tracks whether the relay should currently be on
unsigned long relayWater2Timer = 0;  // Stores the last time RELAY_WATER_2 was turned on
bool relayWater2Active = false;      // Tracks whether the relay should currently be on
unsigned long relayWater3Timer = 0;  // Stores the last time RELAY_WATER_3 was turned on
bool relayWater3Active = false;      // Tracks whether the relay should currently be on
unsigned long relayWater4Timer = 0;  // Stores the last time RELAY_WATER_4 was turned on
bool relayWater4Active = false;      // Tracks whether the relay should currently be on

bool relayLightBand = false;

float light;
float hum1;
float tempInterior;
float hum2;
float tempExterior;

bool workingMode;

void setup() {
  Serial.begin(115200);
  Serial.println("beginning");

  Serial1.begin(115200);  // Serial1 for TX1 and RX1 pins

  /* Setups */
  setupServo(servo, SERVOPIN);
  setupSoilMoisture(SOIL_SENSOR1);
  setupSoilMoisture(SOIL_SENSOR2);
  setupSoilMoisture(SOIL_SENSOR3);
  setupSoilMoisture(SOIL_SENSOR4);
  setupLightSensor(LIGHTSENSOR);

  setupRelay(RELAY_LED_BAND);
  setupRelay(RELAY_WATER_1);
  setupRelay(RELAY_WATER_2);
  setupRelay(RELAY_WATER_3);
  setupRelay(RELAY_WATER_4);
  setupRelay(RELAY6);
  turnOffRelay(RELAY_LED_BAND);
  turnOffRelay(RELAY_WATER_1);
  turnOffRelay(RELAY_WATER_2);
  turnOffRelay(RELAY_WATER_3);
  turnOffRelay(RELAY_WATER_4);
  turnOffRelay(RELAY6);

  closeServo(servo);

  workingMode = AUTO_MODE;
}

unsigned long previousMillis = 0;

void loop() {
  unsigned long currentMillis = millis();
  // Serial.println("Verific serialul");
  if (Serial1.available() > 0) {

    String receivedString = Serial1.readString();
    int serialOP = 0;
    if (isNumber(receivedString)) {
      serialOP = atoi(receivedString.c_str());
    } else {
      serialOP = 0;
      Serial.println(receivedString);
    }

    Serial.print("Received number: ");
    Serial.println(serialOP);

    // see the command from ESP
    switch (serialOP >> 6) {
      case NO_OP:
        Serial.println("NO_OP");
        break;

      case MODE_OP:
        Serial.println("MODE_OP");

        if (serialOP & MODE_BIT) {
          workingMode = AUTO_MODE;
        } else {
          workingMode = MANUAL_MODE;
          turnOffRelay(RELAY_LED_BAND);
          turnOffRelay(RELAY_WATER_1);
          turnOffRelay(RELAY_WATER_2);
          turnOffRelay(RELAY_WATER_3);
          turnOffRelay(RELAY_WATER_4);
          turnOffRelay(RELAY6);
        }
        break;

      case SERVO_OP:
        Serial.println("SERVO_OP");

        if (workingMode == MANUAL_MODE) {
          if (serialOP & SERVO_BIT) {
            setServoAngle(servo, SERVO_OPEN);
          } else {
            setServoAngle(servo, SERVO_CLOSE);
          }
        }

        break;

      case RELAY_OP:
        Serial.println("RELAY_OP");

        if (workingMode == MANUAL_MODE) {
          if (serialOP & RELAY_LED_BAND_BIT) {
            relayLightBand = !relayLightBand;
            if (relayLightBand) {
              turnOnRelay(RELAY_LED_BAND);
            } else {
              turnOffRelay(RELAY_LED_BAND);
            }
          }


          // Handling RELAY_WATER_1 with automatic turn-off after 3 seconds
          if (serialOP & RELAY_WATER_1_BIT) {
            if (!relayWater1Active) {  // Only turn on if it was previously off
              turnOnRelay(RELAY_WATER_1);
              relayWater1Timer = currentMillis;  // Reset the timer
              relayWater1Active = true;          // Mark the relay as active
            }
          }

          // Handling RELAY_WATER_2 with automatic turn-off after 3 seconds
          if (serialOP & RELAY_WATER_2_BIT) {
            if (!relayWater2Active) {  // Only turn on if it was previously off
              turnOnRelay(RELAY_WATER_2);
              relayWater2Timer = currentMillis;  // Reset the timer
              relayWater2Active = true;          // Mark the relay as active
            }
          }
          // Handling RELAY_WATER_3 with automatic turn-off after 3 seconds
          if (serialOP & RELAY_WATER_3_BIT) {
            if (!relayWater3Active) {  // Only turn on if it was previously off
              turnOnRelay(RELAY_WATER_3);
              relayWater3Timer = currentMillis;  // Reset the timer
              relayWater3Active = true;          // Mark the relay as active
            }
          }
          // Handling RELAY_WATER_4 with automatic turn-off after 3 seconds
          if (serialOP & RELAY_WATER_4_BIT) {
            if (!relayWater4Active) {  // Only turn on if it was previously off
              turnOnRelay(RELAY_WATER_4);
              relayWater4Timer = currentMillis;  // Reset the timer
              relayWater4Active = true;          // Mark the relay as active
            }
          }

          if (serialOP & RELAY6_BIT) {
            turnOnRelay(RELAY6);
          }
          if (!(serialOP & RELAY6_BIT)) {
            turnOffRelay(RELAY6);
          }
        }
        break;
    }
  }

  // periodically check: temp, moisture, light
  if (currentMillis - previousMillis >= WAITDELAY) {
    previousMillis = currentMillis;

    // soil moisture
    soil_hum1 = getSoilMoisture(SOIL_SENSOR1);
    soil_hum2 = getSoilMoisture(SOIL_SENSOR2);
    soil_hum3 = getSoilMoisture(SOIL_SENSOR3);
    soil_hum4 = getSoilMoisture(SOIL_SENSOR4);

    // temperature
    tempInterior = dhtInterior.getTemperature();
    tempExterior = dhtExterior.getTemperature();
    hum1 = dhtInterior.getHumidity();
    hum2 = dhtExterior.getHumidity();

    if (tempInterior > INTERIOR_TEMP_THRESHOLD && tempInterior > tempExterior) {
      if (workingMode == AUTO_MODE) {
        openServo(servo);
      } else {
        if (workingMode == AUTO_MODE) {
          closeServo(servo);
        }
      }
    }

    unsigned int noise = 0;
    if (relayWater1Active) {
      noise += 6;
    }
    if (relayWater2Active) {
      noise += 6;
    }
    if (relayWater3Active) {
      noise += 6;
    }
    if (relayWater4Active) {
      noise += 6;
    }
    if (relayLightBand) {
      noise += 6;
    }
    // light
    light = getLight(LIGHTSENSOR);
    Serial.println(light);
    if (light < LIGHT_THRESHOLD + noise) {
      if (workingMode == AUTO_MODE) {
        turnOnRelay(RELAY_LED_BAND);
      }
    } else {
      if (workingMode == AUTO_MODE) {
        turnOffRelay(RELAY_LED_BAND);
      }
    }

    //water
    if (soil_hum1 > WATER_THRESHOLD) {
      if (!relayWater1Active) {  // Only turn on if it was previously off
        turnOnRelay(RELAY_WATER_1);
        relayWater1Timer = currentMillis;  // Reset the timer
        relayWater1Active = true;          // Mark the relay as active
      }
    }
    if (soil_hum2 > WATER_THRESHOLD) {
      if (!relayWater2Active) {  // Only turn on if it was previously off
        turnOnRelay(RELAY_WATER_2);
        relayWater2Timer = currentMillis;  // Reset the timer
        relayWater2Active = true;          // Mark the relay as active
      }
    }
    if (soil_hum3 > WATER_THRESHOLD) {
      if (!relayWater3Active) {  // Only turn on if it was previously off
        turnOnRelay(RELAY_WATER_3);
        relayWater3Timer = currentMillis;  // Reset the timer
        relayWater3Active = true;          // Mark the relay as active
      }
    }
    if (soil_hum4 > WATER_THRESHOLD) {
      if (!relayWater4Active) {  // Only turn on if it was previously off
        turnOnRelay(RELAY_WATER_4);
        relayWater4Timer = currentMillis;  // Reset the timer
        relayWater4Active = true;          // Mark the relay as active
      }
    }


    bool workModeSend = false;
    if (workingMode == AUTO_MODE) {
      workModeSend = true;
    }

    // send data to ESP
    String parameters = "{\"sera_id\":1,\"hum_in\":" + String(hum1) + ",\"hum_out\":" + String(hum2) + ",\"temp_in\":" + String(tempInterior) + ",\"temp_out\":" + String(tempExterior) + ",\"light\":" + String(light) + ",\"soil_hum1\":" + String(soil_hum1) + ",\"soil_hum2\":" + String(soil_hum2) + ",\"soil_hum3\":" + String(soil_hum3) + ",\"soil_hum4\":" + String(soil_hum4) + ", \"auto_mode_manual_mode\":" + String(workModeSend) + "}";
    Serial.println(parameters);
    Serial1.println(parameters);
  }
  autoStopWaterPump(RELAY_WATER_1, relayWater1Timer, relayWater1Active);
  autoStopWaterPump(RELAY_WATER_2, relayWater2Timer, relayWater2Active);
  autoStopWaterPump(RELAY_WATER_3, relayWater3Timer, relayWater3Active);
  autoStopWaterPump(RELAY_WATER_4, relayWater4Timer, relayWater4Active);
}

bool isNumber(String str) {
  for (int i = 0; i < str.length(); i++) {
    if (!isDigit(str.charAt(i))) {
      return false;  // Dacă găsim un caracter care nu este cifră, returnăm false
    }
  }
  return true;  // Toate caracterele sunt cifre, returnăm true
}
