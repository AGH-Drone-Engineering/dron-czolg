
#include "mpu6050_sensor.h"

Mpu6050Sensor::Mpu6050Sensor() {}

bool Mpu6050Sensor::begin() {
    if (!mpu.begin()) {
        return false;
    }
    configure();
    return true;
}

void Mpu6050Sensor::configure() {
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);  // 2, 4, 8, or 16 G
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);       // 250, 500, 1000, or 2000 deg/s
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);     // 260 (disabled), 184, 94, 44, 21, 10, or 5 Hz
}

void Mpu6050Sensor::computeOrientation() {
    mpu.getEvent(&a, &g, &temp);

    unsigned long current_time = millis();
    float dt = (current_time - last_time) / 1000.0;  // ms to s
    last_time = current_time;

    // Calculate pitch and roll from accelerometer
    pitch_acc = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;
    roll_acc = atan2(-a.acceleration.x, a.acceleration.z) * 180 / PI;

    // Integrate gyro data
    pitch_gyro += g.gyro.x * dt;
    roll_gyro += g.gyro.y * dt;
    yaw_gyro += g.gyro.z * dt;

    // Complementary filter
    pitch_filtered = alpha * pitch_gyro + (1 - alpha) * pitch_acc;
    roll_filtered = alpha * roll_gyro + (1 - alpha) * roll_acc;
}
