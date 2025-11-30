#pragma once
#include "config.h"
#include "IImuSensor.h"
#include "IPidTimer.h"

class ImuLogic
{
    IImuSensor &sensor;   // DI: Wstrzyknięcie sensora
    IPidTimer &timer;     // DI: Timer do obliczania dt
    
    // Obliczone kąty (filtrowane)
    float pitch_filtered = 0, roll_filtered = 0, yaw_gyro = 0;
    
    // Kąty z akcelerometru
    float pitch_acc = 0, roll_acc = 0;
    
    // Kąty z żyroskopu (zintegrowane)
    float pitch_gyro = 0, roll_gyro = 0;
    
    // Prędkości kątowe (rate) - bezpośrednio z żyroskopu
    float pitch_rate = 0, roll_rate = 0, yaw_rate = 0;
    
    // Parametry filtra
    float alpha = IMU_FILTER_ALPHA;  // Współczynnik filtra komplementarnego (0.9-0.99)
    
    // Timing
    uint32_t last_time = 0;

public:
    ImuLogic(IImuSensor &s, IPidTimer &t) : sensor(s), timer(t) {}
    
    bool update(); // Czytaj i oblicz kąty
    
    // Gettery dla kątów (filtrowanych)
    float getPitch() const { return pitch_filtered; }
    float getRoll() const { return roll_filtered; }
    float getYaw() const { return yaw_gyro; }
    
    // Gettery dla prędkości kątowych (rate) - potrzebne do kaskadowego PID
    float getPitchRate() const { return pitch_rate; }
    float getRollRate() const { return roll_rate; }
    float getYawRate() const { return yaw_rate; }
    
    // Ustawienie współczynnika filtra komplementarnego
    void setAlpha(float a) { alpha = a; }
};
