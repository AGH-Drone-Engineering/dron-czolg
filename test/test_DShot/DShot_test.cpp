// do poprawy calosc
#include <unity.h>
#include "Arduino.h" // Mock Arduino
#include "DShot.h"   // Mock DShot
#include "config.h"

// Funkcje z DShot.cpp, które możemy testować (statyczne lub publiczne)
static inline uint16_t createPackage(uint16_t value, bool telemetry)
{
    value = (value << 1) | telemetry;
    return (value << 4) | ((value ^ (value >> 4) ^ (value >> 8)) & 0x0F);
}

int16_t convertThrottle(float throttle)
{
    throttle = constrain(throttle, 48.0f, 2047.0f);
    return static_cast<uint16_t>(roundf(throttle));
}

void setUp(void)
{
    // Reset przed każdym testem
}

void tearDown(void)
{
    // Czyszczenie po teście
}

void test_createPackage_no_telemetry(void)
{
    uint16_t package = createPackage(100, false);
    // Sprawdź, czy pakiet jest poprawny (bez telemetry, z CRC)
    // Dla value=100, telemetry=0: value << 1 = 200, potem << 4 = 3200, CRC = (200 ^ 12 ^ 0) & 0x0F = 4
    // Oczekiwany: 3200 | 4 = 3204
    TEST_ASSERT_EQUAL(3204, package);
}

void test_createPackage_with_telemetry(void)
{
    uint16_t package = createPackage(100, true);
    // Z telemetry: value = 200 | 1 = 201, potem << 4 = 3216, CRC = (201 ^ 12 ^ 0) & 0x0F = 5
    // Oczekiwany: 3216 | 5 = 3221
    TEST_ASSERT_EQUAL(3221, package);
}

void test_convertThrottle_within_range(void)
{
    int16_t result = convertThrottle(500.7f);
    TEST_ASSERT_EQUAL(501, result); // roundf(500.7) = 501
}

void test_convertThrottle_below_min(void)
{
    int16_t result = convertThrottle(30.0f);
    TEST_ASSERT_EQUAL(48, result); // constrain do 48
}

void test_convertThrottle_above_max(void)
{
    int16_t result = convertThrottle(2500.0f);
    TEST_ASSERT_EQUAL(2047, result); // constrain do 2047
}

void test_sendThrottle_calls_sendPackage(void)
{
    DShot dshot(nullptr, DShotType::DShot600);
    dshot.sendThrottle(150, false);

    TEST_ASSERT_TRUE(dshot.sendThrottle_called);
    TEST_ASSERT_EQUAL(150, dshot.last_sent_throttle);
    TEST_ASSERT_FALSE(dshot.last_telemetry);
}

void test_sendCommand_calls_sendPackage(void)
{
    DShot dshot(nullptr, DShotType::DShot600);
    dshot.sendCommand(10, true);

    TEST_ASSERT_TRUE(dshot.sendCommand_called);
    TEST_ASSERT_EQUAL(10, dshot.last_sent_command);
    TEST_ASSERT_TRUE(dshot.last_telemetry);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_createPackage_no_telemetry);
    RUN_TEST(test_createPackage_with_telemetry);
    RUN_TEST(test_convertThrottle_within_range);
    RUN_TEST(test_convertThrottle_below_min);
    RUN_TEST(test_convertThrottle_above_max);
    RUN_TEST(test_sendThrottle_calls_sendPackage);
    RUN_TEST(test_sendCommand_calls_sendPackage);
    return UNITY_END();
}