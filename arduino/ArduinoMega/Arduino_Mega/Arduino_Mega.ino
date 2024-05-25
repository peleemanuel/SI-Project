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

float light;
float hum1;
float tempInterior;
float hum2;
float tempExterior;

bool workingMode;

void setup() {
  Serial.begin(115200);
  Serial.println("beginning");

  Serial1.begin(115200); // Serial1 for TX1 and RX1 pins

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
  if (Serial.available() > 0) {
    int serialOP = Serial.parseInt();  // make sure this is Serial1
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
            turnOnRelay(RELAY_LED_BAND);
          }
          if (!(serialOP & RELAY_LED_BAND_BIT)) {
            turnOffRelay(RELAY_LED_BAND);
          }

          if (serialOP & RELAY_WATER_1_BIT) {
            turnOnRelay(RELAY_WATER_1);
          }
          if (!(serialOP & RELAY_WATER_1_BIT)) {
            turnOffRelay(RELAY_WATER_1);
          }

          if (serialOP & RELAY_WATER_2_BIT) {
            turnOnRelay(RELAY_WATER_2);
          }
          if (!(serialOP & RELAY_WATER_2_BIT)) {
            turnOffRelay(RELAY_WATER_2);
          }

          if (serialOP & RELAY_WATER_3_BIT) {
            turnOnRelay(RELAY_WATER_3);
          }
          if (!(serialOP & RELAY_WATER_3_BIT)) {
            turnOffRelay(RELAY_WATER_3);
          }

          if (serialOP & RELAY_WATER_4_BIT) {
            turnOnRelay(RELAY_WATER_4);
          }
          if (!(serialOP & RELAY_WATER_4_BIT)) {
            turnOffRelay(RELAY_WATER_4);
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
      }
    } else {
      closeServo(servo);
    }

    // light
    light = getLight(LIGHTSENSOR);
    if (light < LIGHT_THRESHOLD) {
      if (workingMode == AUTO_MODE) {
        turnOnRelay(RELAY_LED_BAND);
      }
    } else {
      turnOffRelay(RELAY_LED_BAND);
    }

    // send data to ESP
    String parameters = "{\"id\":\"SeraTest\",\"humidity1\":" + String(hum1) + ",\"humidity2\":" + String(hum2) + ",\"temperatureInterior\":" + String(tempInterior) + ",\"temperatureExterior\":" + String(tempExterior) + ",\"light\":" + String(light) + ",\"soil_moisture_plant_1\":" + String(soil_hum1) + ",\"soil_moisture_plant_2\":" + String(soil_hum1) + ",\"soil_moisture_plant_3\":" + String(soil_hum1) + ",\"soil_moisture_plant_4\":" + String(soil_hum1) + "}";
    Serial1.println(parameters);
  }
}
