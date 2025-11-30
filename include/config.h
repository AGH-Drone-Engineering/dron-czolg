#pragma once

// E1 pin 1
// E2 pin 8
// E3 pin 29
// E4 pin 24

// tank motor 1 pin 14
// tank motor 2 pin 17

// servo 1 pin 22
// servo 2 pin 23

// sbus pin 20

// pins
// (update according to the wiring)
#define SERVO_LEFT_PIN 22
#define SERVO_RIGHT_PIN 23

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
#ifdef NATIVE_TESTING
#define SBUS_INPUT dummy
#else
#define SBUS_INPUT Serial5
#endif
// E1 pin 1
// E2 pin 8
// E3 pin 29
// E4 pin 24

// tank motor 1 pin 14
// tank motor 2 pin 17

// servo 1 pin 22
// servo 2 pin 23

// those serials are wrong, correct pins are above
// Ports for motors
#ifdef NATIVE_TESTING
#define MOTOR_PORT_FL nullptr
#define MOTOR_PORT_FR nullptr
#define MOTOR_PORT_BL nullptr
#define MOTOR_PORT_BR nullptr
#define MOTOR_PORT_TL nullptr
#define MOTOR_PORT_TR nullptr
#else
#define MOTOR_PORT_FL &Serial1
#define MOTOR_PORT_FR &Serial2
#define MOTOR_PORT_BL &Serial3
#define MOTOR_PORT_BR &Serial4
#define MOTOR_PORT_TL &Serial5
#define MOTOR_PORT_TR &Serial6
#endif

#define DEFAULT_MODE MODE_TANK

#define SWITCH_MOTOR_PWM_THRESHOLD 100

#define SBUS_MIN 175.0f
#define SBUS_MAX 1811.0f

#define DSHOT_THROTTLE_ACTIVE_MIN 48.0f
#define DSHOT_THROTTLE_ACTIVE_MAX 2047.0f

#define SAFETY_LAND_REDUCTION_STEP 50 // adjust as needed for smooth safety landing

#define TRANSITION_TIME 3000 // milliseconds

typedef enum
{
    MODE_TANK,
    MODE_COPTER,
    MODE_TRANSITION
} vehicle_mode_t;