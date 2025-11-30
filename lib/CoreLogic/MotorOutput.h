// lib/CoreLogic/MotorOutput.h
#pragma once
#include "IMotorDriver.h"
#include "MotorOutputs.h"
#include "config.h"
#include "Utils.h"
#include <array>

// Indeksy silników w tablicy
enum MotorIndex
{
    MOTOR_FL = 0,
    MOTOR_FR = 1,
    MOTOR_BL = 2,
    MOTOR_BR = 3,
    MOTOR_TL = 4,
    MOTOR_TR = 5,
    MOTOR_COUNT = 6
};

/**
 * @brief Warstwa wyjściowa dla silników
 * 
 * Odpowiada za:
 * - Konwersję wartości na format DShot
 * - Wysyłanie do ESC
 * - Reset silników
 */
class MotorOutput
{
    std::array<IMotorDriver *, MOTOR_COUNT> motors;

    /** @brief Konwertuj throttle na wartość DShot */
    static uint16_t convertToDShot(float throttle)
    {
        float dshot = map(throttle, SBUS_MIN, SBUS_MAX,
                          DSHOT_THROTTLE_ACTIVE_MIN, DSHOT_THROTTLE_ACTIVE_MAX);
        return static_cast<uint16_t>(
            constrain(dshot, DSHOT_THROTTLE_ACTIVE_MIN, DSHOT_THROTTLE_ACTIVE_MAX));
    }

public:
    MotorOutput(std::array<IMotorDriver *, MOTOR_COUNT> &m) : motors(m) {}

    /** @brief Wyślij wartości do ESC */
    void send(const MotorOutputs &outputs)
    {
        if (motors[MOTOR_FL])
            motors[MOTOR_FL]->sendThrottle(convertToDShot(outputs.fl), false);
        if (motors[MOTOR_FR])
            motors[MOTOR_FR]->sendThrottle(convertToDShot(outputs.fr), false);
        if (motors[MOTOR_BL])
            motors[MOTOR_BL]->sendThrottle(convertToDShot(outputs.bl), false);
        if (motors[MOTOR_BR])
            motors[MOTOR_BR]->sendThrottle(convertToDShot(outputs.br), false);
        if (motors[MOTOR_TL])
            motors[MOTOR_TL]->sendThrottle(convertToDShot(outputs.tl), false);
        if (motors[MOTOR_TR])
            motors[MOTOR_TR]->sendThrottle(convertToDShot(outputs.tr), false);
    }

    /** @brief Wyślij zero do wszystkich silników */
    void reset()
    {
        for (auto *motor : motors)
        {
            if (motor)
                motor->sendThrottle(0, false);
        }
    }

    /** @brief Wyślij minimalny throttle (do uzbrojenia ESC) */
    void sendMinThrottle()
    {
        uint16_t minThrottle = static_cast<uint16_t>(DSHOT_THROTTLE_ACTIVE_MIN);
        for (auto *motor : motors)
        {
            if (motor)
                motor->sendThrottle(minThrottle, false);
        }
    }
};
