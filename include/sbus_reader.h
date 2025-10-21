#pragma once

#include "config.h"
#include "sbus.h"

class Sbus_reader
{
private:
    static constexpr int SBUS_MIN = 172;
    static constexpr int SBUS_MAX = 1811;
    static constexpr int SBUS_CENTER = (SBUS_MIN + SBUS_MAX) / 2;
    static constexpr float SBUS_HALF_RANGE = (SBUS_MAX - SBUS_MIN) / 2.0f;
    bfs::SbusRx sbus_rx;
    bfs::SbusData sbus_data;

    float data[5]; // rc_throttle, rc_steer, rc_pitch, rc_roll, rc_mode

    // 0 - OK
    // 1 - Signal lost
    // 2 - Failsafe
    int status; // may be enum later

    float NormalizeChannel(int16_t channel_);

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
    int get_status();

    int Read_Sbus();
    void print_data();
};
