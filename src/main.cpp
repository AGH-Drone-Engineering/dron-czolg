#include "config.h"
#include "sbus_reader.h"
#include "motor.h"

Sbus_reader SBUS_Reader;
Motor_controller Motor_Controller(SBUS_Reader);
int first_lost_frame;

void setup()
{
    Serial.begin(115200);
    SBUS_Reader.init();
    Motor_Controller.init();
    first_lost_frame = 0;
}

void loop()
{
    if (SBUS_Reader.Read_Sbus() != 0.0)
    {
        // if (millis() - first_lost_frame > 1000)
        // {

        //     printf("lost signal\n");

        //     first_lost_frame = millis();

        //     return;
        // }
    }

    // first_lost_frame = millis();

    // if (millis() - first_lost_frame < 500)

    // {

    //     // printf("throttle: %.2f, arm: %.2f, mode: %.2f\n",

    //     //        SBUS_Reader.get_throttle(),

    //     //        SBUS_Reader.get_arm_status(),

    //     //        SBUS_Reader.get_mode());

    //     // // Arm/Disarm

    //     if (SBUS_Reader.get_arm_status() > 0.5f && !Motor_Controller.is_armed())

    //     {

    //         Motor_Controller.arm_motors();
    //     }

    //     else if (SBUS_Reader.get_arm_status() < 0.5f && Motor_Controller.is_armed())

    //     {

    //         Motor_Controller.disarm_motors();
    //     }
    // }

    if (Motor_Controller.get_current_mode() != SBUS_Reader.get_mode())
    {
        Motor_Controller.update_mode(SBUS_Reader.get_mode());
        printf("changing mode\n");
        delay(3000);
    }

    // // Sterowanie tank
    // if (Motor_Controller.get_current_mode() == MODE_TANK)
    // {
    //Motor_Controller.update_motors();
    Motor_Controller.set_vehicle_PWM(SBUS_Reader.get_throttle(), SBUS_Reader.get_yaw());

    //}

    // Motor_Controller.run_one_motor_test("TL", SBUS_Reader.get_throttle());
}
