#pragma once

typedef struct {
    float fl, fr, bl, br;  // front-left, ...
} motors_pwm_s;

void setMotorPWM(int pin, int value);

void setVehiclePWM(const motors_pwm_s* motors_pwm);
