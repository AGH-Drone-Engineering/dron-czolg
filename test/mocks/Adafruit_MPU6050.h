#pragma once
class Adafruit_MPU6050
{
public:
    void begin() {}
    void getEvent(sensors_event_t *event) {}
};
struct sensors_event_t
{
    float acceleration[3] = {0};
    float gyro[3] = {0};
};