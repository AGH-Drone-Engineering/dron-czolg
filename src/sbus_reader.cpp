#include "sbus_reader.h"

Sbus_reader::Sbus_reader()
#ifdef NATIVE_TESTING
    : sbus_rx()
#else
    : sbus_rx(&SBUS_INPUT)
#endif
{
}

void Sbus_reader::init()
{
    sbus_rx.Begin();
}

float Sbus_reader::NormalizeChannel(int16_t channel)
{
    if (channel < 0 || channel >= bfs::SbusData::NUM_CH)
        return 0;
    int raw_val = sbus_data.ch[channel];
    return constrain(raw_val, SBUS_MIN, SBUS_MAX);
}

float Sbus_reader::isChannelActive(int16_t channel)
{
    if (channel < 0 || channel >= bfs::SbusData::NUM_CH)
        return 0.0;
    int raw_val = sbus_data.ch[channel];
    return raw_val > SBUS_CENTER ? 1.0 : 0.0;
}

float Sbus_reader::Read_Sbus()
{
    if (sbus_rx.Read())
    {
        sbus_data = sbus_rx.data();
        data[0] = NormalizeChannel(RC_THROTTLE_CH);
        data[1] = NormalizeChannel(RC_YAW_CH);
        data[2] = isChannelActive(RC_ARM_CH);
        data[3] = isChannelActive(RC_MODE_CH);
        return 0.0;
    }
    return 1.0;
}

float Sbus_reader::get_throttle() { return data[0]; }
float Sbus_reader::get_yaw() { return data[1]; }
float Sbus_reader::get_arm_status() { return data[2]; }
float Sbus_reader::get_mode() { return data[3]; }