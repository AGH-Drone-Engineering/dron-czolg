#include "ImuLogic.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

bool ImuLogic::update()
{
    ImuData data;
    if (!sensor.readData(data))
        return false;

    // Oblicz dt (czas od ostatniego cyklu)
    uint32_t current_time = timer.getMillis();
    float dt = (current_time - last_time) / 1000.0f; // ms -> s

    // Zabezpieczenie przed pierwszym wywołaniem lub zbyt długim dt
    if (last_time == 0 || dt > 1.0f)
    {
        last_time = current_time;
        return true; // Pierwszy cykl - tylko inicjalizacja czasu
    }
    last_time = current_time;

    // Użyj przeliczonych danych float jeśli dostępne, inaczej przelicz surowe
    float accelX, accelY, accelZ;
    float gyroX, gyroY, gyroZ;

    // Sprawdź czy dane float są już przeliczone (niezerowe)
    if (data.accelXf != 0 || data.accelYf != 0 || data.accelZf != 0)
    {
        // Dane już przeliczone przez driver
        accelX = data.accelXf;
        accelY = data.accelYf;
        accelZ = data.accelZf;
        gyroX = data.gyroXf;
        gyroY = data.gyroYf;
        gyroZ = data.gyroZf;
    }
    else
    {
        // Przelicz surowe dane (zakładając MPU6050 z konfigurowalnymi zakresami)
        accelX = data.accelX / IMU_ACCEL_SCALE;
        accelY = data.accelY / IMU_ACCEL_SCALE;
        accelZ = data.accelZ / IMU_ACCEL_SCALE;

        // Żyroskop: skala z config.h
        gyroX = data.gyroX / IMU_GYRO_SCALE; // °/s
        gyroY = data.gyroY / IMU_GYRO_SCALE;
        gyroZ = data.gyroZ / IMU_GYRO_SCALE;
    }

    // Zapisz prędkości kątowe (rate) - bezpośrednio z żyroskopu
    pitch_rate = gyroX;
    roll_rate = gyroY;
    yaw_rate = gyroZ;

    // Oblicz kąty z akcelerometru (w stopniach)
    // pitch_acc = atan2(y, z) * 180/PI - zgodne ze starym kodem
    pitch_acc = atan2(accelY, accelZ) * 180.0f / M_PI;
    roll_acc = atan2(-accelX, accelZ) * 180.0f / M_PI;

    // Integruj dane z żyroskopu
    pitch_gyro += gyroX * dt;
    roll_gyro += gyroY * dt;
    yaw_gyro += gyroZ * dt; // Yaw tylko z żyroskopu (dryfuje z czasem)

    // Filtr komplementarny - łączy szybką reakcję żyroskopu z stabilnością akcelerometru
    // alpha bliskie 1 = więcej żyroskopu (szybsza reakcja)
    // alpha bliskie 0 = więcej akcelerometru (mniej dryfu)
    pitch_filtered = alpha * pitch_gyro + (1.0f - alpha) * pitch_acc;
    roll_filtered = alpha * roll_gyro + (1.0f - alpha) * roll_acc;

    // Synchronizuj żyroskop z filtrem (zapobiega dryfowi integracji)
    pitch_gyro = pitch_filtered;
    roll_gyro = roll_filtered;

    return true;
}