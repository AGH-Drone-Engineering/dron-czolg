// src/new_main.cpp
#include "SbusParser.h"
#include "TeensySbusReader.h"
#include "PidController.h"
#include "TeensyPidTimer.h"
#include "MotorLogic.h"
#include "TeensyDShotDriver.h"
#include "ImuLogic.h"
#include "TeensyMpuSensor.h"
#include "ServoLogic.h"
#include "TeensyServoDriver.h"
#include <array>

// Sprzęt
TeensySbusReader realReader(Serial5);
TeensyPidTimer realTimer;
TeensyDShotDriver realDriver(Serial1);
TeensyMpuSensor realSensor;
TeensyServoDriver realServoDriver;

// Logika z DI
SbusParser parser(realReader);
std::array<PidGains, 4> pidGains = {/* Twoje wartości */};
PidController pid(realTimer, pidGains);
MotorLogic motor(realDriver);
ImuLogic imu(realSensor);
ServoLogic servo(realServoDriver, SERVO_LEFT_PIN, SERVO_RIGHT_PIN); // Z config

void setup()
{
    // Inicjalizacja sprzętu
}

void loop()
{
    if (parser.parse() && imu.update())
    {
        std::array<float, 4> setpoints = {parser.getThrottle(), parser.getYaw(), parser.getPitch(), parser.getRoll()};
        std::array<float, 4> measurements = {0, imu.getYaw(), imu.getPitch(), imu.getRoll()};
        auto outputs = pid.compute(setpoints, measurements);
        motor.setThrottle(outputs[0]);
        servo.setAngle(outputs[1]); // Dla skrętu
    }
}