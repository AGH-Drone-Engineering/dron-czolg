#include "SbusParser.h"
#include "Utils.h"

float SbusParser::normalizeChannel(int16_t rawValue)
{
    return constrainFloat(static_cast<float>(rawValue), SBUS_MIN, SBUS_MAX);
}

float SbusParser::isChannelActive(int16_t rawValue)
{
    return rawValue > SBUS_CENTER ? 1.0f : 0.0f;
}

float SbusParser::parse()
{
    if (!reader.readChannels())
    {
        status = SbusStatus::LOST_FRAME;
        return 1.0f;
    }

    // Sprawdź status failsafe
    if (reader.isFailsafe())
    {
        status = SbusStatus::FAILSAFE;
        return 2.0f;
    }

    if (reader.isLostFrame())
    {
        status = SbusStatus::LOST_FRAME;
        return 1.0f;
    }

    status = SbusStatus::OK;

    // Parsuj kanały zgodnie z mapowaniem RC
    data[0] = normalizeChannel(reader.getChannel(RC_THROTTLE_CH)); // Throttle
    data[1] = normalizeChannel(reader.getChannel(RC_YAW_CH));      // Yaw
    data[2] = normalizeChannel(reader.getChannel(RC_PITCH_CH));    // Pitch
    data[3] = normalizeChannel(reader.getChannel(RC_ROLL_CH));     // Roll
    data[4] = isChannelActive(reader.getChannel(RC_ARM_CH));       // Arm status (0 lub 1)
    data[5] = isChannelActive(reader.getChannel(RC_MODE_CH));      // Mode (0 = tank, 1 = copter)

    return 0.0f;
}