// lib/CoreLogic/PidController.h
#pragma once
#include "IPidTimer.h"
#include <array>

struct PidGains
{
    float kp, ki, kd;
};

class PidController
{
    IPidTimer &timer;              // DI: Wstrzyknięcie timera
    std::array<PidGains, 4> gains; // Dla throttle, yaw, pitch, roll
    std::array<float, 4> prevErrors = {0};
    std::array<float, 4> integrals = {0};
    uint32_t prevTime = 0;

public:
    PidController(IPidTimer &t, const std::array<PidGains, 4> &g) : timer(t), gains(g) {}
    std::array<float, 4> compute(const std::array<float, 4> &setpoints, const std::array<float, 4> &measurements);
    // throttle, yaw, pitch, roll
};
