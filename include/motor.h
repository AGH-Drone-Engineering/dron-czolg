#include "main.h"
#include "DShot.h"
#include "pids3d.h"
#include "mpu6050_sensor.h"

class Motor_controller
{
private:
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

public:
    Motor_controller();
    void init();
    void reset_motors();
    void update_mode(float change_to_); // determine mode based on switch position
    // takes float from Sbus_reader.get_mode() which returns float
    void update_motors(
        float *sbus_data_,
        Pids3d &pid_inner_,
        Pids3d &pid_outer_,
        Mpu6050_Sensor &mpu_sensor_,
        float dt_);

    void set_vehicle_PWM();
};