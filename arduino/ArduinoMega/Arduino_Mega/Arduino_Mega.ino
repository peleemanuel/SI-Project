#include "relay_functions.h"
#define RELAY1 52
#define RELAY2 53
#define RELAY3 50
#define RELAY4 51
#define RELAY5 48
#define RELAY6 49

#include "DHT22.h"
#define DHT1PIN 36
#define DHT2PIN 37
DHT22 dht22(DHT1PIN);
DHT22 dht222(DHT2PIN);

#include "light_sensor.h"
#define LIGHTSENSOR A2

#include "servo_functions.h"
#define SERVOPIN 8

#include "soil_moisture.h"
#define SOIL_SENSOR1 A8
#define SOIL_SENSOR2 A9
#define SOIL_SENSOR3 A10
#define SOIL_SENSOR4 A11

Servo servo;
float soil_hum1;
float soil_hum2;
float soil_hum3;
float soil_hum4;

float light;
float hum1;
float temp1;
float hum2;
float temp2;

void setup() {
  Serial.begin(9600);
  Serial.println("beginning");

  /* Setups */
  setupServo(servo, SERVOPIN);
  setupSoilMoisture(SOIL_SENSOR1);
  setupSoilMoisture(SOIL_SENSOR2);
  setupSoilMoisture(SOIL_SENSOR3);
  setupSoilMoisture(SOIL_SENSOR4);
  setupLightSensor(LIGHTSENSOR);
  setupRelay(RELAY1);
  setupRelay(RELAY2);
  setupRelay(RELAY3);
  setupRelay(RELAY4);
  setupRelay(RELAY5);
  setupRelay(RELAY6);
  turnOffRelay(RELAY1);
  turnOffRelay(RELAY2);
  turnOffRelay(RELAY3);
  turnOffRelay(RELAY4);
  turnOffRelay(RELAY5);
  turnOffRelay(RELAY6);
  setServoAngle(servo, 0);

  // delay(1000);
  // setServoAngle(servo, 180);
  // turnOnRelay(RELAY1);
  // turnOnRelay(RELAY2);

  // delay(2000);
  // setServoAngle(servo, 90);
  // turnOffRelay(RELAY1);
  // turnOffRelay(RELAY2);
}
void loop() {
  if (Serial.available() > 0) {
    // Citește caracterul primit
    String receivedString = Serial.readString();
    receivedString.trim();
    if (receivedString == "r1o") {
      turnOnRelay(RELAY1);
    }
    if (receivedString == "r2o") {
      turnOnRelay(RELAY2);
    }
    if (receivedString == "r3o") {
      turnOnRelay(RELAY3);
    }
    if (receivedString == "r4o") {
      turnOnRelay(RELAY4);
    }
    if (receivedString == "r5o") {
      turnOnRelay(RELAY5);
    }
    if (receivedString == "r6o") {
      turnOnRelay(RELAY6);
    }
    if (receivedString == "r1") {
      turnOffRelay(RELAY1);
    }
    if (receivedString == "r2") {
      turnOffRelay(RELAY2);
    }
    if (receivedString == "r3") {
      turnOffRelay(RELAY3);
    }
    if (receivedString == "r4") {
      turnOffRelay(RELAY4);
    }
    if (receivedString == "r5") {
      turnOffRelay(RELAY5);
    }
    if (receivedString == "r6") {
      turnOffRelay(RELAY6);
    }
    // int angle = receivedString.toInt();
    // Serial.println(angle);
    // servo.write(angle);
  }

  soil_hum1 = getSoilMoisture(SOIL_SENSOR1);
  soil_hum2 = getSoilMoisture(SOIL_SENSOR2);
  soil_hum3 = getSoilMoisture(SOIL_SENSOR3);
  soil_hum4 = getSoilMoisture(SOIL_SENSOR4);

  Serial.print("Soil humidity 1: ");
  Serial.println(soil_hum1);
  Serial.print("Soil humidity 2: ");
  Serial.println(soil_hum2);
  Serial.print("Soil humidity 3: ");
  Serial.println(soil_hum3);
  Serial.print("Soil humidity 4: ");
  Serial.println(soil_hum4);
  Serial.println();

  light = getLight(LIGHTSENSOR);
  Serial.print("Light: ");
  Serial.println(light);
  Serial.println();
  //delay(1000);
  // // if(light < 20) {
  // //   // turnOnRelay(RELAY1);
  // // } else {
  // //   // turnOffRelay(RELAY1);
  // // }
  // if (contor++ < 100) {
  //   turnOnRelay(RELAY1);
  // } else {
  //   turnOffRelay(RELAY1);
  // }
  // Serial.print("contor: ");
  // Serial.println(contor);
  // if (contor > 200)
  //   contor = 0;
  // delay(2000);

  temp1 = dht22.getTemperature();
  hum1 = dht22.getHumidity();
  Serial.print("Temp: ");
  Serial.println(temp1, 2);
  Serial.print("Air hum: ");
  Serial.println(hum1, 2);
  Serial.println();
  temp2 = dht222.getTemperature();
  hum2 = dht222.getHumidity();
  Serial.print("Temp: ");
  Serial.println(temp2, 2);
  Serial.print("Air hum: ");
  Serial.println(hum2, 2);
  Serial.println();
  delay(2500);
}
// // void loop() {
// //   delay(2000);
// //   temp = dht22.getTemperature();
// //   hum = dht22.getHumidity();
// //   Serial.print("temp: ");
// //   Serial.println(temp,2);
// //   Serial.print("hum: ");
// //   Serial.println(hum,2);
// //   // delay(500);
// //   // Serial.println(analogRead(A1));
// //   // Serial.println(getLight(LIGHTSENSOR));
// //   // if (Serial.available() > 0) {
// //   //   // Citește caracterul primit
// //   //   String receivedString = Serial.readString();
// //   //   receivedString.trim();

// //   //   int angle = receivedString.toInt();
// //   //   Serial.println(angle);
// //   //   servo.write(angle);
// //   // }
// // }


// #include <Servo.h>

// // Creează un obiect Servo pentru a controla servo motorul
// Servo myServo;

// // Definește pinii pentru butoane
// const int buttonPin1 = 40;  // Butonul pentru mișcare la stânga
// const int buttonPin2 = 38;  // Butonul pentru mișcare la dreapta

// // Unghiuri pentru pozițiile servo
// int angleLeft = 180;  // Unghiul pentru stânga
// int angleRight = 0;   // Unghiul pentru dreapta
// int angle = 90;
// void setup() {
//   // Inițializează comunicația serială
//   Serial.begin(9600);
//   myServo.write(angle);
//   // Atașează servo motorul la pinul PWM
//   myServo.attach(8);

//   // Inițializează pinii butoanelor ca intrări
//   pinMode(buttonPin1, INPUT);
//   pinMode(buttonPin2, INPUT);
// }

// void loop() {
//   // Verifică dacă butonul 1 este apăsat
//   if (digitalRead(buttonPin1) == HIGH) {
//     // Muta servo la unghiul pentru stânga
//     if (angle <= 170)
//       angle += 10;
//     myServo.write(angle);
//     Serial.println("Servo se mișcă la stânga");
//     delay(50);  // Mică întârziere pentru a evita citiri multiple
//   }

//   // Verifică dacă butonul 2 este apăsat
//   if (digitalRead(buttonPin2) == HIGH) {
//     // Muta servo la unghiul pentru dreapta
//     if (angle >= 10)
//       angle -= 10;
//     myServo.write(angle);
//     Serial.println("Servo se mișcă la dreapta");
//     delay(50);  // Mică întârziere pentru a evita citiri multiple
//   }
// }
