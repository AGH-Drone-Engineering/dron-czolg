#pragma once
#include <stdint.h>
#include <iostream>
#include <cmath>
#include <algorithm>

// 1. Typy danych Arduino
typedef uint8_t byte;
typedef bool boolean;

// 2. Czas (sterowany przez nas w testach)
extern unsigned long _mock_millis;
inline unsigned long millis() { return _mock_millis; }
inline void delay(unsigned long ms) { _mock_millis += ms; }

// 3. Serial (wypisuje na konsolę PC)
class SerialMock
{
public:
    void begin(long) {}
    void print(const char *s) { std::cout << s; }
    void print(int n) { std::cout << n; }
    void print(float n) { std::cout << n; }
    void println(const char *s) { std::cout << s << std::endl; }
    void println(int n) { std::cout << n << std::endl; }
    void println(float n) { std::cout << n << std::endl; }
    void println() { std::cout << std::endl; }
};
static SerialMock Serial;

// 4. Funkcje matematyczne
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#define map(x, in_min, in_max, out_min, out_max) ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)