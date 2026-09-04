/**
 * @file    encoder.c
 * @brief   Quadrature encoder driver using hardware timer encoder mode.
 * @author  Debosmita Paul
 * @date    2026-09-04
 *
 * Left motor:  TIM3 CH1/CH2 (PA6/PA7)
 * Right motor: TIM4 CH1/CH2 (PB6/PB7)
 */

#include "encoder.h"
#include "config.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ── Internal State ─────────────────────────────────────── */
static int32_t left_count  = 0;
static int32_t right_count = 0;
static int32_t left_prev   = 0;
static int32_t right_prev  = 0;
static int16_t left_speed  = 0;
static int16_t right_speed = 0;

/* ── Initialization ─────────────────────────────────────── */
void encoder_init(void)
{
    /*
     * TODO: Configure TIM3 in Encoder Mode:
     *   - CH1 (PA6) and CH2 (PA7) as encoder inputs
     *   - Count on both edges for 4x resolution
     *   - Auto-reload: 0xFFFF
     *
     * TODO: Configure TIM4 in Encoder Mode:
     *   - CH1 (PB6) and CH2 (PB7) as encoder inputs
     *   - Count on both edges for 4x resolution
     *   - Auto-reload: 0xFFFF
     *
     * TODO: Start both timers:
     *   HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
     *   HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
     */

    encoder_reset();
}

/* ── Get Counts ─────────────────────────────────────────── */
int32_t encoder_get_left(void)
{
    return left_count;
}

int32_t encoder_get_right(void)
{
    return right_count;
}

/* ── Reset Counters ─────────────────────────────────────── */
void encoder_reset(void)
{
    left_count  = 0;
    right_count = 0;
    left_prev   = 0;
    right_prev  = 0;
    left_speed  = 0;
    right_speed = 0;

    /* TODO: Reset timer counters:
     * __HAL_TIM_SET_COUNTER(&htim3, 0);
     * __HAL_TIM_SET_COUNTER(&htim4, 0);
     */
}

/* ── Speed Readings ─────────────────────────────────────── */
int16_t encoder_get_left_speed(void)
{
    return left_speed;
}

int16_t encoder_get_right_speed(void)
{
    return right_speed;
}

/* ── Update (call every control period) ─────────────────── */
void encoder_update(void)
{
    /*
     * TODO: Read timer counters:
     * int32_t raw_left  = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);
     * int32_t raw_right = (int16_t)__HAL_TIM_GET_COUNTER(&htim4);
     *
     * Handle overflow/underflow of 16-bit counters.
     */

    int32_t raw_left  = 0; /* placeholder */
    int32_t raw_right = 0; /* placeholder */

    /* Calculate delta since last update */
    left_speed  = (int16_t)(raw_left  - left_prev);
    right_speed = (int16_t)(raw_right - right_prev);

    left_count  += left_speed;
    right_count += right_speed;

    left_prev  = raw_left;
    right_prev = raw_right;
}

/* ── Convert Ticks to Distance ──────────────────────────── */
float encoder_ticks_to_mm(int32_t ticks)
{
    float circumference = (float)WHEEL_DIAMETER_MM * (float)M_PI;
    return ((float)ticks / (float)TICKS_PER_REV) * circumference;
}
