// src/main.cpp
#include "config.h"
#include "SbusParser.h"
#include "TeensySbusReader.h"
#include "TeensyPidTimer.h"
#include "TeensyDShotDriver.h"
#include "ImuLogic.h"
#include "TeensyMpuSensor.h"
#include "ServoLogic.h"
#include "TeensyServoDriver.h"
#include "MotorOutput.h"
#include "FlightController.h"
#include <array>

// ============ Hardware Drivers ============
TeensySbusReader realReader(SBUS_INPUT);
TeensyPidTimer realTimer;
TeensyMpuSensor realSensor;
TeensyServoDriver realServoDriver;

// Drivery DShot dla 6 silników
#ifndef NATIVE_TESTING
TeensyDShotDriver motorDriverFL(*MOTOR_PORT_FL, DShotType::DShot600);
TeensyDShotDriver motorDriverFR(*MOTOR_PORT_FR, DShotType::DShot600);
TeensyDShotDriver motorDriverBL(*MOTOR_PORT_BL, DShotType::DShot600);
TeensyDShotDriver motorDriverBR(*MOTOR_PORT_BR, DShotType::DShot600);
TeensyDShotDriver motorDriverTL(*MOTOR_PORT_TL, DShotType::DShot600);
TeensyDShotDriver motorDriverTR(*MOTOR_PORT_TR, DShotType::DShot600);
#endif

// ============ Core Logic with DI ============
SbusParser parser(realReader);
ImuLogic imu(realSensor, realTimer);
ServoLogic servo(realServoDriver, SERVO_LEFT_PIN, SERVO_RIGHT_PIN);

// Tablica wskaźników do driverów silników
#ifndef NATIVE_TESTING
std::array<IMotorDriver*, 6> motorDrivers = {
    &motorDriverFL, &motorDriverFR, &motorDriverBL,
    &motorDriverBR, &motorDriverTL, &motorDriverTR
};
#else
std::array<IMotorDriver*, 6> motorDrivers = {
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
};
#endif

// Warstwa wyjściowa silników
MotorOutput motorOutput(motorDrivers);

// Główny kontroler lotu
FlightController flightController(imu, parser, servo, motorOutput, realTimer);

// ============ PID Gains Configuration (from config.h) ============
const PidGains yawRateGains   = {PID_YAW_RATE_KP, PID_YAW_RATE_KI, PID_YAW_RATE_KD, PID_YAW_RATE_MIN, PID_YAW_RATE_MAX};
const PidGains pitchRateGains = {PID_PITCH_RATE_KP, PID_PITCH_RATE_KI, PID_PITCH_RATE_KD, PID_PITCH_RATE_MIN, PID_PITCH_RATE_MAX};
const PidGains rollRateGains  = {PID_ROLL_RATE_KP, PID_ROLL_RATE_KI, PID_ROLL_RATE_KD, PID_ROLL_RATE_MIN, PID_ROLL_RATE_MAX};
const PidGains pitchAngleGains = {PID_PITCH_ANGLE_KP, PID_PITCH_ANGLE_KI, PID_PITCH_ANGLE_KD, PID_PITCH_ANGLE_MIN, PID_PITCH_ANGLE_MAX};
const PidGains rollAngleGains  = {PID_ROLL_ANGLE_KP, PID_ROLL_ANGLE_KI, PID_ROLL_ANGLE_KD, PID_ROLL_ANGLE_MIN, PID_ROLL_ANGLE_MAX};

// ============ Timing ============
uint32_t lastLoopTime = 0;

void setup()
{
    Serial.begin(115200);
    while (!Serial && millis() < SERIAL_TIMEOUT)
        ; // Czekaj na Serial

    Serial.println("=== Dron-Czolg Initializing ===");

    // Inicjalizacja Wire dla MPU6050
    Wire.begin();
    Wire.setClock(I2C_CLOCK_SPEED);

    // Inicjalizacja kontrolera lotu
    flightController.init();
    flightController.setPidGains(
        yawRateGains, pitchRateGains, rollRateGains,
        pitchAngleGains, rollAngleGains
    );
    Serial.println("Flight controller initialized");

    // Ustaw początkowy czas
    lastLoopTime = millis();

    Serial.println("=== Ready ===");
}

void loop()
{
    // Oblicz dt
    uint32_t currentTime = millis();
    float dt = (currentTime - lastLoopTime) / 1000.0f;
    lastLoopTime = currentTime;

    // Zabezpieczenie przed zbyt krótkim dt
    if (dt < 0.001f)
        return;

    // 1. Odczyt z radia
    parser.parse();

    // 2. Odczyt z IMU
    if (!imu.update())
    {
        Serial.println("IMU read failed!");
        // Kontynuuj mimo błędu - może się naprawić
    }

    // 3. Aktualizacja kontrolera lotu (cała logika sterowania)
    flightController.update(dt);

    // 4. Debug output (opcjonalnie)
    static uint32_t lastDebugTime = 0;
    if (currentTime - lastDebugTime > DEBUG_INTERVAL)
    {
        lastDebugTime = currentTime;
        
        if (flightController.isFailsafe())
        {
            Serial.println("FAILSAFE!");
        }
        else if (flightController.isInTransition())
        {
            Serial.println("TRANSITION...");
        }
        else
        {
            Serial.print("Mode: ");
            Serial.print(flightController.getCurrentMode() == MODE_TANK ? "TANK" : "COPTER");
            Serial.print(" Armed: ");
            Serial.println(flightController.isArmed() ? "YES" : "NO");
        }
    }

    // 5. Ogranicz częstotliwość pętli
    uint32_t loopDuration = millis() - currentTime;
    if (loopDuration < LOOP_DT)
    {
        delay(LOOP_DT - loopDuration);
    }
}