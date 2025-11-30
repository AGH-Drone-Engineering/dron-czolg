#pragma once
#include <stdint.h>

// Dopasowane do prawdziwego enum (bez 'class')
enum DShotType
{
    DShot150,
    DShot300,
    DShot600
};

class DShot
{
public:
    // Dopasowany konstruktor (symuluje HardwareSerial *uart)
    DShot(void *uart, DShotType type);

    // Publiczne metody z oryginału
    void sendThrottle(int16_t throttle, bool telemetry = false);

    void sendCommand(uint8_t command, bool telemetry = false);

    // Zmienne pomocnicze do testów
    int16_t last_sent_throttle = 0;
    uint8_t last_sent_command = 0;
    bool last_telemetry = false;
    bool sendThrottle_called = false;
    bool sendCommand_called = false;
};