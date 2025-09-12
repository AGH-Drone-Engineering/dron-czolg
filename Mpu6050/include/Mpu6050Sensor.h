#ifndef MPU6050SENSOR_H
#define MPU6050SENSOR_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

class Mpu6050Sensor {
public:
    Mpu6050Sensor();
    bool begin();
    void read(sensors_event_t *a, sensors_event_t *g, sensors_event_t *temp);
private:
    Adafruit_MPU6050 mpu;
    void configure();
};

#endif // MPU6050SENSOR_H
