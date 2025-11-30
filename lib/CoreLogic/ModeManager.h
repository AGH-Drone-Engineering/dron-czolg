// lib/CoreLogic/ModeManager.h
#pragma once
#include "config.h"
#include "IPidTimer.h"
#include "ServoLogic.h"

/**
 * @brief Zarządzanie trybami pracy pojazdu (czołg/copter)
 * 
 * Obsługuje przełączanie między trybami, tranzycje
 * i synchronizację z serwomechanizmami transformacji.
 */
class ModeManager
{
    IPidTimer &timer;
    ServoLogic &servos;

    vehicle_mode_t currentMode = MODE_TANK;
    vehicle_mode_t targetMode = MODE_TANK;
    uint32_t transitionStartTime = 0;
    bool initialized = false;

public:
    ModeManager(IPidTimer &t, ServoLogic &srv)
        : timer(t), servos(srv) {}

    /**
     * @brief Aktualizuj tryb na podstawie przełącznika
     * @param modeSwitch Wartość z radia (0 = tank, >0.5 = copter)
     * @return true jeśli zmieniono tryb lub zakończono tranzycję
     */
    bool update(float modeSwitch);

    /** @brief Pobierz aktualny tryb */
    vehicle_mode_t getCurrentMode() const { return currentMode; }

    /** @brief Pobierz docelowy tryb (różny od current podczas tranzycji) */
    vehicle_mode_t getTargetMode() const { return targetMode; }

    /** @brief Sprawdź czy trwa tranzycja */
    bool isInTransition() const { return currentMode == MODE_TRANSITION; }

    /** @brief Wymuś natychmiastową zmianę trybu (bez tranzycji) */
    void forceMode(vehicle_mode_t mode);

    /** @brief Resetuj do stanu początkowego */
    void reset();
};
