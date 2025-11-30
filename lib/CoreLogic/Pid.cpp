// lib/CoreLogic/Pid.cpp
#include "Pid.h"

Pid::Pid(float kp, float ki, float kd, float outMin, float outMax)
{
    gains.kp = kp;
    gains.ki = ki;
    gains.kd = kd;
    gains.outMin = outMin;
    gains.outMax = outMax;
}

Pid::Pid(const PidGains &g) : gains(g) {}

float Pid::compute(float measurement, float setpoint, float dt)
{
    if (dt <= 0.0f)
        return 0.0f;

    // Oblicz błąd
    float error = setpoint - measurement;

    // Część całkująca z anti-windup
    integral += error * dt;
    if (integral > gains.outMax)
        integral = gains.outMax;
    if (integral < gains.outMin)
        integral = gains.outMin;

    // Część różniczkująca
    float derivative = (error - prevError) / dt;

    // Oblicz wyjście PID
    float output = gains.kp * error + gains.ki * integral + gains.kd * derivative;

    // Zapisz błąd dla następnej iteracji
    prevError = error;

    // Ogranicz wyjście do limitów
    if (output > gains.outMax)
        return gains.outMax;
    if (output < gains.outMin)
        return gains.outMin;

    return output;
}

void Pid::reset()
{
    prevError = 0.0f;
    integral = 0.0f;
}

void Pid::setGains(float kp, float ki, float kd)
{
    gains.kp = kp;
    gains.ki = ki;
    gains.kd = kd;
}

void Pid::setGains(const PidGains &g)
{
    gains = g;
}

void Pid::setOutputLimits(float outMin, float outMax)
{
    gains.outMin = outMin;
    gains.outMax = outMax;
}
