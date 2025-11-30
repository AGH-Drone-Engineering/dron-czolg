#pragma once
#include "ISbusReader.h"
#include "config.h"

// Mapowanie kanałów RC (zgodne ze starym kodem)
// 0 - roll (left/right right stick)
// 1 - pitch (up/down right stick)
// 2 - throttle (up/down left stick)
// 3 - yaw (left/right left stick)
// 5 - arm (left switch)
// 6 - mode (right switch)
#define RC_THROTTLE_CH 2
#define RC_YAW_CH 3
#define RC_PITCH_CH 1
#define RC_ROLL_CH 0
#define RC_ARM_CH 5
#define RC_MODE_CH 6

class SbusParser
{
    ISbusReader &reader; // DI: Wstrzyknięcie interfejsu
    float data[6] = {0}; // Parsed data: throttle, yaw, pitch, roll, arm, mode
    SbusStatus status = SbusStatus::OK;
    
    // Środek zakresu SBUS
    static constexpr float SBUS_CENTER = (SBUS_MIN + SBUS_MAX) / 2.0f;
    
    // Normalizacja kanału do zakresu [SBUS_MIN, SBUS_MAX]
    float normalizeChannel(int16_t rawValue);
    
    // Sprawdź czy kanał jest aktywny (przełącznik)
    float isChannelActive(int16_t rawValue);

public:
    SbusParser(ISbusReader &r) : reader(r) {}
    
    // Parsuje kanały na throttle/yaw itp.
    // Zwraca: 0 = OK, 1 = Lost frame, 2 = Failsafe
    float parse();
    
    // Gettery dla wartości sterujących
    float getThrottle() const { return data[0]; }
    float getYaw() const { return data[1]; }
    float getPitch() const { return data[2]; }
    float getRoll() const { return data[3]; }
    float getArmStatus() const { return data[4]; }
    float getMode() const { return data[5]; }
    
    // Pobierz wskaźnik do tablicy danych
    float* getData() { return data; }
    
    // Status odczytu
    SbusStatus getStatus() const { return status; }
    bool isFailsafe() const { return status == SbusStatus::FAILSAFE; }
    bool isLostFrame() const { return status == SbusStatus::LOST_FRAME; }
    bool isOk() const { return status == SbusStatus::OK; }
};
