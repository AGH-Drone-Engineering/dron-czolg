#pragma once

class Pids
{
public:
    Pids(float Kp_ = 0.0f, float Ki_ = 0.0f, float Kd_ = 0.0f,
         float out_min_ = -1.0f, float out_max_ = 1.0f);

    void reset();

    float compute(float meas_, float sp_, float dt_);

    void setGains(float Kp_, float Ki_, float Kd_);
    void setOutputLimits(float out_min_, float out_max_);
    void init();

private:
    float Kp, Ki, Kd;
    float prev_err;
    float integral;
    float out_min, out_max;
};