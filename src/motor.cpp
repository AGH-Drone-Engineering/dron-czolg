#include <main.h>
#include <motor.h>

#include "DShot.h"

#define MOTOR_SERIAL_FL &Serial1
#define MOTOR_SERIAL_FR &Serial2
#define MOTOR_SERIAL_BL &Serial3
#define MOTOR_SERIAL_BR &Serial4
#define MOTOR_SERIAL_TL &Serial5
#define MOTOR_SERIAL_TR &Serial6

DShot motor0(MOTOR_SERIAL_FL, DShotType::DShot600);
DShot motor1(MOTOR_SERIAL_FR, DShotType::DShot600);
DShot motor2(MOTOR_SERIAL_BL, DShotType::DShot600);
DShot motor3(MOTOR_SERIAL_BR, DShotType::DShot600);
DShot motor4(MOTOR_SERIAL_TL, DShotType::DShot600);
DShot motor5(MOTOR_SERIAL_TR, DShotType::DShot600);

int ClampMotorValue(DShot& motor, int value) {
    // Clamp value to allowed range
    if (value < ESC_INPUT_MIN) value = ESC_INPUT_MIN;
    if (value > ESC_INPUT_MAX) value = ESC_INPUT_MAX;
    return value;
}

void resetMotors() {
    motor0.sendThrottle(0);
    motor1.sendThrottle(0);
    motor2.sendThrottle(0);
    motor3.sendThrottle(0);
    motor4.sendThrottle(0);
    motor5.sendThrottle(0);
}

void ClampMotorValues(motors_pwm_s& motors_pwm, vehicle_mode_t mode) {
    if (mode == MODE_COPTER) {
        motors_pwm.fl = ClampMotorValue(motor0, static_cast<int>(motors_pwm.fl));
        motors_pwm.fr = ClampMotorValue(motor1, static_cast<int>(motors_pwm.fr));
        motors_pwm.bl = ClampMotorValue(motor2, static_cast<int>(motors_pwm.bl));
        motors_pwm.br = ClampMotorValue(motor3, static_cast<int>(motors_pwm.br));
        // Tank motors off
        motors_pwm.tl = 0;
        motors_pwm.tr = 0;
    } else {
        // Copter motors off
        motors_pwm.fl = 0;
        motors_pwm.fr = 0;
        motors_pwm.bl = 0;
        motors_pwm.br = 0;
        // Tank motors on
        motors_pwm.tl = ClampMotorValue(motor4, static_cast<int>(motors_pwm.tl));
        motors_pwm.tr = ClampMotorValue(motor5, static_cast<int>(motors_pwm.tr));
    }
}

void setVehiclePWM(const motors_pwm_s* motors_pwm, vehicle_mode_t mode) {
    if (mode == MODE_COPTER) {
        motor0.sendThrottle(static_cast<int>(motors_pwm->fl) * (DSHOT_MAX_THROTTLE / ESC_INPUT_MAX));
        motor1.sendThrottle(static_cast<int>(motors_pwm->fr) * (DSHOT_MAX_THROTTLE / ESC_INPUT_MAX));
        motor2.sendThrottle(static_cast<int>(motors_pwm->bl) * (DSHOT_MAX_THROTTLE / ESC_INPUT_MAX));
        motor3.sendThrottle(static_cast<int>(motors_pwm->br) * (DSHOT_MAX_THROTTLE / ESC_INPUT_MAX));
    } else {
        motor4.sendThrottle(static_cast<int>(motors_pwm->tl) * (DSHOT_MAX_THROTTLE / ESC_INPUT_MAX));
        motor5.sendThrottle(static_cast<int>(motors_pwm->tr) * (DSHOT_MAX_THROTTLE / ESC_INPUT_MAX));
    }
}