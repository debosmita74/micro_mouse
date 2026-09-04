/**
 * @file    test_pid.c
 * @brief   Unit tests for PID controller.
 *
 * Compile: gcc -Wall -Wextra -Iinclude tests/test_pid.c src/pid.c -o test_pid -lm
 * Run:     ./test_pid
 */

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "pid.h"

#define ASSERT_NEAR(a, b, tol) \
    assert(fabs((double)(a) - (double)(b)) < (double)(tol))

static void test_pid_init(void)
{
    PidController pid;
    pid_init(&pid, 1.0f, 0.1f, 0.5f, -100.0f, 100.0f);

    ASSERT_NEAR(pid.kp, 1.0f, 0.001f);
    ASSERT_NEAR(pid.ki, 0.1f, 0.001f);
    ASSERT_NEAR(pid.kd, 0.5f, 0.001f);
    ASSERT_NEAR(pid.integral, 0.0f, 0.001f);
    ASSERT_NEAR(pid.prev_error, 0.0f, 0.001f);

    printf("  [PASS] test_pid_init\n");
}

static void test_pid_proportional(void)
{
    PidController pid;
    pid_init(&pid, 2.0f, 0.0f, 0.0f, -1000.0f, 1000.0f);

    float output = pid_compute(&pid, 100.0f, 80.0f, 0.001f);
    /* error = 20, P = 2.0 * 20 = 40 */
    ASSERT_NEAR(output, 40.0f, 1.0f);

    printf("  [PASS] test_pid_proportional\n");
}

static void test_pid_clamping(void)
{
    PidController pid;
    pid_init(&pid, 100.0f, 0.0f, 0.0f, -50.0f, 50.0f);

    float output = pid_compute(&pid, 100.0f, 0.0f, 0.001f);
    /* error = 100, P = 10000, should clamp to 50 */
    ASSERT_NEAR(output, 50.0f, 0.001f);

    printf("  [PASS] test_pid_clamping\n");
}

static void test_pid_reset(void)
{
    PidController pid;
    pid_init(&pid, 1.0f, 1.0f, 1.0f, -100.0f, 100.0f);

    pid_compute(&pid, 50.0f, 0.0f, 0.01f);
    pid_reset(&pid);

    ASSERT_NEAR(pid.integral, 0.0f, 0.001f);
    ASSERT_NEAR(pid.prev_error, 0.0f, 0.001f);

    printf("  [PASS] test_pid_reset\n");
}

static void test_pid_zero_error(void)
{
    PidController pid;
    pid_init(&pid, 1.0f, 0.0f, 0.0f, -100.0f, 100.0f);

    float output = pid_compute(&pid, 50.0f, 50.0f, 0.001f);
    ASSERT_NEAR(output, 0.0f, 0.001f);

    printf("  [PASS] test_pid_zero_error\n");
}

int main(void)
{
    printf("Running PID tests...\n");

    test_pid_init();
    test_pid_proportional();
    test_pid_clamping();
    test_pid_reset();
    test_pid_zero_error();

    printf("All PID tests passed!\n");
    return 0;
}
