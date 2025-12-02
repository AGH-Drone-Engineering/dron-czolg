#include "motor.h"

Motor_controller::Motor_controller(Sbus_reader &sbus_reader_)
    : sbus_reader_ref(sbus_reader_),
      motor_tl(MOTOR_PORT_TL, DShotType::DShot300),
      motor_tr(MOTOR_PORT_TR, DShotType::DShot300),
      motor_drone_fl(MOTOR_PORT_DRONE_FL, DShotType::DShot600),
      motor_drone_fr(MOTOR_PORT_DRONE_FR_1, DShotType::DShot600),
      motor_drone_bl(MOTOR_PORT_DRONE_BL, DShotType::DShot600),
      motor_drone_br(MOTOR_PORT_DRONE_BR, DShotType::DShot600),
      servo_left(SERVO_LEFT_PIN),
      servo_right(SERVO_RIGHT_PIN),
      current_mode(MODE_TANK),
      tl(0), tr(0)
{
}

void Motor_controller::init()
{
    sbus_reader_ref.init();
    reset_motors();
    // servo_left.attach();
    // servo_right.attach();
    // servo_left.set_servo_tank_mode();
    // servo_right.set_servo_tank_mode();
    current_mode = MODE_TANK;
}

void Motor_controller::reset_motors()
{
    motor_tl.sendThrottle(0);
    motor_tr.sendThrottle(0);
    tl = 0;
    tr = 0;
}

void Motor_controller::update_mode(float change_to_)
{
    vehicle_mode_t desired_mode = (change_to_ > 0.5f) ? MODE_COPTER : MODE_TANK;

    if (desired_mode != current_mode)
    {
        current_mode = desired_mode;
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
    }
}

void Motor_controller::update_motors()
{
    // Proste sterowanie tank (różnicowe)
    float throttle = sbus_reader_ref.get_throttle(); // SBUS_MIN do SBUS_MAX
    float yaw = sbus_reader_ref.get_yaw();           // SBUS_MIN do SBUS_MAX

    // Normalizuj do zakresu 0-1 i -1 do 1
    float throttle_norm = (throttle - SBUS_MIN) / (SBUS_MAX - SBUS_MIN); // 0 do 1
    float yaw_norm = (yaw - SBUS_CENTER) / (SBUS_CENTER - SBUS_MIN);     // -1 do 1

    // Sterowanie różnicowe
    float left = throttle_norm + yaw_norm * 0.5f;
    float right = throttle_norm - yaw_norm * 0.5f;

    // Ogranicz do 0-1
    left = constrain(left, 0.0f, 1.0f);
    right = constrain(right, 0.0f, 1.0f);

    // Mapuj na zakres DShot
    tl = left * (DSHOT_THROTTLE_ACTIVE_MAX - DSHOT_THROTTLE_ACTIVE_MIN) + DSHOT_THROTTLE_ACTIVE_MIN;
    tr = right * (DSHOT_THROTTLE_ACTIVE_MAX - DSHOT_THROTTLE_ACTIVE_MIN) + DSHOT_THROTTLE_ACTIVE_MIN;

    // Jeśli throttle bardzo niski, wyłącz motory
    if (throttle_norm < 0.05f)
    {
        tl = 0;
        tr = 0;
    }
}

void Motor_controller::set_vehicle_PWM()
{
    if (!armed)
    {
        reset_motors();
        return;
    }
    motor_tl.sendThrottle(tl);
    motor_tr.sendThrottle(tr);
}

void Motor_controller::arm_motors()
{
    if (can_arm())
    {
        set_armed(true);
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
    float throttle = sbus_reader_ref.get_throttle();
    float throttle_norm = (throttle - SBUS_MIN) / (SBUS_MAX - SBUS_MIN);
    return throttle_norm < 0.1f; // Można uzbroić tylko przy niskim throttle
}

void Motor_controller::run_one_motor_test(string motor_name, int16_t throttle)
{
    if (motor_name == "FL")
    {
        motor_drone_fl.sendThrottle(throttle);
        motor_drone_bl.sendThrottle(0);
        motor_drone_fr.sendThrottle(0);
        motor_drone_br.sendThrottle(0);
        motor_tl.sendThrottle(0);
        motor_tr.sendThrottle(0);
    }
    else if (motor_name == "BL")
    {
        motor_drone_fl.sendThrottle(0);
        motor_drone_bl.sendThrottle(throttle);
        motor_drone_fr.sendThrottle(0);
        motor_drone_br.sendThrottle(0);
        motor_tl.sendThrottle(0);
        motor_tr.sendThrottle(0);
    }
    else if (motor_name == "FR_1")
    {

        motor_drone_fl.sendThrottle(0);
        motor_drone_bl.sendThrottle(0);
        motor_drone_fr.sendThrottle(throttle);
        motor_drone_br.sendThrottle(0);
        motor_tl.sendThrottle(0);
        motor_tr.sendThrottle(0);
    }

    else if (motor_name == "BR")
    {
        motor_drone_fl.sendThrottle(0);
        motor_drone_bl.sendThrottle(0);
        motor_drone_fr.sendThrottle(0);
        motor_drone_br.sendThrottle(throttle);
        motor_tl.sendThrottle(0);
        motor_tr.sendThrottle(0);
    }
    else if (motor_name == "TL")
    {
        motor_drone_fl.sendThrottle(0);
        motor_drone_bl.sendThrottle(0);
        motor_drone_fr.sendThrottle(0);
        motor_drone_br.sendThrottle(0);
        motor_tl.sendThrottle(throttle);
        // motor_tr.sendThrottle(0);
    }
    else if (motor_name == "TR")
    {
        motor_drone_fl.sendThrottle(0);
        motor_drone_bl.sendThrottle(0);
        motor_drone_fr.sendThrottle(0);
        motor_drone_br.sendThrottle(0);
        // motor_tl.sendThrottle(0);
        motor_tr.sendThrottle(throttle);
    }
}

void Motor_controller::set_tank_motor_pwm(std::string motor_name, int pwm_value)
{
    if (pwm_value < 260)
        pwm_value = SBUS_MIN;
    pwm_value = map(pwm_value, SBUS_MIN, SBUS_MAX, 0, 255);

    if (motor_name == "TL")
    {
        analogWrite(MOTOR_PWM_TL, pwm_value);
    }
    else if (motor_name == "TR")
    {
        analogWrite(MOTOR_PWM_TR, pwm_value);
    }
    else
    {
        Serial.println("Error: Unknown motor name for PWM control");
    }
}