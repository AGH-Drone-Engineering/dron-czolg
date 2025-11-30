#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "IImuSensor.h"

class TeensyMpuSensor : public IImuSensor
{
    uint8_t address;

public:
    TeensyMpuSensor(uint8_t addr = 0x68) : address(addr)
    {
        Wire.begin();
        // Inicjalizacja MPU6050 (z oryginalnego kodu)
    }
    bool readData(ImuData &data) override
    {
        // Kod z mpu6050_sensor.cpp (I2C)
        Wire.beginTransmission(address);
        Wire.write(0x3B); // Rejestr akcelerometru
        Wire.endTransmission(false);
        Wire.requestFrom(address, 14);
        if (Wire.available() == 14)
        {
            data.accelX = Wire.read() << 8 | Wire.read();
            // ... reszta rejestrów
            return true;
        }
        return false;
    }
};