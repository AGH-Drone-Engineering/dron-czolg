// lib/CoreLogic/StabilizationController.h
#pragma once
#include "Pid.h"
#include "ImuLogic.h"
#include "IMixer.h"
#include "config.h"

/**
 * @brief Kontroler stabilizacji - oblicza PID dla wszystkich osi
 *
 * Implementuje kaskadowy PID:
 * - Zewnętrzna pętla: angle -> rate (pitch, roll)
 * - Wewnętrzna pętla: rate -> output (pitch, roll, yaw)
 */
class StabilizationController
{
    // Wewnętrzna pętla (rate)
    Pid yawRatePid;
    Pid pitchRatePid;
    Pid rollRatePid;

    // Zewnętrzna pętla (angle)
    Pid pitchAnglePid;
    Pid rollAnglePid;

    // Wartości pośrednie (dla debugowania)
    float rollDesired = 0.0f;
    float pitchDesired = 0.0f;

public:
    StabilizationController() = default;

    /** @brief Ustaw wzmocnienia dla wewnętrznej pętli (rate) */
    void setInnerGains(const PidGains &yaw, const PidGains &pitch, const PidGains &roll)
    {
        yawRatePid.setGains(yaw);
        pitchRatePid.setGains(pitch);
        rollRatePid.setGains(roll);
    }

    /** @brief Ustaw wzmocnienia dla zewnętrznej pętli (angle) */
    void setOuterGains(const PidGains &pitch, const PidGains &roll)
    {
        pitchAnglePid.setGains(pitch);
        rollAnglePid.setGains(roll);
    }

    /**
     * @brief Oblicz wyjścia PID dla trybu copter (kaskadowy PID)
     * @param imu Dane z IMU
     * @param setpoints Setpointy: throttle, yaw_rate, pitch_angle, roll_angle
     * @param dt Czas od ostatniego wywołania
     * @return MixerInput z wartościami do miksera
     */
    MixerInput computeCopter(const ImuLogic &imu,
                             float throttle, float yawSp, float pitchSp, float rollSp,
                             float dt)
    {
        MixerInput result;
        result.throttle = throttle;

        // Zewnętrzna pętla: angle -> rate
        rollDesired = rollAnglePid.compute(imu.getRoll(), rollSp, dt);
        pitchDesired = pitchAnglePid.compute(imu.getPitch(), pitchSp, dt);

        // Wewnętrzna pętla: rate -> output
        result.yaw = yawRatePid.compute(imu.getYawRate(), yawSp, dt);
        result.roll = rollRatePid.compute(imu.getRollRate(), rollDesired, dt);
        result.pitch = pitchRatePid.compute(imu.getPitchRate(), pitchDesired, dt);

        return result;
    }

    /**
     * @brief Oblicz wyjścia PID dla trybu tank (prostsza stabilizacja)
     * @param imu Dane z IMU
     * @param throttle Throttle setpoint
     * @param yawSp Yaw setpoint (sterowanie)
     * @param dt Czas od ostatniego wywołania
     * @return MixerInput z wartościami do miksera
     */
    MixerInput computeTank(const ImuLogic &imu,
                           float throttle, float yawSp,
                           float dt)
    {
        MixerInput result;
        result.throttle = throttle;
        result.yaw = yawSp; // Bezpośrednie sterowanie
        result.pitch = 0.0f;
        result.roll = 0.0f;

        // Opcjonalnie: PID dla stabilizacji (na razie wyłączony)
        // result.yaw = yawRatePid.compute(imu.getYawRate(), yawSp, dt);

        return result;
    }

    /** @brief Reset wszystkich PID */
    void reset()
    {
        yawRatePid.reset();
        pitchRatePid.reset();
        rollRatePid.reset();
        pitchAnglePid.reset();
        rollAnglePid.reset();
        rollDesired = 0.0f;
        pitchDesired = 0.0f;
    }

    // Gettery do debugowania
    float getRollDesired() const { return rollDesired; }
    float getPitchDesired() const { return pitchDesired; }
};
