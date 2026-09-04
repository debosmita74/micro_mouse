/**
 * @file    main.c
 * @brief   Micromouse entry point and state machine.
 * @author  Debosmita Paul
 * @date    2026-09-04
 *
 * Implements the top-level state machine:
 *   IDLE → CALIBRATION → SEARCH_RUN → RETURN_TO_START → SPEED_RUN → FINISHED
 */

#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "motor.h"
#include "encoder.h"
#include "sensor.h"
#include "imu.h"
#include "pid.h"
#include "maze.h"
#include "motion.h"
#include "battery.h"

/* ── Robot States ───────────────────────────────────────── */
typedef enum {
    STATE_IDLE,
    STATE_CALIBRATION,
    STATE_SEARCH_RUN,
    STATE_RETURN_TO_START,
    STATE_SPEED_RUN,
    STATE_ERROR,
    STATE_FINISHED
} RobotState;

/* ── Global State ───────────────────────────────────────── */
static volatile RobotState  robot_state = STATE_IDLE;
static Pose                 robot_pose;
static SensorData           sensor_data;
static ImuData              imu_data;

/* ── PID Controllers ────────────────────────────────────── */
static PidController pid_left_speed;
static PidController pid_right_speed;
static PidController pid_wall;

/* ── Forward Declarations ───────────────────────────────── */
static void system_init(void);
static void state_idle(void);
static void state_calibration(void);
static void state_search_run(void);
static void state_return_to_start(void);
static void state_speed_run(void);
static void state_error(void);
static bool button_pressed(void);
static void led_on(void);
static void led_off(void);
static void led_blink(uint32_t count);
static void delay_ms(uint32_t ms);

/* ── Placeholder HAL stubs (replace with actual HAL) ──── */
/* These would be provided by STM32 HAL or your own drivers */
void HAL_Init(void) { /* STM32 HAL initialization */ }
void SystemClock_Config(void) { /* Configure 72MHz clock */ }

/**
 * @brief  Main entry point.
 */
int main(void)
{
    system_init();

    while (1) {
        /* Check battery on every loop */
        if (battery_is_low()) {
            robot_state = STATE_ERROR;
        }

        switch (robot_state) {
            case STATE_IDLE:
                state_idle();
                break;

            case STATE_CALIBRATION:
                state_calibration();
                break;

            case STATE_SEARCH_RUN:
                state_search_run();
                break;

            case STATE_RETURN_TO_START:
                state_return_to_start();
                break;

            case STATE_SPEED_RUN:
                state_speed_run();
                break;

            case STATE_FINISHED:
                motor_disable();
                led_blink(3);
                delay_ms(2000);
                break;

            case STATE_ERROR:
                state_error();
                break;
        }
    }

    return 0;
}

/* ── System Initialization ──────────────────────────────── */
static void system_init(void)
{
    HAL_Init();
    SystemClock_Config();

    /* Initialize all subsystems */
    motor_init();
    encoder_init();
    sensor_init();
    battery_init();
    motion_init();
    maze_init();

    /* Initialize PID controllers */
    pid_init(&pid_left_speed, KP_SPEED, KI_SPEED, KD_SPEED,
             PID_OUTPUT_MIN, PID_OUTPUT_MAX);
    pid_init(&pid_right_speed, KP_SPEED, KI_SPEED, KD_SPEED,
             PID_OUTPUT_MIN, PID_OUTPUT_MAX);
    pid_init(&pid_wall, KP_WALL, KI_WALL, KD_WALL,
             PID_OUTPUT_MIN, PID_OUTPUT_MAX);

    /* Initialize IMU */
    if (!imu_init()) {
        /* IMU not found — continue without it */
    }

    /* Set initial pose */
    robot_pose.x   = 0;
    robot_pose.y   = 0;
    robot_pose.dir = DIR_NORTH;

    /* Start in idle */
    robot_state = STATE_IDLE;
    led_on();
}

/* ── IDLE State ─────────────────────────────────────────── */
static void state_idle(void)
{
    led_on();
    if (button_pressed()) {
        delay_ms(500);
        robot_state = STATE_CALIBRATION;
    }
}

/* ── CALIBRATION State ──────────────────────────────────── */
static void state_calibration(void)
{
    led_blink(2);

    sensor_calibrate();
    imu_calibrate();
    encoder_reset();
    imu_reset_yaw();

    pid_reset(&pid_left_speed);
    pid_reset(&pid_right_speed);
    pid_reset(&pid_wall);

    delay_ms(1000);
    motor_enable();

    robot_state = STATE_SEARCH_RUN;
}

/* ── SEARCH RUN State ───────────────────────────────────── */
static void state_search_run(void)
{
    /* Read sensors */
    sensor_read_all(&sensor_data);
    imu_read(&imu_data);

    /* Update maze walls based on sensor readings */
    bool front = sensor_front_wall(&sensor_data);
    bool left  = sensor_left_wall(&sensor_data);
    bool right = sensor_right_wall(&sensor_data);

    maze_update_walls(robot_pose.x, robot_pose.y, robot_pose.dir,
                      front, left, right);
    maze_mark_visited(robot_pose.x, robot_pose.y);

    /* Check if we reached the goal */
    if (maze_is_goal(robot_pose.x, robot_pose.y)) {
        motion_stop();
        robot_state = STATE_RETURN_TO_START;
        return;
    }

    /* Recompute flood fill and choose next direction */
    maze_flood_fill();
    Direction next_dir = maze_best_direction(&robot_pose);

    /* Execute motion */
    motion_execute_direction(&robot_pose, next_dir);
}

/* ── RETURN TO START State ──────────────────────────────── */
static void state_return_to_start(void)
{
    /* Run flood fill targeting start cell (0,0) */
    maze_flood_fill_to(0, 0);

    sensor_read_all(&sensor_data);
    imu_read(&imu_data);

    bool front = sensor_front_wall(&sensor_data);
    bool left  = sensor_left_wall(&sensor_data);
    bool right = sensor_right_wall(&sensor_data);

    maze_update_walls(robot_pose.x, robot_pose.y, robot_pose.dir,
                      front, left, right);

    if (robot_pose.x == 0 && robot_pose.y == 0) {
        motion_stop();
        /* Wait for button press to start speed run */
        led_blink(5);
        while (!button_pressed()) {
            delay_ms(10);
        }
        delay_ms(500);
        robot_state = STATE_SPEED_RUN;
        return;
    }

    Direction next_dir = maze_best_direction(&robot_pose);
    motion_execute_direction(&robot_pose, next_dir);
}

/* ── SPEED RUN State ────────────────────────────────────── */
static void state_speed_run(void)
{
    /* Recompute flood fill to goal */
    maze_flood_fill();

    sensor_read_all(&sensor_data);
    imu_read(&imu_data);

    if (maze_is_goal(robot_pose.x, robot_pose.y)) {
        motion_stop();
        robot_state = STATE_FINISHED;
        return;
    }

    Direction next_dir = maze_best_direction(&robot_pose);
    motion_execute_direction(&robot_pose, next_dir);
}

/* ── ERROR State ────────────────────────────────────────── */
static void state_error(void)
{
    motor_disable();
    /* Rapid blink to indicate error */
    while (1) {
        led_on();
        delay_ms(100);
        led_off();
        delay_ms(100);
    }
}

/* ── Helper Functions (stubs — replace with HAL) ────────── */
static bool button_pressed(void)
{
    /* TODO: Read PC13 — active low on Blue Pill */
    return false;
}

static void led_on(void)
{
    /* TODO: Set PB0 HIGH */
}

static void led_off(void)
{
    /* TODO: Set PB0 LOW */
}

static void led_blink(uint32_t count)
{
    for (uint32_t i = 0; i < count; i++) {
        led_on();
        delay_ms(200);
        led_off();
        delay_ms(200);
    }
}

static void delay_ms(uint32_t ms)
{
    /* TODO: Use HAL_Delay or SysTick-based delay */
    volatile uint32_t i;
    for (i = 0; i < ms * 7200; i++) {
        __asm__("nop");
    }
}
