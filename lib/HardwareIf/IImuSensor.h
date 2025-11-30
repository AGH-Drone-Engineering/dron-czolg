#pragma once
#include <cstdint>

struct ImuData
{
    // Surowe dane (raw)
    int16_t accelX, accelY, accelZ;
    int16_t gyroX, gyroY, gyroZ;
    
    // Przeliczone dane (w jednostkach SI: m/s², rad/s)
    float accelXf, accelYf, accelZf;  // Przyspieszenie w m/s² lub g
    float gyroXf, gyroYf, gyroZf;     // Prędkość kątowa w rad/s
};

class IImuSensor
{
public:
    virtual bool readData(ImuData &data) = 0; // Czytaj surowe dane
    virtual ~IImuSensor() {}
};