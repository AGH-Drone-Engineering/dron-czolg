#pragma once

#include "config.h"
#include "DShot.h"
#include "servo_controller.h"
#include "sbus_reader.h"

class Motor_controller
{
public:
    Motor_controller(Sbus_reader &sbus_reader_);
    void init();
    void reset_motors();
    void update_mode(float change_to_);
    void update_motors();
    void set_vehicle_PWM();
    void arm_motors();
    void disarm_motors();
    bool is_armed() { return armed; }
    void set_armed(bool arm_status) { armed = arm_status; }
    bool can_arm();

private:
    ServoController servo_left;
    ServoController servo_right;
    float tl, tr;  // tank-left, tank-right
    DShot motor_tl;
    DShot motor_tr;
    vehicle_mode_t current_mode;
    Sbus_reader &sbus_reader_ref;
    bool armed = false;
};
