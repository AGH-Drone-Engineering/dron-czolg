// lib/CoreLogic/IMixer.h
#pragma once
#include "MotorOutputs.h"

/**
 * @brief Dane wejściowe dla miksera
 * 
 * Zawiera setpointy sterowania i wyjścia PID.
 */
struct MixerInput
{
    float throttle = 0.0f;
    float yaw = 0.0f;
    float pitch = 0.0f;
    float roll = 0.0f;
};

/**
 * @brief Interfejs miksera silników
 * 
 * Mikser przelicza wejścia sterowania (throttle, yaw, pitch, roll)
 * na wartości dla poszczególnych silników.
 */
class IMixer
{
public:
    virtual ~IMixer() = default;

    /**
     * @brief Oblicz wyjścia silników na podstawie wejść sterowania
     * @param input Wejścia sterowania (throttle, yaw, pitch, roll)
     * @param output Struktura do wypełnienia wartościami silników
     */
    virtual void mix(const MixerInput &input, MotorOutputs &output) = 0;
};
