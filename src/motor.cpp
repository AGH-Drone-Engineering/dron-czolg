#include "motor.h"
#include <servo_controller.h>

Motor_controller::Motor_controller()
    : motor_fl(MOTOR_PORT_FL, DShotType::DShot600),
      motor_fr(MOTOR_PORT_FR, DShotType::DShot600),
      motor_bl(MOTOR_PORT_BL, DShotType::DShot600),
      motor_br(MOTOR_PORT_BR, DShotType::DShot600),
      motor_tl(MOTOR_PORT_TL, DShotType::DShot600),
      motor_tr(MOTOR_PORT_TR, DShotType::DShot600),
      pids_inner(0.f, 0.f, 0.f, -1000.f, 1000.f),
      pids_outer(0.f, 0.f, 0.f, -1000.f, 1000.f)
{
}

void Motor_controller::init()
{
    pids_inner = pids_inner;
    pids_outer = pids_outer;
    pids_inner.init_all();
    pids_outer.init_all();
    Serial.println("Pids initialized");

    reset_motors();
    Serial.println("Motors initialized");
}

void Motor_controller::reset_motors()
{
    motor_fl.sendThrottle(0);
    motor_fr.sendThrottle(0);
    motor_bl.sendThrottle(0);
    motor_br.sendThrottle(0);
    motor_tl.sendThrottle(0);
    motor_tr.sendThrottle(0);
}

void Motor_controller::update_mode(float change_to_)
{
    if (change_to_ > 1000)
    {
        // Ensure all motors are stopped before switching modes
        if (current_mode != MODE_COPTER && tl <= SWITCH_MOTOR_PWM_THRESHOLD && tr <= SWITCH_MOTOR_PWM_THRESHOLD)
        {
            current_mode = MODE_COPTER;
            pids_inner.reset();
            pids_outer.reset();
        }
    }
    else
    {
        if (current_mode != MODE_TANK && bl <= SWITCH_MOTOR_PWM_THRESHOLD && br <= SWITCH_MOTOR_PWM_THRESHOLD && fl <= SWITCH_MOTOR_PWM_THRESHOLD && fr <= SWITCH_MOTOR_PWM_THRESHOLD)
        {
            current_mode = MODE_TANK;
            pids_inner.reset();
            pids_outer.reset();
            // again place for servo
        }
    }
}

void Motor_controller::update_motors(
    float *sbus_data_,
    Mpu6050_Sensor &mpu_sensor_,
    float dt_)
{
    if (current_mode == MODE_TANK)
    {
        throttle_sp = sbus_data_[0] * THROTTLE_COEF_TANK;
        yaw_sp = sbus_data_[1] * STEER_COEF;
        pitch_sp = 0.0f;
        roll_sp = 0.0f;

        pid_yaw_ctrl = pids_inner.yaw.compute(mpu_sensor_.get_yaw_rate(), yaw_sp, dt_);
        pid_roll_ctrl = pids_inner.roll.compute(mpu_sensor_.get_roll(), roll_sp, dt_);
        pid_pitch_ctrl = pids_inner.pitch.compute(mpu_sensor_.get_pitch(), pitch_sp, dt_);

        tl = throttle_sp - pid_yaw_ctrl + pid_roll_ctrl + pid_pitch_ctrl;
        tr = throttle_sp + pid_yaw_ctrl - pid_roll_ctrl - pid_pitch_ctrl;

        fl = 0;
        fr = 0;
        bl = 0;
        br = 0;
    }
    else
    {
        throttle_sp = sbus_data_[0] * THROTTLE_COEF_COPTER;
        yaw_sp = sbus_data_[1] * YAW_RATE_COEF;
        pitch_sp = sbus_data_[2] * PITCH_COEF;
        roll_sp = sbus_data_[4] * ROLL_COEF;

        pid_yaw_ctrl = pids_inner.yaw.compute(mpu_sensor_.get_yaw_rate(), yaw_sp, dt_);
        roll_desired = pids_outer.roll.compute(mpu_sensor_.get_roll(), roll_sp, dt_);
        pitch_desired = pids_outer.pitch.compute(mpu_sensor_.get_pitch(), pitch_sp, dt_);
        pid_roll_ctrl = pids_inner.roll.compute(mpu_sensor_.get_roll_rate(), roll_desired, dt_);
        pid_pitch_ctrl = pids_inner.pitch.compute(mpu_sensor_.get_pitch_rate(), pitch_desired, dt_);

        fr = throttle_sp - pid_yaw_ctrl + pid_roll_ctrl + pid_pitch_ctrl;
        fl = throttle_sp + pid_yaw_ctrl - pid_roll_ctrl + pid_pitch_ctrl;
        br = throttle_sp + pid_yaw_ctrl + pid_roll_ctrl - pid_pitch_ctrl;
        bl = throttle_sp - pid_yaw_ctrl - pid_roll_ctrl - pid_pitch_ctrl;

        tl = 0;
        tr = 0;
    }
    // Print PID output
    Serial.print("PID Control Yaw: ");
    Serial.print(pid_yaw_ctrl);
    Serial.print(", Roll: ");
    Serial.print(pid_roll_ctrl);
    Serial.print(", Pitch: ");
    Serial.println(pid_pitch_ctrl);
}

void Motor_controller::set_vehicle_PWM()
{
    motor_fl.sendThrottle(fl);
    motor_fr.sendThrottle(fr);
    motor_bl.sendThrottle(bl);
    motor_br.sendThrottle(br);
    motor_tl.sendThrottle(tl);
    motor_tr.sendThrottle(tr);
}