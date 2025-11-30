#include <unity.h>
#include "Pid.h"

void setUp(void) {}
void tearDown(void) {}

void test_pid_proportional_only(void)
{
    Pid pid;
    pid.setGains({1.0f, 0.0f, 0.0f, -100.0f, 100.0f}); // Kp=1, Ki=0, Kd=0

    float output = pid.compute(0.0f, 10.0f, 0.01f); // setpoint=0, measurement=10

    TEST_ASSERT_FLOAT_WITHIN(0.01f, -10.0f, output); // error = 0 - 10 = -10
}

void test_pid_integral_accumulates(void)
{
    Pid pid;
    pid.setGains({0.0f, 1.0f, 0.0f, -100.0f, 100.0f}); // Ki=1

    pid.compute(10.0f, 0.0f, 1.0f);                // error=10, dt=1s -> integral=10
    float output = pid.compute(10.0f, 0.0f, 1.0f); // integral=20

    TEST_ASSERT_FLOAT_WITHIN(0.01f, 20.0f, output);
}

void test_pid_derivative(void)
{
    Pid pid;
    pid.setGains({0.0f, 0.0f, 1.0f, -100.0f, 100.0f}); // Kd=1

    pid.compute(0.0f, 0.0f, 0.01f);                 // pierwszy pomiar
    float output = pid.compute(0.0f, 10.0f, 0.01f); // error zmienił się o -10

    // derivative = (0 - (-10)) / 0.01 = 1000, ale Kd=1 więc -1000
    TEST_ASSERT_TRUE(output < 0); // kierunek powinien być ujemny
}

void test_pid_output_clamping(void)
{
    Pid pid;
    pid.setGains({100.0f, 0.0f, 0.0f, -50.0f, 50.0f}); // limity ±50

    float output = pid.compute(0.0f, 100.0f, 0.01f); // error=-100, P=-10000

    TEST_ASSERT_FLOAT_WITHIN(0.01f, -50.0f, output); // ograniczone do -50
}

void test_pid_anti_windup(void)
{
    Pid pid;
    pid.setGains({0.0f, 10.0f, 0.0f, -50.0f, 50.0f}); // Ki=10, limity ±50

    // Akumuluj integral przez wiele iteracji
    for (int i = 0; i < 100; i++)
    {
        pid.compute(100.0f, 0.0f, 0.1f);
    }

    float output = pid.compute(100.0f, 0.0f, 0.1f);

    TEST_ASSERT_FLOAT_WITHIN(0.01f, 50.0f, output); // nie przekracza limitu
}

void test_pid_reset(void)
{
    Pid pid;
    pid.setGains({0.0f, 1.0f, 0.0f, -100.0f, 100.0f});

    pid.compute(10.0f, 0.0f, 1.0f); // integral = 10
    pid.reset();
    float output = pid.compute(10.0f, 0.0f, 1.0f); // po resecie integral = 10

    TEST_ASSERT_FLOAT_WITHIN(0.01f, 10.0f, output); // zaczyna od nowa
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_pid_proportional_only);
    RUN_TEST(test_pid_integral_accumulates);
    RUN_TEST(test_pid_derivative);
    RUN_TEST(test_pid_output_clamping);
    RUN_TEST(test_pid_anti_windup);
    RUN_TEST(test_pid_reset);

    return UNITY_END();
}