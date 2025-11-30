// lib/TeensyDrivers/TeensyDShotDriver.h
#pragma once
#include <Arduino.h>
#include "IMotorDriver.h"

// Stałe DShot
constexpr uint8_t DSHOT_ZERO_BYTE = 0b11111000;
constexpr uint8_t DSHOT_ONE_BYTE = 0b10000000;

enum class DShotType
{
    DShot150,
    DShot300,
    DShot600
};

class TeensyDShotDriver : public IMotorDriver
{
    HardwareSerial &serial;

    // Tworzy pakiet DShot z CRC
    static uint16_t createPacket(uint16_t value, bool telemetry)
    {
        value = value & 0x07FF; // 11 bitów
        uint16_t packet = (value << 1) | (telemetry ? 1 : 0);
        uint16_t crc = (packet ^ (packet >> 4) ^ (packet >> 8)) & 0x0F;
        return (packet << 4) | crc;
    }

public:
    TeensyDShotDriver(HardwareSerial &s, DShotType type = DShotType::DShot600) : serial(s)
    {
        // Inicjalizacja UART z odpowiednią prędkością dla DShot
        switch (type)
        {
        case DShotType::DShot150:
            serial.begin(1500000, SERIAL_8N1);
            break;
        case DShotType::DShot300:
            serial.begin(3000000, SERIAL_8N1);
            break;
        case DShotType::DShot600:
        default:
            serial.begin(6000000, SERIAL_8N1);
            break;
        }
    }

    // Wyślij surowy pakiet DShot (16 bitów -> 16 bajtów bitstream)
    void sendPacket(uint16_t packet) override
    {
        uint8_t buffer[16];

        // Konwertuj każdy bit pakietu na bajt DShot
        for (uint8_t i = 0; i < 16; i++)
        {
            if (packet & ((1 << 15) >> i))
            {
                buffer[i] = DSHOT_ONE_BYTE;
            }
            else
            {
                buffer[i] = DSHOT_ZERO_BYTE;
            }
        }

        serial.write(buffer, 16);
    }

    // Wyślij throttle (0-2047) z automatyczną konwersją
    void sendThrottle(uint16_t throttle, bool telemetry = false) override
    {
        uint16_t packet = createPacket(throttle, telemetry);
        sendPacket(packet);
    }

    // Wyślij komendę specjalną (0-47)
    void sendCommand(uint8_t command, bool telemetry = false) override
    {
        command = (command > 47) ? 47 : command;
        uint16_t packet = createPacket(command, telemetry);
        sendPacket(packet);
    }
};