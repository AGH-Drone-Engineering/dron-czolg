#include "config.h"
#include "sbus_reader.h"
#include "motor.h"

#ifndef NATIVE_TESTING

Sbus_reader SBUS_Reader;
Motor_controller Motor_Controller(SBUS_Reader);

void setup()
{
    Serial.begin(115200);
    SBUS_Reader.init();
    Motor_Controller.init();
}

void loop()
{
    // Czytaj SBUS
    if (SBUS_Reader.Read_Sbus() != 0)
        return; // Pomiń jeśli brak sygnału

    // Arm/Disarm
    if (SBUS_Reader.get_arm_status() > 0.5f && !Motor_Controller.is_armed())
    {
        Motor_Controller.arm_motors();
    }
    else if (SBUS_Reader.get_arm_status() < 0.5f && Motor_Controller.is_armed())
    {
        Motor_Controller.disarm_motors();
    }

    // Aktualizuj tryb (serwa)
    Motor_Controller.update_mode(SBUS_Reader.get_mode());

    // Sterowanie tank
    Motor_Controller.update_motors();

    // Wyślij PWM do motorów
    Motor_Controller.set_vehicle_PWM();
}

#endif