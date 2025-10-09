#pragma once

#include <Arduino.h>

// we would use 1 esc for 4 drone engines and 2 other controllers for tank tracks
// engines numbering should be:
//     front left (FL) - 0
//     front right (FR) - 1
//     back left (BL) - 2
//     back right (BR) - 3
//     tank left (TL) - 4
//     tank right (TR) - 5


#define ESC_INPUT_MIN 0
#define ESC_INPUT_MAX 255

typedef struct {
    float fl, fr, bl, br, tl, tr;  // front-left, ..., tank-left, tank-right
} motors_pwm_s;

void setMotorPWM(int pin, int value);

void setVehiclePWM(const motors_pwm_s* motors_pwm);
