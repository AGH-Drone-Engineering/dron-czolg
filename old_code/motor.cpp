#include "motor.h"

Motor_controller::Motor_controller(Sbus_reader &sbus_reader_)
    : sbus_reader_ref(sbus_reader_),
      motor_fl(MOTOR_PORT_FL, DShotType::DShot600),
      motor_fr(MOTOR_PORT_FR, DShotType::DShot600),
      motor_bl(MOTOR_PORT_BL, DShotType::DShot600),
      motor_br(MOTOR_PORT_BR, DShotType::DShot600),
      motor_tl(MOTOR_PORT_TL, DShotType::DShot600),
      motor_tr(MOTOR_PORT_TR, DShotType::DShot600),
      pids_inner(0.f, 0.f, 0.f, -1000.f, 1000.f),
      pids_outer(0.f, 0.f, 0.f, -1000.f, 1000.f),
      servo_left(SERVO_LEFT_PIN),
      servo_right(SERVO_RIGHT_PIN),
      current_mode(MODE_TANK)
{
}

void Motor_controller::init()
{
    sbus_reader_ref.init();
    Serial.println("Sbus initialized");

    pids_inner = pids_inner;
    pids_outer = pids_outer;
    pids_inner.init_all();
    pids_outer.init_all();
    Serial.println("Pids initialized");

    reset_motors();
    Serial.println("Motors initialized");

    servo_left.attach();
    servo_right.attach();
    Serial.println("Servos attached");

    current_mode = MODE_TRANSITION;
    transition_start_time = 0;
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

// we should make some handling for transition during flight to prevent sudden engine stop
void Motor_controller::update_mode(float change_to_)
{
    vehicle_mode_t desired_mode = (change_to_ > 0.5f) ? MODE_COPTER : MODE_TANK;
    if (current_mode == MODE_TRANSITION)
    {
        if (current_mode == MODE_TRANSITION)
        {
            if (transition_start_time == 0)
            {
                current_mode = desired_mode;
                target_mode = desired_mode;

                // Ustaw serwa natychmiast
                if (current_mode == MODE_COPTER)
                {
                    servo_left.set_servo_copter_mode();
                    servo_right.set_servo_copter_mode();
                }
                else
                {
                    servo_left.set_servo_tank_mode();
                    servo_right.set_servo_tank_mode();
                }
                Serial.print("INIT MODE: ");
                Serial.println(current_mode == MODE_COPTER ? "COPTER" : "TANK");
                return;
            }
            if (millis() - transition_start_time >= TRANSITION_TIME)
            {
                current_mode = target_mode;
                pids_inner.reset();
                pids_outer.reset();

                Serial.print("TRANSITION DONE -> ");
                Serial.println(current_mode == MODE_COPTER ? "COPTER" : "TANK");
            }
        }
    }
    else
    {
        if (desired_mode != current_mode && desired_mode != target_mode)
        {
            Serial.println("STARTING TRANSITION...");
            current_mode = MODE_TRANSITION;
            target_mode = desired_mode;
            transition_start_time = millis();

            pids_inner.reset();
            pids_outer.reset();

            // Rozpocznij ruch serw
            if (target_mode == MODE_COPTER)
            {
                servo_left.set_servo_copter_mode();
                servo_right.set_servo_copter_mode();
            }
            else
            {
                servo_left.set_servo_tank_mode();
                servo_right.set_servo_tank_mode();
            }
        }
    }
}

void Motor_controller::update_motors(
    Mpu6050_Sensor &mpu_sensor_,
    float dt_)
{
    if (current_mode == MODE_TRANSITION)
    {
        fl = 0;
        fr = 0;
        bl = 0;
        br = 0;
        tl = 0;
        tr = 0;
        return;
    }
    if (current_mode == MODE_TANK)
    {
        throttle_sp = sbus_reader_ref.get_throttle() * THROTTLE_COEF_TANK;
        yaw_sp = sbus_reader_ref.get_yaw() * STEER_COEF;
        pitch_sp = 0.0f;
        roll_sp = 0.0f;

        pid_yaw_ctrl = pids_inner.yaw.compute(mpu_sensor_.get_yaw_rate(), yaw_sp, dt_);
        pid_roll_ctrl = pids_inner.roll.compute(mpu_sensor_.get_roll(), roll_sp, dt_);
        pid_pitch_ctrl = pids_inner.pitch.compute(mpu_sensor_.get_pitch(), pitch_sp, dt_);

        // tl = throttle_sp - pid_yaw_ctrl + pid_roll_ctrl + pid_pitch_ctrl;
        // tr = throttle_sp + pid_yaw_ctrl - pid_roll_ctrl - pid_pitch_ctrl;
        tl = throttle_sp;
        tr = throttle_sp;
        fl = 0;
        fr = 0;
        bl = 0;
        br = 0;
    }
    else
    {
        throttle_sp = sbus_reader_ref.get_throttle() * THROTTLE_COEF_COPTER;
        yaw_sp = sbus_reader_ref.get_yaw() * YAW_RATE_COEF;
        pitch_sp = sbus_reader_ref.get_pitch() * PITCH_COEF;
        roll_sp = sbus_reader_ref.get_roll() * ROLL_COEF;

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

    // Serial.print("PID Control Yaw: ");
    // Serial.print(pid_yaw_ctrl);
    // Serial.print(", Roll: ");
    // Serial.print(pid_roll_ctrl);
    // Serial.print(", Pitch: ");
    // Serial.println(pid_pitch_ctrl);
}

void Motor_controller::set_vehicle_PWM()
{
    if (!armed)
    {
        reset_motors();
        return;
    }

    motor_fl.sendThrottle(fl);
    motor_fr.sendThrottle(fr);
    motor_bl.sendThrottle(bl);
    motor_br.sendThrottle(br);
    motor_tl.sendThrottle(tl);
    motor_tr.sendThrottle(tr);
}

void Motor_controller::safety_land()
{
    // Gradually reduce throttle to zero
    float reduction_step = SAFETY_LAND_REDUCTION_STEP;
    if (current_mode == MODE_TANK)
    {
        if (tl > 0)
            tl -= reduction_step;
        if (tr > 0)
            tr -= reduction_step;
        if (tl < 0)
            tl = 0;
        if (tr < 0)
            tr = 0;
    }
    else
    {
        if (fl > 0)
            fl -= reduction_step;
        if (fr > 0)
            fr -= reduction_step;
        if (bl > 0)
            bl -= reduction_step;
        if (br > 0)
            br -= reduction_step;
        if (fl < 0)
            fl = 0;
        if (fr < 0)
            fr = 0;
        if (bl < 0)
            bl = 0;
        if (br < 0)
            br = 0;
    }
}

void Motor_controller::arm_motors()
{
    if (this->can_arm())
    {
        set_armed(true);
        pids_inner.reset();
        pids_outer.reset();

        // Send minimum throttle to arm ESCs
        motor_fl.sendThrottle(DSHOT_THROTTLE_ACTIVE_MIN);
        motor_fr.sendThrottle(DSHOT_THROTTLE_ACTIVE_MIN);
        motor_bl.sendThrottle(DSHOT_THROTTLE_ACTIVE_MIN);
        motor_br.sendThrottle(DSHOT_THROTTLE_ACTIVE_MIN);
        motor_tl.sendThrottle(DSHOT_THROTTLE_ACTIVE_MIN);
        motor_tr.sendThrottle(DSHOT_THROTTLE_ACTIVE_MIN);
    }
}

void Motor_controller::disarm_motors()
{
    set_armed(false);
    reset_motors();
}

bool Motor_controller::can_arm()
{
    if (sbus_reader_ref.get_throttle() < DSHOT_THROTTLE_ACTIVE_MIN && current_mode != MODE_TRANSITION)
    {
        return true;
    }
    return false;
}

int Motor_controller::map_motor_values(float val)
{
    val = constrain(val, 48.0f, 2047.0f);
    return static_cast<int>(val);
}
