// lib/CoreLogic/ArmingManager.h
#pragma once
#include "config.h"
#include "MotorOutputs.h"

/**
 * @brief Zarządzanie uzbrajaniem/rozbrajaniem silników
 * 
 * Obsługuje logikę bezpieczeństwa związaną z arm/disarm:
 * - Sprawdza warunki pozwalające na uzbrojenie
 * - Blokuje uzbrojenie podczas tranzycji
 * - Wymaga niskiego throttle przed uzbrojeniem
 */
class ArmingManager
{
    bool armed = false;

public:
    /**
     * @brief Sprawdź czy można uzbroić silniki
     * @param throttle Aktualna wartość throttle
     * @param inTransition Czy trwa tranzycja trybu
     * @return true jeśli można uzbroić
     */
    bool canArm(float throttle, bool inTransition) const
    {
        // Można uzbroić tylko gdy:
        // 1. Throttle jest poniżej minimum aktywnego
        // 2. Nie jesteśmy w trakcie tranzycji
        return throttle < DSHOT_THROTTLE_ACTIVE_MIN && !inTransition;
    }

    /**
     * @brief Próbuj uzbroić silniki
     * @param throttle Aktualna wartość throttle
     * @param inTransition Czy trwa tranzycja trybu
     * @return true jeśli uzbrojono pomyślnie
     */
    bool arm(float throttle, bool inTransition)
    {
        if (canArm(throttle, inTransition))
        {
            armed = true;
            return true;
        }
        return false;
    }

    /** @brief Rozbrój silniki */
    void disarm()
    {
        armed = false;
    }

    /** @brief Sprawdź czy silniki są uzbrojone */
    bool isArmed() const { return armed; }

    /** @brief Ustaw stan uzbrojenia (używaj ostrożnie) */
    void setArmed(bool arm) { armed = arm; }
};
