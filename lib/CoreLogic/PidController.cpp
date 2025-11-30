#include "PidController.h"

std::array<float, 4> PidController::compute(const std::array<float, 4> &setpoints, const std::array<float, 4> &measurements)
{
    uint32_t currentTime = timer.getMillis();
    float dt = (currentTime - prevTime) / 1000.0f; // Sekundy
    prevTime = currentTime;

    std::array<float, 4> outputs = {0};
    for (int i = 0; i < 4; ++i)
    {
        float error = setpoints[i] - measurements[i];
        integrals[i] += error * dt;
        float derivative = (error - prevErrors[i]) / dt;
        outputs[i] = gains[i].kp * error + gains[i].ki * integrals[i] + gains[i].kd * derivative;
        prevErrors[i] = error;
    }
    return outputs;
}