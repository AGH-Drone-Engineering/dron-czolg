#include "config.h"
#include "sbus_receiver.h"
#include "mpu6050_sensor.h"
#include "motor.h"
#include "pids3d.h"

Sbus_reader SBUS_Reader;
Mpu6050_Sensor MPU5060_Sensor;
Motor_controller Motor_Controller;

vehicle_mode_t mode;
unsigned long last_time_us, now_us;
float dt;
int sbus_status;

void setup()
{
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

        while (!Serial)
            delay(10);

        MPU5060_Sensor.init();
        SBUS_Reader.init();
        Motor_Controller.init();

        mode = DEFAULT_MODE;
        if (mode == MODE_TANK)
        {
            // change to tank
        }
        else
        {
            // change to copter
        }

        last_time_us = micros();
    }

    void loop()
    {
        now_us = micros();
        void loop()
        {
            now_us = micros();
            dt = (now_us - last_time_us) / 1000000.0f;
            if (dt <= 0)
                dt = 0.001f;
            if (dt <= 0)
                dt = 0.001f;
            last_time_us = now_us;

            MPU5060_Sensor.compute_orientation();
            MPU5060_Sensor.compute_orientation();

            sbus_status = SBUS_Reader.Read_Sbus();
            // TODO handle safe mode
            switch (sbus_status)
            {
            case 2:
                Serial.println("Failsafe active");
                return;
                break;
            case 1:
                Serial.println("Signal lost");
                sbus_status = SBUS_Reader.Read_Sbus();
                // TODO handle safe mode
                switch (sbus_status)
                {
                case 2:
                    Serial.println("Failsafe active");
                    return;
                    break;
                case 1:
                    Serial.println("Signal lost");
                    return;
                    break;
                case 0:
                    SBUS_Reader.print_data();
                    break;
                    break;
                case 0:
                    SBUS_Reader.print_data();
                    break;
                }

                Motor_Controller.update_mode(SBUS_Reader.get_mode());

                Motor_Controller.update_motors(SBUS_Reader.get_data(), MPU5060_Sensor, dt);

                Motor_Controller.set_vehicle_PWM();

                SBUS_Reader.print_data();
                // printing of PID data is in motor.cpp
                MPU5060_Sensor.print_data();
            }