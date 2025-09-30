#pragma once

// LOOP
#define LOOP_DT 20

// ESC
#define ESC_INPUT_MIN 0
#define ESC_INPUT_MAX 255

// RC (TANK)
#define THROTTLE_COEF_TANK 1
#define STEER_COEF 1

// RC (COPTER)
#define THROTTLE_COEF_COPTER 1
#define ROLL_COEF 1
#define PITCH_COEF 1
#define YAW_RATE_COEF 1

// SBUS channel mapping
#define RC_THROTTLE_CH 0
#define RC_STEER_CH 1
#define RC_PITCH_CH 2
#define RC_ROLL_CH 3
#define RC_MODE_CH 4 // Switch between tank and copter

typedef enum { MODE_TANK = 0, MODE_COPTER } vehicle_mode_t;
