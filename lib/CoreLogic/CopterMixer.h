// lib/CoreLogic/CopterMixer.h
#pragma once
#include "IMixer.h"

/**
 * @brief Mikser dla trybu quadcoptera (konfiguracja X)
 * 
 * Przelicza throttle, yaw, pitch, roll na prędkości 4 silników.
 * Silniki czołgu są wyłączone.
 * 
 * Konfiguracja silników (widok z góry):
 *     FL (CCW)    FR (CW)
 *         \      /
 *          \    /
 *           ----
 *          /    \
 *         /      \
 *     BL (CW)    BR (CCW)
 */
class CopterMixer : public IMixer
{
public:
    void mix(const MixerInput &input, MotorOutputs &output) override
    {
        // Mikser quadcoptera w konfiguracji X
        // FL: +pitch, -roll, +yaw (CCW)
        // FR: +pitch, +roll, -yaw (CW)
        // BL: -pitch, -roll, -yaw (CW)
        // BR: -pitch, +roll, +yaw (CCW)

        output.fl = input.throttle + input.yaw - input.roll + input.pitch;
        output.fr = input.throttle - input.yaw + input.roll + input.pitch;
        output.bl = input.throttle - input.yaw - input.roll - input.pitch;
        output.br = input.throttle + input.yaw + input.roll - input.pitch;

        // Silniki czołgu wyłączone w trybie copter
        output.tl = 0.0f;
        output.tr = 0.0f;
    }
};
