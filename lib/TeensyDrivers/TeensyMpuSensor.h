#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "IImuSensor.h"

class TeensyMpuSensor : public IImuSensor
{
    uint8_t address;
    
    // Skale dla MPU6050
    // Akcelerometr: ±8g -> 4096 LSB/g
    // Żyroskop: ±500°/s -> 65.5 LSB/(°/s)
    static constexpr float ACCEL_SCALE = 4096.0f;
    static constexpr float GYRO_SCALE = 65.5f;

public:
    TeensyMpuSensor(uint8_t addr = 0x68) : address(addr)
    {
        Wire.begin();
        
        // Inicjalizacja MPU6050
        // Wybudź czujnik (power management)
        Wire.beginTransmission(address);
        Wire.write(0x6B); // PWR_MGMT_1
        Wire.write(0x00); // Wybudź
        Wire.endTransmission(true);
        
        // Ustaw zakres akcelerometru na ±8g
        Wire.beginTransmission(address);
        Wire.write(0x1C); // ACCEL_CONFIG
        Wire.write(0x10); // ±8g
        Wire.endTransmission(true);
        
        // Ustaw zakres żyroskopu na ±500°/s
        Wire.beginTransmission(address);
        Wire.write(0x1B); // GYRO_CONFIG
        Wire.write(0x08); // ±500°/s
        Wire.endTransmission(true);
        
        // Ustaw filtr dolnoprzepustowy (DLPF)
        Wire.beginTransmission(address);
        Wire.write(0x1A); // CONFIG
        Wire.write(0x06); // ~5Hz bandwidth
        Wire.endTransmission(true);
    }

    bool readData(ImuData &data) override
    {
        Wire.beginTransmission(address);
        Wire.write(0x3B); // Rejestr akcelerometru (ACCEL_XOUT_H)
        if (Wire.endTransmission(false) != 0)
            return false;
            
        Wire.requestFrom(address, (uint8_t)14);
        if (Wire.available() != 14)
            return false;

        // Odczyt surowych danych (big-endian)
        data.accelX = (Wire.read() << 8) | Wire.read();
        data.accelY = (Wire.read() << 8) | Wire.read();
        data.accelZ = (Wire.read() << 8) | Wire.read();
        
        // Pomijamy temperaturę (2 bajty)
        Wire.read();
        Wire.read();
        
        data.gyroX = (Wire.read() << 8) | Wire.read();
        data.gyroY = (Wire.read() << 8) | Wire.read();
        data.gyroZ = (Wire.read() << 8) | Wire.read();
        
        // Przelicz na jednostki fizyczne
        data.accelXf = data.accelX / ACCEL_SCALE; // g
        data.accelYf = data.accelY / ACCEL_SCALE;
        data.accelZf = data.accelZ / ACCEL_SCALE;
        
        data.gyroXf = data.gyroX / GYRO_SCALE; // °/s
        data.gyroYf = data.gyroY / GYRO_SCALE;
        data.gyroZf = data.gyroZ / GYRO_SCALE;

        return true;
    }
};