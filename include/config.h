#pragma once

// Piny
// tank motor 1 pin 14, tank motor 2 pin 17
// servo 1 pin 22, servo 2 pin 23
// sbus pin 20

#define SERVO_LEFT_PIN 22
#define SERVO_RIGHT_PIN 23

// SBUS

#define SBUS_INPUT Serial5

// porty motorów drone
#define MOTOR_PORT_DRONE_FL &Serial1
#define MOTOR_PORT_DRONE_FR &Serial2
#define MOTOR_PORT_DRONE_BL &Serial3
#define MOTOR_PORT_DRONE_BR &Serial4

// Porty motorów tank
#define MOTOR_PORT_TL &Serial5
#define MOTOR_PORT_TR &Serial6

// SBUS
#define SBUS_MIN 175.0f
#define SBUS_MAX 1811.0f
#define SBUS_CENTER ((SBUS_MIN + SBUS_MAX) / 2.0f)

// DShot
#define DSHOT_THROTTLE_ACTIVE_MIN 48.0f
#define DSHOT_THROTTLE_ACTIVE_MAX 2047.0f

// Tryby - tylko dla serw
typedef enum
{
    MODE_TANK,
    MODE_COPTER
} vehicle_mode_t;