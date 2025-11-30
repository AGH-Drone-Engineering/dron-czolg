// lib/CoreLogic/SafetyController.h
#pragma once
#include "config.h"
#include "MotorOutputs.h"

/**
 * @brief Kontroler bezpieczeństwa
 *
 * Obsługuje sytuacje awaryjne:
 * - Failsafe (utrata sygnału RC)
 * - Safety land (stopniowe zmniejszanie throttle)
 */
class SafetyController
{
    bool failsafeActive = false;
    float reductionStep = SAFETY_LAND_REDUCTION_STEP;

public:
    /** @brief Ustaw krok redukcji throttle podczas safety land */
    void setReductionStep(float step) { reductionStep = step; }

    /**
     * @brief Wykonaj safety land - stopniowo zmniejsz throttle
     * @param outputs Wyjścia silników do modyfikacji
     * @param isTankMode true jeśli aktualny tryb to czołg
     */
    void safetyLand(MotorOutputs &outputs, bool isTankMode)
    {
        if (isTankMode)
        {
            // Redukuj silniki czołgu
            if (outputs.tl > 0)
                outputs.tl -= reductionStep;
            if (outputs.tr > 0)
                outputs.tr -= reductionStep;
            if (outputs.tl < 0)
                outputs.tl = 0;
            if (outputs.tr < 0)
                outputs.tr = 0;
        }
        else
        {
            // Redukuj silniki copter
            if (outputs.fl > 0)
                outputs.fl -= reductionStep;
            if (outputs.fr > 0)
                outputs.fr -= reductionStep;
            if (outputs.bl > 0)
                outputs.bl -= reductionStep;
            if (outputs.br > 0)
                outputs.br -= reductionStep;
            if (outputs.fl < 0)
                outputs.fl = 0;
            if (outputs.fr < 0)
                outputs.fr = 0;
            if (outputs.bl < 0)
                outputs.bl = 0;
            if (outputs.br < 0)
                outputs.br = 0;
        }
    }

    /** @brief Aktywuj failsafe */
    void activateFailsafe() { failsafeActive = true; }

    /** @brief Dezaktywuj failsafe */
    void deactivateFailsafe() { failsafeActive = false; }

    /** @brief Sprawdź czy failsafe jest aktywny */
    bool isFailsafeActive() const { return failsafeActive; }

    /**
     * @brief Sprawdź warunki failsafe i zaktualizuj stan
     * @param sbusFailsafe Czy SBUS zgłasza failsafe
     * @param sbusLostFrame Czy SBUS zgłasza utratę ramki
     * @return true jeśli failsafe jest aktywny
     */
    bool checkFailsafe(bool sbusFailsafe, bool sbusLostFrame)
    {
        if (sbusFailsafe)
        {
            activateFailsafe();
        }
        // Uwaga: lost frame nie aktywuje od razu failsafe
        // (może być chwilowa utrata)
        return failsafeActive;
    }
};
