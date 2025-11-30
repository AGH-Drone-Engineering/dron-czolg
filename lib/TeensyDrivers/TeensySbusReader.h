#pragma once
#include <Arduino.h>
#include "ISbusReader.h"
#include "SBUS.h" // Biblioteka bfs::SbusRx

class TeensySbusReader : public ISbusReader
{
    bfs::SbusRx sbus;
    bfs::SbusData sbusData;
    bool dataValid = false;

public:
    TeensySbusReader(HardwareSerial &serial) : sbus(&serial)
    {
        sbus.Begin();
    }

    bool readChannels() override
    {
        if (sbus.Read())
        {
            sbusData = sbus.data();
            dataValid = true;
            return true;
        }
        dataValid = false;
        return false;
    }

    int16_t getChannel(int ch) const override
    {
        if (ch >= 0 && ch < bfs::SbusData::NUM_CH)
        {
            return sbusData.ch[ch];
        }
        return 0;
    }

    SbusStatus getStatus() const override
    {
        if (!dataValid)
            return SbusStatus::LOST_FRAME;
        if (sbusData.failsafe)
            return SbusStatus::FAILSAFE;
        if (sbusData.lost_frame)
            return SbusStatus::LOST_FRAME;
        return SbusStatus::OK;
    }

    bool isFailsafe() const override
    {
        return sbusData.failsafe;
    }

    bool isLostFrame() const override
    {
        return sbusData.lost_frame || !dataValid;
    }
};