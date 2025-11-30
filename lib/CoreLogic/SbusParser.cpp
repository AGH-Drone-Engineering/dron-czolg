#include "SbusParser.h"
#include "Utils.h"

bool SbusParser::parse()
{
    if (!reader.readChannels())
        return false;
    // Obronka danych (constrain jest z Arduino.h, pewnie trzeba bedzie zrobić własną)
    data[0] = constrain(reader.getChannel(2), SBUS_MIN, SBUS_MAX); // Throttle
    // ... reszta parsowania
    return true;
}