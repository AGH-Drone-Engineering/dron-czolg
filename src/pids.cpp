#include "pids.h"

Pids::Pids(float Kp_, float Ki_, float Kd_, float out_min_, float out_max_)
    : Kp(Kp_), Ki(Ki_), Kd(Kd_),
      prev_err(0.0f), integral(0.0f),
      out_min(out_min_), out_max(out_max_)
{
}

void Pids::reset()
{
    prev_err = 0.0f;
    integral = 0.0f;
}

void Pids::init()
{
    Kp = 1.0f;
    Ki = 0.01f;
    Kd = 1.0f;
    out_min = -100.0f;
    out_max = 100.0f;
    reset();
}

float Pids::compute(float meas_, float sp_, float dt_)
{
    float err = sp_ - meas_;
    integral += err * dt_;

    if (integral > out_max)
        integral = out_max;
    if (integral < out_min)
        integral = out_min;

    float deriv = (err - prev_err) / dt_;
    float out = Kp * err + Ki * integral + Kd * deriv;

    if (out > out_max)
    {
        prev_err = err;
        return out_max;
    }
    if (out < out_min)
    {
        prev_err = err;
        return out_min;
    }

    prev_err = err;
    return out;
}

void Pids::setGains(float Kp_, float Ki_, float Kd_)
{
    this->Kp = Kp_;
    this->Ki = Ki_;
    this->Kd = Kd_;
}

void Pids::setOutputLimits(float out_min_, float out_max_)
{
    this->out_min = out_min_;
    this->out_max = out_max_;
}