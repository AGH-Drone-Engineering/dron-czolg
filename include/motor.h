#include "config.h"
#include "DShot.h"
#include "pids3d.h"
#include "mpu6050_sensor.h"
#include "servo_controller.h"
#include "sbus_reader.h"

class Motor_controller
{
public:
    Motor_controller(Sbus_reader &sbus_reader_);
    void init();
    void reset_motors();
    void update_mode(float change_to_); // determine mode based on switch position
    // takes float from Sbus_reader.get_mode() which returns float
    void update_motors(
        Mpu6050_Sensor &mpu_sensor_,
        float dt_);

    void set_vehicle_PWM();

    void safety_land();

    void arm_motors();

    void disarm_motors();

    bool is_armed() { return armed; }

    void set_armed(bool arm_status) { armed = arm_status; }

    bool can_arm();

    float get_tl() { return tl; }
    float get_tr() { return tr; }

private:
    ServoController servo_left;
    ServoController servo_right;
    float fl, fr, bl, br, tl, tr; // front-left, ..., tank-left, tank-right
    DShot motor_fl;
    DShot motor_fr;
    DShot motor_bl;
    DShot motor_br;
    DShot motor_tl;
    DShot motor_tr;
    Pids3d pids_outer;
    Pids3d pids_inner;
    vehicle_mode_t current_mode;
    vehicle_mode_t target_mode;
    unsigned long transition_start_time;
    float throttle_sp, yaw_sp, roll_sp, pitch_sp;
    float roll_desired, pitch_desired;
    float pid_yaw_ctrl, pid_roll_ctrl, pid_pitch_ctrl;

    Sbus_reader &sbus_reader_ref;
    bool armed = false;
    int map_motor_values(float val);
};
