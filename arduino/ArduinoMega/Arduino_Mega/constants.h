#ifndef CONSTANTS_DEFINE
#define CONSTANTS_DEFINE

#define WAITDELAY 10000  // time in milliseconds
#define WATER_PUMP_DELAY 3000 // time in mill, plant watering time
// working mode
#define MANUAL_MODE 0
#define AUTO_MODE 1

// command register
#define NO_OP 0
#define MODE_OP 1
#define SERVO_OP 2
#define RELAY_OP 3

// actual bits
#define MODE_BIT 1
#define SERVO_BIT 1 // 1 = open | 0 = close
#define RELAY_LED_BAND_BIT 1
#define RELAY_WATER_1_BIT 2
#define RELAY_WATER_2_BIT 4
#define RELAY_WATER_3_BIT 8
#define RELAY_WATER_4_BIT 16
#define RELAY6_BIT 32

// servo angles
#define SERVO_OPEN 0
#define SERVO_CLOSE 69

// interior temperature threshold
#define INTERIOR_TEMP_THRESHOLD 15.0f

// light threshold for led band
#define LIGHT_THRESHOLD 50



#endif