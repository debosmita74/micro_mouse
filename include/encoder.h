/**
 * @file    encoder.h
 * @brief   Quadrature encoder interface for odometry.
 */

#ifndef ENCODER_H
#define ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief  Initialize encoder timers (TIM3, TIM4) in encoder mode.
 */
void encoder_init(void);

/**
 * @brief  Get left encoder count (signed, cumulative).
 * @return Encoder tick count.
 */
int32_t encoder_get_left(void);

/**
 * @brief  Get right encoder count (signed, cumulative).
 * @return Encoder tick count.
 */
int32_t encoder_get_right(void);

/**
 * @brief  Reset both encoder counters to zero.
 */
void encoder_reset(void);

/**
 * @brief  Get left encoder speed in ticks per control period.
 * @return Speed in ticks/period.
 */
int16_t encoder_get_left_speed(void);

/**
 * @brief  Get right encoder speed in ticks per control period.
 * @return Speed in ticks/period.
 */
int16_t encoder_get_right_speed(void);

/**
 * @brief  Update speed calculations. Call once per control loop.
 */
void encoder_update(void);

/**
 * @brief  Convert encoder ticks to distance in mm.
 * @param  ticks  Encoder tick count.
 * @return Distance in mm.
 */
float encoder_ticks_to_mm(int32_t ticks);

#ifdef __cplusplus
}
#endif

#endif /* ENCODER_H */
