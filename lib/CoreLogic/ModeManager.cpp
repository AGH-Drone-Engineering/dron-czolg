// lib/CoreLogic/ModeManager.cpp
#include "ModeManager.h"

bool ModeManager::update(float modeSwitch)
{
    vehicle_mode_t desiredMode = (modeSwitch > 0.5f) ? MODE_COPTER : MODE_TANK;

    if (currentMode == MODE_TRANSITION)
    {
        // Pierwsze wywołanie po starcie - ustaw tryb natychmiast
        if (!initialized || transitionStartTime == 0)
        {
            currentMode = desiredMode;
            targetMode = desiredMode;
            initialized = true;

            // Ustaw serwa
            if (currentMode == MODE_COPTER)
            {
                servos.setCopterMode();
            }
            else
            {
                servos.setTankMode();
            }
            return true;
        }

        // Sprawdź czy minął czas tranzycji
        if (timer.getMillis() - transitionStartTime >= TRANSITION_TIME)
        {
            currentMode = targetMode;
            return true; // Zakończono tranzycję
        }
    }
    else
    {
        // Normalna praca - sprawdź czy trzeba zmienić tryb
        if (desiredMode != currentMode && desiredMode != targetMode)
        {
            currentMode = MODE_TRANSITION;
            targetMode = desiredMode;
            transitionStartTime = timer.getMillis();

            // Rozpocznij ruch serw
            if (targetMode == MODE_COPTER)
            {
                servos.setCopterMode();
            }
            else
            {
                servos.setTankMode();
            }
            return true; // Rozpoczęto tranzycję
        }
    }

    return false; // Brak zmiany
}

void ModeManager::forceMode(vehicle_mode_t mode)
{
    currentMode = mode;
    targetMode = mode;
    transitionStartTime = 0;
    initialized = true;

    if (mode == MODE_COPTER)
    {
        servos.setCopterMode();
    }
    else if (mode == MODE_TANK)
    {
        servos.setTankMode();
    }
}

void ModeManager::reset()
{
    currentMode = MODE_TRANSITION;
    targetMode = MODE_TANK;
    transitionStartTime = 0;
    initialized = false;
}
