// lib/CoreLogic/Pid.h
#pragma once

struct PidGains
{
    float kp = 0.0f;
    float ki = 0.0f;
    float kd = 0.0f;
    float outMin = -1000.0f;
    float outMax = 1000.0f;
};

/**
 * @brief Pojedynczy regulator PID
 * 
 * Implementuje algorytm PID z anti-windup i ograniczeniem wyjścia.
 * Zgodny z oryginalną klasą Pids ze starego kodu.
 */
class Pid
{
    PidGains gains;
    float prevError = 0.0f;
    float integral = 0.0f;

public:
    Pid() = default;
    Pid(float kp, float ki, float kd, float outMin = -1000.0f, float outMax = 1000.0f);
    Pid(const PidGains &g);

    /**
     * @brief Oblicz wyjście PID
     * @param measurement Aktualna wartość mierzona
     * @param setpoint Wartość zadana
     * @param dt Czas od ostatniego wywołania [s]
     * @return Sygnał sterujący
     */
    float compute(float measurement, float setpoint, float dt);

    /** @brief Zeruj stan regulatora (integral i prevError) */
    void reset();

    /** @brief Ustaw wzmocnienia */
    void setGains(float kp, float ki, float kd);
    void setGains(const PidGains &g);

    /** @brief Ustaw limity wyjścia */
    void setOutputLimits(float outMin, float outMax);

    /** @brief Pobierz aktualne wzmocnienia */
    const PidGains &getGains() const { return gains; }
};
