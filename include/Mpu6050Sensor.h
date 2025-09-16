#ifndef MPU6050SENSOR_H
#define MPU6050SENSOR_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>


class Mpu6050Sensor {
public:
    Mpu6050Sensor();
    bool begin();
    void computeOrientation(); // Reads sensors and computes orientation
    
    float getPitch() const { return pitch_acc; }
    float getRoll() const { return roll_acc; }
    sensors_event_t getAccelEvent() const { return a; }
    sensors_event_t getGyroEvent() const { return g; }
    sensors_event_t getTempEvent() const { return temp; }
private:
    Adafruit_MPU6050 mpu;
    void configure();
    float pitch_acc, roll_acc;
    sensors_event_t a, g, temp;
};

#endif // MPU6050SENSOR_H
