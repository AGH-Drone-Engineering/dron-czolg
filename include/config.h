#pragma once

// pins
// (update according to the wiring)
#define SERVO_LEFT_PIN 9
#define SERVO_RIGHT_PIN 10

// LOOP
#define LOOP_DT 20

// ESC
#define ESC_INPUT_MIN 0
#define ESC_INPUT_MAX 255

// RC ( TANK )
#define THROTTLE_COEF_TANK 1
#define STEER_COEF 1

// RC ( COPTER )
#define THROTTLE_COEF_COPTER 1
#define ROLL_COEF 1
#define PITCH_COEF 1
#define YAW_RATE_COEF 1

// SBUS Channel mapping
#define SBUS_INPUT Serial1
#define RC_THROTTLE_CH 0
#define RC_STEER_CH 1
#define RC_PITCH_CH 2
#define RC_ROLL_CH 3
#define RC_MODE_CH 4 // Switch between tank and copter mode

// Ports for motors
#define MOTOR_PORT_FL &Serial1
#define MOTOR_PORT_FR &Serial2
#define MOTOR_PORT_BL &Serial3
#define MOTOR_PORT_BR &Serial4
#define MOTOR_PORT_TL &Serial5
#define MOTOR_PORT_TR &Serial6

#define DEFAULT_MODE MODE_TANK

#define SWITCH_MOTOR_PWM_THRESHOLD 0

#define DSHOT_THROTTLE_ACTIVE_MIN 50

typedef enum
{
    MODE_TANK = 0,
    MODE_COPTER
} vehicle_mode_t;