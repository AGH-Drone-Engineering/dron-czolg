#pragma once

#include "config.h"
#include "DShot.h"
#include "servo_controller.h"
#include "sbus_reader.h"
#include <string>

using std::string;

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
    void run_one_motor_test(string motor_name, int16_t throttle);
    vehicle_mode_t get_current_mode() { return current_mode; };

private:
    Sbus_reader &sbus_reader_ref;
    DShot motor_tl;
    DShot motor_tr;
    DShot motor_drone_fl;
    DShot motor_drone_fr;
    DShot motor_drone_bl;
    DShot motor_drone_br;
    bool armed = false;
    ServoController servo_left;
    ServoController servo_right;
    vehicle_mode_t current_mode;
    float tl, tr; // tank-left, tank-right
    void set_tank_motor_pwm(std::string motor_name, int pwm_value);
};
