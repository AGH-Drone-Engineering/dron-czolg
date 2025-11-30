#include <unity.h>
#include "Arduino.h" // Mock Arduino
#include "sbus_reader.h"
#include "SBUS.h"   // Mock SBUS
#include "config.h" // Dla stałych SBUS_MIN, SBUS_MAX, SBUS_CENTER

void setUp(void)
{
    // Reset mock states
    bfs::mock_sbus_read_return = true;
    bfs::mock_sbus_lost_frame = false;
    bfs::mock_sbus_failsafe = false;
    for (int i = 0; i < 16; ++i)
        bfs::mock_sbus_ch[i] = 0;
}

void tearDown(void)
{
    // Clean up after each test (jeśli potrzebne, ale setUp już resetuje)
}

void test_Read_Sbus_success(void)
{
    Sbus_reader reader;
    // Ustaw mock dane
    bfs::mock_sbus_ch[0] = 1000; // throttle
    bfs::mock_sbus_ch[1] = 1000; // yaw
    bfs::mock_sbus_ch[2] = 1000; // pitch
    bfs::mock_sbus_ch[3] = 1000; // roll
    bfs::mock_sbus_ch[4] = 1500; // arm (aktywny, > SBUS_CENTER=993)
    bfs::mock_sbus_ch[5] = 1500; // mode (aktywny, > SBUS_CENTER=993)

    float status = reader.Read_Sbus();
    TEST_ASSERT_EQUAL_FLOAT(0.0, status);

    // NormalizeChannel: constrain(1000, 175, 1811) = 1000
    TEST_ASSERT_FLOAT_WITHIN(0.01, 1000.0, reader.get_throttle());
    TEST_ASSERT_FLOAT_WITHIN(0.01, 1000.0, reader.get_yaw());
    TEST_ASSERT_FLOAT_WITHIN(0.01, 1000.0, reader.get_pitch());
    TEST_ASSERT_FLOAT_WITHIN(0.01, 1000.0, reader.get_roll());
    TEST_ASSERT_EQUAL_FLOAT(1.0, reader.get_arm_status()); // 1500 > 993
    TEST_ASSERT_EQUAL_FLOAT(1.0, reader.get_mode());       // 1500 > 993
}

// Dodaj nowy test dla edge case
void test_Read_Sbus_edge_case(void)
{
    Sbus_reader reader;
    bfs::mock_sbus_ch[0] = 0;    // throttle < SBUS_MIN
    bfs::mock_sbus_ch[1] = 2000; // yaw > SBUS_MAX
    bfs::mock_sbus_ch[4] = 993;  // arm == SBUS_CENTER (granica, powinna być 0.0)

    float status = reader.Read_Sbus();
    TEST_ASSERT_EQUAL_FLOAT(0.0, status);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 175.0, reader.get_throttle()); // constrain(0, 175, 1811) = 175
    TEST_ASSERT_FLOAT_WITHIN(0.01, 1811.0, reader.get_yaw());     // constrain(2000, 175, 1811) = 1811
    TEST_ASSERT_EQUAL_FLOAT(0.0, reader.get_arm_status());        // 993 not > 993
}

void test_Read_Sbus_lost_frame(void)
{
    Sbus_reader reader;
    bfs::mock_sbus_lost_frame = true;

    float status = reader.Read_Sbus();
    TEST_ASSERT_EQUAL_FLOAT(1.0, status);
}

void test_Read_Sbus_failsafe(void)
{
    Sbus_reader reader;
    bfs::mock_sbus_failsafe = true;

    float status = reader.Read_Sbus();
    TEST_ASSERT_EQUAL_FLOAT(2.0, status);
}

void test_Read_Sbus_no_data(void)
{
    Sbus_reader reader;
    bfs::mock_sbus_read_return = false;

    float status = reader.Read_Sbus();
    TEST_ASSERT_EQUAL_FLOAT(1.0, status);
}

void test_isChannelActive_active(void)
{
    Sbus_reader reader;
    bfs::mock_sbus_ch[4] = 1500; // arm > SBUS_CENTER

    reader.Read_Sbus();
    TEST_ASSERT_EQUAL_FLOAT(1.0, reader.get_arm_status());
}

void test_isChannelActive_inactive(void)
{
    Sbus_reader reader;
    bfs::mock_sbus_ch[4] = 500; // arm < SBUS_CENTER

    reader.Read_Sbus();
    TEST_ASSERT_EQUAL_FLOAT(0.0, reader.get_arm_status());
}

// Usunięto test_isChannelActive_invalid_channel (trudny do przetestowania bez dostępu do prywatnych metod)

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_Read_Sbus_success);
    RUN_TEST(test_Read_Sbus_lost_frame);
    RUN_TEST(test_Read_Sbus_failsafe);
    RUN_TEST(test_Read_Sbus_no_data);
    RUN_TEST(test_isChannelActive_active);
    RUN_TEST(test_isChannelActive_inactive);
    RUN_TEST(test_Read_Sbus_edge_case); // Nowy test
    return UNITY_END();
}