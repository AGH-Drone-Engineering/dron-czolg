#pragma once
#include <cstdint>

struct ImuData
{
    int16_t accelX, accelY, accelZ;
    int16_t gyroX, gyroY, gyroZ;
};

class IImuSensor
{
public:
    virtual bool readData(ImuData &data) = 0; // Czytaj surowe dane
    virtual ~IImuSensor() {}
};