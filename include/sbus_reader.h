#pragma once

#include "config.h"
#include "sbus.h"

// Kanały SBUS:
// 2 - throttle (góra/dół lewy drążek)
// 3 - yaw (lewo/prawo lewy drążek)
// 5 - arm (lewy przełącznik)
// 6 - mode (prawy przełącznik)

#define RC_THROTTLE_CH 2
#define RC_YAW_CH 3
#define RC_ARM_CH 5
#define RC_MODE_CH 6

class Sbus_reader
{
public:
    Sbus_reader();
    void init();

    float get_throttle();
    float get_yaw();
    float get_arm_status();
    float get_mode();

    float Read_Sbus();

private:
    bfs::SbusRx sbus_rx;
    bfs::SbusData sbus_data;
    float data[4]; // throttle, yaw, arm, mode

    float NormalizeChannel(int16_t channel_);
    float isChannelActive(int16_t channel_);
};
