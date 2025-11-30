// lib/CoreLogic/FlightController.h
#pragma once

#include "ImuLogic.h"
#include "SbusParser.h"
#include "ServoLogic.h"
#include "MotorOutput.h"
#include "MotorOutputs.h"
#include "StabilizationController.h"
#include "ModeManager.h"
#include "ArmingManager.h"
#include "SafetyController.h"
#include "TankMixer.h"
#include "CopterMixer.h"
#include "config.h"

/**
 * @brief Główny kontroler lotu - orkiestrator systemu
 * 
 * Łączy wszystkie komponenty i zarządza główną pętlą sterowania.
 * Deleguje zadania do wyspecjalizowanych komponentów:
 * - ModeManager: zarządzanie trybami
 * - ArmingManager: arm/disarm
 * - SafetyController: failsafe
 * - StabilizationController: PID
 * - Mixery: przeliczanie na silniki
 * - MotorOutput: wysyłanie do ESC
 */
class FlightController
{
    // Komponenty wejściowe (DI)
    ImuLogic &imu;
    SbusParser &sbus;
    ServoLogic &servos;
    MotorOutput &motorOutput;
    IPidTimer &timer;

    // Komponenty wewnętrzne
    ModeManager modeManager;
    ArmingManager armingManager;
    SafetyController safetyController;
    StabilizationController stabilization;
    
    // Miksery
    TankMixer tankMixer;
    CopterMixer copterMixer;

    // Stan
    MotorOutputs outputs;

public:
    FlightController(
        ImuLogic &imuRef,
        SbusParser &sbusRef,
        ServoLogic &servoRef,
        MotorOutput &motorRef,
        IPidTimer &timerRef)
        : imu(imuRef),
          sbus(sbusRef),
          servos(servoRef),
          motorOutput(motorRef),
          timer(timerRef),
          modeManager(timerRef, servoRef)
    {
    }

    /** @brief Inicjalizacja kontrolera */
    void init()
    {
        servos.attach();
        motorOutput.reset();
        modeManager.reset();
        stabilization.reset();
        outputs.reset();
    }

    /** @brief Ustaw wzmocnienia PID */
    void setPidGains(
        const PidGains &yawRate,
        const PidGains &pitchRate,
        const PidGains &rollRate,
        const PidGains &pitchAngle,
        const PidGains &rollAngle)
    {
        stabilization.setInnerGains(yawRate, pitchRate, rollRate);
        stabilization.setOuterGains(pitchAngle, rollAngle);
    }

    /**
     * @brief Główna funkcja aktualizacji - wywołuj w loop()
     * @param dt Czas od ostatniego wywołania [s]
     */
    void update(float dt)
    {
        // 1. Sprawdź failsafe
        if (safetyController.checkFailsafe(sbus.isFailsafe(), sbus.isLostFrame()))
        {
            handleFailsafe();
            return;
        }
        safetyController.deactivateFailsafe();

        // 2. Aktualizacja trybu
        bool modeChanged = modeManager.update(sbus.getMode());
        if (modeChanged)
        {
            stabilization.reset();
        }

        // 3. Obsługa arm/disarm
        handleArming();

        // 4. Jeśli w tranzycji - wyłącz silniki
        if (modeManager.isInTransition())
        {
            outputs.reset();
            sendOutputs();
            return;
        }

        // 5. Oblicz sterowanie
        computeControl(dt);

        // 6. Wyślij do silników
        sendOutputs();
    }

    // ============ Gettery stanu ============
    bool isArmed() const { return armingManager.isArmed(); }
    vehicle_mode_t getCurrentMode() const { return modeManager.getCurrentMode(); }
    bool isInTransition() const { return modeManager.isInTransition(); }
    bool isFailsafe() const { return safetyController.isFailsafeActive(); }
    const MotorOutputs &getOutputs() const { return outputs; }

    // ============ Dostęp do komponentów (do debugowania) ============
    const StabilizationController &getStabilization() const { return stabilization; }
    const ModeManager &getModeManager() const { return modeManager; }

private:
    void handleFailsafe()
    {
        bool isTank = (modeManager.getCurrentMode() == MODE_TANK);
        safetyController.safetyLand(outputs, isTank);
        sendOutputs();
    }

    void handleArming()
    {
        float throttle = sbus.getThrottle();
        bool inTransition = modeManager.isInTransition();

        if (sbus.getArmStatus() > 0.5f)
        {
            if (!armingManager.isArmed())
            {
                if (armingManager.arm(throttle, inTransition))
                {
                    stabilization.reset();
                    motorOutput.sendMinThrottle();
                }
            }
        }
        else
        {
            if (armingManager.isArmed())
            {
                armingManager.disarm();
                outputs.reset();
                motorOutput.reset();
            }
        }
    }

    void computeControl(float dt)
    {
        MixerInput mixerInput;

        if (modeManager.getCurrentMode() == MODE_TANK)
        {
            // Tryb czołgu
            float throttle = sbus.getThrottle() * THROTTLE_COEF_TANK;
            float yaw = sbus.getYaw() * STEER_COEF;

            mixerInput = stabilization.computeTank(imu, throttle, yaw, dt);
            tankMixer.mix(mixerInput, outputs);
        }
        else // MODE_COPTER
        {
            // Tryb copter
            float throttle = sbus.getThrottle() * THROTTLE_COEF_COPTER;
            float yaw = sbus.getYaw() * YAW_RATE_COEF;
            float pitch = sbus.getPitch() * PITCH_COEF;
            float roll = sbus.getRoll() * ROLL_COEF;

            mixerInput = stabilization.computeCopter(imu, throttle, yaw, pitch, roll, dt);
            copterMixer.mix(mixerInput, outputs);
        }
    }

    void sendOutputs()
    {
        if (armingManager.isArmed())
        {
            motorOutput.send(outputs);
        }
        else
        {
            motorOutput.reset();
        }
    }
};
