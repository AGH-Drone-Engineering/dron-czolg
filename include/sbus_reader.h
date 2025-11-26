#pragma once

#include "config.h"
#include "sbus.h"

// sbus channels:
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
#define RC_MODE_CH 6 // Switch between tank and copter mode
                     // for switches

class Sbus_reader
{
public:
    Sbus_reader();
    void init();

    float get_throttle();
    float get_yaw();
    float get_pitch();
    float get_roll();
    float get_arm_status();
    float get_mode();
    float *get_data();
    float get_status();

    float Read_Sbus();
    void print_data();

private:
    static constexpr float SBUS_MIN = 175.0f;
    static constexpr float SBUS_MAX = 1811.0f;
    static constexpr float DSHOT_MIN_THROTTLE = 48.0f;
    static constexpr float DSHOT_MAX_THROTTLE = 2047.0f;
    static constexpr float SBUS_CENTER = (SBUS_MIN + SBUS_MAX) / 2;
    bfs::SbusRx sbus_rx;
    bfs::SbusData sbus_data;

    float data[6]; // rc_throttle, rc_yaw, rc_pitch, rc_roll, rc_arm, rc_mode

    // 0 - OK
    // 1 - Signal lost
    // 2 - Failsafe
    int status; // may be enum later

    float NormalizeChannel(int16_t channel_);
    float isChannelActive(int16_t channel_);
};
