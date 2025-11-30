#pragma once
#include <Arduino.h>
#include "IPidTimer.h"

class TeensyPidTimer : public IPidTimer
{
public:
    uint32_t getMillis() const override { return millis(); }
};