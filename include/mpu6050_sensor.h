#ifndef MPU6050SENSOR_H
#define MPU6050SENSOR_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

class Mpu6050_Sensor
{
private:
    Adafruit_MPU6050 mpu;
    sensors_event_t a, g, temp;

    unsigned long last_time = 0;
    float alpha = 0.98; // Complementary filter coefficient
    float pitch_acc, roll_acc;
    float pitch_gyro = 0, roll_gyro = 0, yaw_gyro = 0;
    float pitch_filtered = 0, roll_filtered = 0;

    // void configure();

public:
    Mpu6050_Sensor();
    void init();

    float get_pitch();
    float get_pitch_rate();
    float get_roll();
    float get_roll_rate();
    float get_yaw();
    float get_yaw_rate();

    sensors_event_t get_accel_event();
    sensors_event_t get_gyro_event();
    sensors_event_t get_temp_event();

    void compute_orientation();
    void print_data();
};

#endif // MPU6050SENSOR_H