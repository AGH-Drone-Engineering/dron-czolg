#include "ImuLogic.h"
#include <cmath> // Dla atan2 itp.

bool ImuLogic::update()
{
    ImuData data;
    if (!sensor.readData(data))
        return false;

    // Prosta filtracja (zastąp swoim kodem z mpu6050_sensor.cpp)
    float accelX = data.accelX / 16384.0f; // Skalowanie
    float accelY = data.accelY / 16384.0f;
    float accelZ = data.accelZ / 16384.0f;

    pitch = atan2(accelY, sqrt(accelX * accelX + accelZ * accelZ)) * 180 / M_PI;
    roll = atan2(-accelX, accelZ) * 180 / M_PI;
    // Yaw z żyroskopu (uprość)
    yaw += data.gyroZ / 131.0f * 0.01f; // DT = 0.01s

    return true;
}