#ifndef MPU6050SENSOR_H
#define MPU6050SENSOR_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

class Mpu6050Sensor {
   public:
    Mpu6050Sensor();
    bool begin();
    void computeOrientation();  // Reads sensors and computes orientation

    float getPitch() const { return pitch_filtered; }
    float getRoll() const { return roll_filtered; }
    float getPitchRate() const { return g.gyro.x; }
    float getRollRate() const { return g.gyro.y; }
    float getYaw() const { return yaw_gyro; }
    float getYawRate() const { return g.gyro.z; }  // Yaw rate from gyroscope
    sensors_event_t getAccelEvent() const { return a; }
    sensors_event_t getGyroEvent() const { return g; }
    sensors_event_t getTempEvent() const { return temp; }

   private:
    Adafruit_MPU6050 mpu;
    sensors_event_t a, g, temp;
    void configure();
    unsigned long last_time = 0;
    float alpha = 0.98;  // Complementary filter coefficient
    float pitch_acc, roll_acc;
    float pitch_gyro = 0, roll_gyro = 0, yaw_gyro = 0;
    float pitch_filtered = 0, roll_filtered = 0;
};

#endif  // MPU6050SENSOR_H
