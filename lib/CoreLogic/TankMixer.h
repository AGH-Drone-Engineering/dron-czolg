// lib/CoreLogic/TankMixer.h
#pragma once
#include "IMixer.h"

/**
 * @brief Mikser dla trybu czołgu (sterowanie różnicowe)
 * 
 * Przelicza throttle i yaw na prędkości lewej i prawej gąsienicy.
 * Silniki copter są wyłączone.
 */
class TankMixer : public IMixer
{
public:
    void mix(const MixerInput &input, MotorOutputs &output) override
    {
        // Sterowanie różnicowe dla gąsienic
        // yaw dodatni = skręt w prawo = lewa gąsienica szybciej
        output.tl = input.throttle + input.yaw;
        output.tr = input.throttle - input.yaw;

        // Silniki copter wyłączone w trybie czołgu
        output.fl = 0.0f;
        output.fr = 0.0f;
        output.bl = 0.0f;
        output.br = 0.0f;
    }
};
