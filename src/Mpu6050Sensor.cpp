#include "Mpu6050Sensor.h"


Mpu6050Sensor::Mpu6050Sensor() {}

bool Mpu6050Sensor::begin() {
    if (!mpu.begin()) {
        return false;
    }
    configure();
    return true;
}

void Mpu6050Sensor::configure() {
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G); // 2, 4, 8, or 16 G
    mpu.setGyroRange(MPU6050_RANGE_500_DEG); // 250, 500, 1000, or 2000 deg/s
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ); // 260 (disabled), 184, 94, 44, 21, 10, or 5 Hz
}

void Mpu6050Sensor::read(sensors_event_t *a, sensors_event_t *g, sensors_event_t *temp) {
    mpu.getEvent(a, g, temp);
}
