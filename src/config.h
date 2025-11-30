#pragma once

// ============ HARDWARE PINS ============
// E1 pin 1
// E2 pin 8
// E3 pin 29
// E4 pin 24

// tank motor 1 pin 14
// tank motor 2 pin 17

// servo 1 pin 22
// servo 2 pin 23

// sbus pin 20

// Servo pins (update according to the wiring)
#define SERVO_LEFT_PIN 22
#define SERVO_RIGHT_PIN 23

// Servo angles for modes
#define SERVO_ANGLE_COPTER 20   // Angle for copter/drone mode
#define SERVO_ANGLE_TANK 180    // Angle for tank mode

// ============ LOOP TIMING ============
#define LOOP_DT 20              // Main loop period [ms]
#define DEBUG_INTERVAL 500      // Debug output interval [ms]
#define SERIAL_TIMEOUT 3000     // Serial init timeout [ms]

// ============ I2C / IMU ============
#define I2C_CLOCK_SPEED 400000  // I2C clock speed [Hz] (400kHz fast mode)

// IMU complementary filter coefficient (0.9-0.99)
// Higher = faster response, more gyro trust
// Lower = more stable, more accelerometer trust
#define IMU_FILTER_ALPHA 0.98f

// IMU sensor scales (for MPU6050)
#define IMU_ACCEL_SCALE 4096.0f // LSB/g for ±8g range
#define IMU_GYRO_SCALE 65.5f    // LSB/(°/s) for ±500°/s range

// ============ ESC / MOTOR ============
#define ESC_INPUT_MIN 0
#define ESC_INPUT_MAX 255

// ============ RC COEFFICIENTS ( TANK ) ============
#define THROTTLE_COEF_TANK 1.0f
#define STEER_COEF 1.0f

// ============ RC COEFFICIENTS ( COPTER ) ============
#define THROTTLE_COEF_COPTER 1.0f
#define ROLL_COEF 1.0f
#define PITCH_COEF 1.0f
#define YAW_RATE_COEF 1.0f

// ============ PID GAINS ============
// Inner loop (rate PIDs) - react to angular velocity
#define PID_YAW_RATE_KP 1.0f
#define PID_YAW_RATE_KI 0.01f
#define PID_YAW_RATE_KD 0.1f
#define PID_YAW_RATE_MIN -100.0f
#define PID_YAW_RATE_MAX 100.0f

#define PID_PITCH_RATE_KP 1.0f
#define PID_PITCH_RATE_KI 0.01f
#define PID_PITCH_RATE_KD 0.1f
#define PID_PITCH_RATE_MIN -100.0f
#define PID_PITCH_RATE_MAX 100.0f

#define PID_ROLL_RATE_KP 1.0f
#define PID_ROLL_RATE_KI 0.01f
#define PID_ROLL_RATE_KD 0.1f
#define PID_ROLL_RATE_MIN -100.0f
#define PID_ROLL_RATE_MAX 100.0f

// Outer loop (angle PIDs) - convert angle error to rate setpoint
#define PID_PITCH_ANGLE_KP 2.0f
#define PID_PITCH_ANGLE_KI 0.0f
#define PID_PITCH_ANGLE_KD 0.0f
#define PID_PITCH_ANGLE_MIN -100.0f
#define PID_PITCH_ANGLE_MAX 100.0f

#define PID_ROLL_ANGLE_KP 2.0f
#define PID_ROLL_ANGLE_KI 0.0f
#define PID_ROLL_ANGLE_KD 0.0f
#define PID_ROLL_ANGLE_MIN -100.0f
#define PID_ROLL_ANGLE_MAX 100.0f

// ============ SBUS ============
#define SBUS_MIN 175.0f
#define SBUS_MAX 1811.0f

#ifdef NATIVE_TESTING
#define SBUS_INPUT dummy
#else
#define SBUS_INPUT Serial5
#endif

// ============ MOTOR PORTS ============
// E1 pin 1, E2 pin 8, E3 pin 29, E4 pin 24
// tank motor 1 pin 14, tank motor 2 pin 17
// (those serials are wrong, correct pins are above - TODO: fix)
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

// ============ DSHOT ============
#define DSHOT_THROTTLE_ACTIVE_MIN 48.0f
#define DSHOT_THROTTLE_ACTIVE_MAX 2047.0f

// ============ MODE / TRANSITION ============
#define DEFAULT_MODE MODE_TANK
#define TRANSITION_TIME 3000            // Mode transition duration [ms]
#define SWITCH_MOTOR_PWM_THRESHOLD 100

// ============ SAFETY ============
#define SAFETY_LAND_REDUCTION_STEP 50.0f  // Throttle reduction per cycle during safety land

// ============ VEHICLE MODE ENUM ============
typedef enum
{
    MODE_TANK,
    MODE_COPTER,
    MODE_TRANSITION
} vehicle_mode_t;