// lib/CoreLogic/MotorOutputs.h
#pragma once

/**
 * @brief Struktura przechowująca wartości wyjściowe dla wszystkich silników
 * 
 * Oddziela dane od logiki - miksery wypełniają tę strukturę,
 * a MotorOutput wysyła ją do ESC.
 */
struct MotorOutputs
{
    // Silniki quadcoptera
    float fl = 0.0f; // Front-Left
    float fr = 0.0f; // Front-Right
    float bl = 0.0f; // Back-Left
    float br = 0.0f; // Back-Right

    // Silniki czołgu (gąsienice)
    float tl = 0.0f; // Tank-Left
    float tr = 0.0f; // Tank-Right

    /** @brief Wyzeruj wszystkie wyjścia */
    void reset()
    {
        fl = fr = bl = br = tl = tr = 0.0f;
    }
};
