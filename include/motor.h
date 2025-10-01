#pragma once

typedef struct {
    float fl, fr, bl, br, tl, tr;  // front-left, ..., tank-left, tank-right
} motors_pwm_s;

void setMotorPWM(int pin, int value);

void setVehiclePWM(const motors_pwm_s* motors_pwm);
