#include "motor.h"

Motor_controller::Motor_controller(Sbus_reader &sbus_reader_)
    : sbus_reader_ref(sbus_reader_),
      motor_tl(MOTOR_PORT_TL, DShotType::DShot600),
      motor_tr(MOTOR_PORT_TR, DShotType::DShot600),
      motor_drone_fl(MOTOR_PORT_DRONE_FL_3, DShotType::DShot600),
      motor_drone_fr(MOTOR_PORT_DRONE_FR_1, DShotType::DShot600),
      motor_drone_bl(MOTOR_PORT_DRONE_BL_2, DShotType::DShot600),
      motor_drone_br(MOTOR_PORT_DRONE_BR_4, DShotType::DShot600),
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
    servo_left.attach();
    servo_right.attach();
    servo_left.set_servo_tank_mode();
    servo_right.set_servo_tank_mode();
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
    // Proste sterowanie tank (różnicowe) na surowych wartościach SBUS
    float throttle = sbus_reader_ref.get_throttle(); // SBUS_MIN do SBUS_MAX
    float yaw = sbus_reader_ref.get_yaw();           // SBUS_MIN do SBUS_MAX

    // Oblicz left i right na podstawie surowych wartości (bez wcześniejszej normalizacji)
    float left = throttle + (yaw - SBUS_CENTER) * 0.5f;
    float right = throttle - (yaw - SBUS_CENTER) * 0.5f;

    // Ogranicz do zakresu SBUS_MIN - SBUS_MAX
    left = constrain(left, SBUS_MIN, SBUS_MAX);
    right = constrain(right, SBUS_MIN, SBUS_MAX);

    // Znormalizuj do 0-1
    float left_norm = (left - SBUS_MIN) / (SBUS_MAX - SBUS_MIN);
    float right_norm = (right - SBUS_MIN) / (SBUS_MAX - SBUS_MIN);

    // Mapuj na zakres DShot
    tl = left_norm * (DSHOT_THROTTLE_ACTIVE_MAX - DSHOT_THROTTLE_ACTIVE_MIN) + DSHOT_THROTTLE_ACTIVE_MIN;
    tr = right_norm * (DSHOT_THROTTLE_ACTIVE_MAX - DSHOT_THROTTLE_ACTIVE_MIN) + DSHOT_THROTTLE_ACTIVE_MIN;

    // Jeśli throttle bardzo niski, wyłącz motory
    float throttle_norm = (throttle - SBUS_MIN) / (SBUS_MAX - SBUS_MIN);
    if (throttle_norm < 0.05f)
    {
        tl = 0;
        tr = 0;
    }
}

void Motor_controller::set_vehicle_PWM()
{
    motor_tl.sendThrottle(tl);
    motor_tr.sendThrottle(tr);
    if (millis() % 200 == 0)
    {
        printf("throttle left: %.2f, throttle right: %.2f, arm: %.2f, mode: %.2f\n",
               tl, tr,
               is_armed() ? 1.0f : 0.0f,
               sbus_reader_ref.get_mode());
    }
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
        motor_tr.sendThrottle(0);
    }
    else if (motor_name == "TR")
    {
        motor_drone_fl.sendThrottle(0);
        motor_drone_bl.sendThrottle(0);
        motor_drone_fr.sendThrottle(0);
        motor_drone_br.sendThrottle(0);
        motor_tl.sendThrottle(0);
        motor_tr.sendThrottle(throttle);
    }
}
