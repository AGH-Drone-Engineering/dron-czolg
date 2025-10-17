#pragma once
#include "pids.h"

class Pids3d
{
public:
    Pids roll, pitch, yaw;

    Pids3d(float Kp_, float Ki_, float Kd_,
           float out_min_, float out_max_)
        : roll(Kp_, Ki_, Kd_, out_min_, out_max_),
          pitch(Kp_, Ki_, Kd_, out_min_, out_max_),
          yaw(Kp_, Ki_, Kd_, out_min_, out_max_)
    {
    }

    void reset()
    {
        roll.reset();
        pitch.reset();
        yaw.reset();
    }

    void init_all()
    {
        roll.init();
        pitch.init();
        yaw.init();
    }
};