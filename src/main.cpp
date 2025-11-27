#include "config.h"
#include "sbus_reader.h"
#include "mpu6050_sensor.h"
#include "motor.h"
#include "pids3d.h"

Sbus_reader SBUS_Reader;
Mpu6050_Sensor MPU5060_Sensor;
Motor_controller Motor_Controller(SBUS_Reader);

unsigned long last_time_us, now_us;
float dt;
int sbus_status;

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        delay(10);

    MPU5060_Sensor.init();
    SBUS_Reader.init();
    Motor_Controller.init();

    last_time_us = micros();
}

void loop()
{
    now_us = micros();
    dt = (now_us - last_time_us) / 1000000.0f;
    if (dt <= 0)
        dt = 0.001f;
    last_time_us = now_us;

    if (SBUS_Reader.get_arm_status() > 0.5f)
    {
        Motor_Controller.arm_motors();
    }
    else
    {
        Motor_Controller.disarm_motors();
    }

    MPU5060_Sensor.compute_orientation();

    sbus_status = SBUS_Reader.Read_Sbus();
    switch (sbus_status)
    {
    case 2:
        if (millis() % 1000 < LOOP_DT)
            Serial.println("Failsafe active");
        Motor_Controller.safety_land();
        return;
        break;
    case 1:
        if (millis() % 1000 < LOOP_DT)
            Serial.println("Lost frame");
        // Motor_Controller.safety_land();
        return;
        break;
    case 0:
        // SBUS_Reader.print_data();
        break;
    }
    if (millis() % 1000 < LOOP_DT)
    {
        // limit update rate to 50 Hz for stability
        SBUS_Reader.print_data();
        return;
    }

    Motor_Controller.update_mode(SBUS_Reader.get_mode());

    Motor_Controller.update_motors(MPU5060_Sensor, dt);

    Motor_Controller.set_vehicle_PWM();

    // SBUS_Reader.print_data();
    // printing of PID data is in motor.cpp
    // MPU5060_Sensor.print_data();
}
