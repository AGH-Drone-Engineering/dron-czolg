// #include "config.h"
// #include "sbus_reader.h"
// #include "mpu6050_sensor.h"
// #include "motor.h"
// #include "pids3d.h"

// Sbus_reader SBUS_Reader;
// Mpu6050_Sensor MPU5060_Sensor;
// Motor_controller Motor_Controller;

// vehicle_mode_t mode;
// unsigned long last_time_us, now_us;
// float dt;
// int sbus_status;

// void setup()
// {
//     Serial.begin(115200);
//     while (!Serial)
//         delay(10);

//     MPU5060_Sensor.init();
//     SBUS_Reader.init();
//     Motor_Controller.init();

//     mode = DEFAULT_MODE;

//     last_time_us = micros();
// }

// void loop()
// {
//     now_us = micros();
//     dt = (now_us - last_time_us) / 1000000.0f;
//     if (dt <= 0)
//         dt = 0.001f;
//     last_time_us = now_us;

//     MPU5060_Sensor.compute_orientation();

//     sbus_status = SBUS_Reader.Read_Sbus();
//     switch (sbus_status)
//     {
//     case 2:
//         Serial.println("Failsafe active");
//         Motor_Controller.safety_land();
//         return;
//         break;
//     case 1:
//         Serial.println("Signal lost");
//         Motor_Controller.safety_land();
//         return;
//         break;
//     case 0:
//         SBUS_Reader.print_data();
//         break;
//     }

//     Motor_Controller.update_mode(SBUS_Reader.get_mode());

//     Motor_Controller.update_motors(SBUS_Reader.get_data(), MPU5060_Sensor, dt);

//     Motor_Controller.set_vehicle_PWM();

//     SBUS_Reader.print_data();
//     // printing of PID data is in motor.cpp
//     MPU5060_Sensor.print_data();
// }

#include "config.h"
#include "sbus_reader.h"

Sbus_reader sbus;

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        delay(10);

    sbus.init();

    Serial.println("Dron-Czolg setup complete.");
}

void loop()
{
    int status = sbus.Read_Sbus();
    if (status == 0)
    {
        sbus.print_data();
    }
    else if (status == 1)
    {
        Serial.println("SBUS Signal Lost");
    }
    else if (status == 2)
    {
        Serial.println("SBUS Failsafe Activated");
    }

    delay(100);
}