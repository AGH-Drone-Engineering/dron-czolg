#include "mpu6050_sensor.h"

Mpu6050_Sensor::Mpu6050_Sensor() {}

void Mpu6050_Sensor::init()
{
    while (!mpu.begin())
    {
        Serial.println("Failed to found MPU6050 Chip, still looking...");
        delay(10);
    }

    // 2, 4, 8 or 16 G
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    // 250, 500, 1000, 2000 deg/s
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    // 260 (disabled), 184, 94, 44, 21, 10 or 5 Hz
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

    Serial.println("MPU6050 found!\n");
    delay(100);
}

float Mpu6050_Sensor::get_pitch() { return pitch_filtered; }
float Mpu6050_Sensor::get_pitch_rate() { return g.gyro.x; }
float Mpu6050_Sensor::get_roll();
{
    return roll_filtered;
}
float Mpu6050_Sensor::get_roll_rate();
{
    return g.gyro.y;
}
float Mpu6050_Sensor::get_yaw();
{
    return yaw_gyro;
}
float Mpu6050_Sensor::get_yaw_rate();
{
    return g.gyro.z;
}

sensors_event_t Mpu6050_Sensor::get_accel_event() { return a; }
sensors_event_t Mpu6050_Sensor::get_gyro_event() { return g; }
sensors_event_t Mpu6050_Sensor::get_temp_event() { return temp; }

void Mpu6050_Sensor::compute_orientation()
{
    mpu.getEvent(&a, &g, &temp);

    unsigned long current_time = millis();
    float dt = (current_time - last_time) / 1000.0; // ms -> s
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

void Mpu6050_Sensor::print_data()
{
    // Print orientation
    Serial.print("Pitch: ");
    Serial.print(pitch_filtered);
    Serial.print(" Roll: ");
    Serial.print(roll_filtered);
    Serial.print(" Yaw (Relative): ");
    Serial.print(yaw_gyro);
    Serial.print(" Yaw Rate: ");
    Serial.println(g.gyro.z);
}