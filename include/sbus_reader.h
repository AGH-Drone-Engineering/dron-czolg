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

    int get_throttle();
    int get_yaw();
    int get_pitch();
    int get_roll();
    int get_arm_status();
    int get_mode();
    int *get_data();
    int get_status();

    int Read_Sbus();
    void print_data();

private:
    static constexpr int SBUS_MIN = 175;
    static constexpr int SBUS_MAX = 1811;
    static constexpr int DSHOT_MIN_THROTTLE = 48;
    static constexpr int DSHOT_MAX_THROTTLE = 2047;
    static constexpr int SBUS_CENTER = (SBUS_MIN + SBUS_MAX) / 2;
    bfs::SbusRx sbus_rx;
    bfs::SbusData sbus_data;

    int data[6]; // rc_throttle, rc_yaw, rc_pitch, rc_roll, rc_arm, rc_mode

    // 0 - OK
    // 1 - Signal lost
    // 2 - Failsafe
    int status; // may be enum later

    int NormalizeChannel(int16_t channel_);
    int isChannelActive(int16_t channel_);
};
