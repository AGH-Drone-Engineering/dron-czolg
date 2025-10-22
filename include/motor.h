#include "config.h"
#include "DShot.h"
#include "pids3d.h"
#include "mpu6050_sensor.h"
#include "servo_controller.h"

class Motor_controller
{
public:
    Motor_controller();
    void init();
    void reset_motors();
    void update_mode(float change_to_); // determine mode based on switch position
    // takes float from Sbus_reader.get_mode() which returns float
    void update_motors(
        float *sbus_data_,
        Mpu6050_Sensor &mpu_sensor_,
        float dt_);

    void set_vehicle_PWM();

    void safety_land();

    void arm_motors();

    void disarm_motors();

    bool is_armed() { return armed; }

    void set_armed(bool arm_status) { armed = arm_status; }

private:
    Pids3d pids_inner;
    Pids3d pids_outer;
    ServoController servo_left;
    ServoController servo_right;
    float fl, fr, bl, br, tl, tr; // front-left, ..., tank-left, tank-right
    DShot motor_fl;
    DShot motor_fr;
    DShot motor_bl;
    DShot motor_br;
    DShot motor_tl;
    DShot motor_tr;
    vehicle_mode_t current_mode;
    float throttle_sp, yaw_sp, roll_sp, pitch_sp;
    float roll_desired, pitch_desired;
    float pid_yaw_ctrl, pid_roll_ctrl, pid_pitch_ctrl;

    bool armed = false;
};
