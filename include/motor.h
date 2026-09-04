/**
 * @file    motor.h
 * @brief   Motor driver interface (TB6612FNG).
 */

#ifndef MOTOR_H
#define MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/** @brief Motor identifier. */
typedef enum {
    MOTOR_LEFT,
    MOTOR_RIGHT
} MotorId;

/** @brief Motor direction. */
typedef enum {
    MOTOR_FORWARD,
    MOTOR_BACKWARD,
    MOTOR_BRAKE,
    MOTOR_COAST
} MotorDirection;

/**
 * @brief  Initialize motor driver GPIO and PWM timers.
 */
void motor_init(void);

/**
 * @brief  Set motor speed and direction.
 * @param  motor   MOTOR_LEFT or MOTOR_RIGHT.
 * @param  dir     Desired direction.
 * @param  pwm     PWM duty (0–999).
 */
void motor_set(MotorId motor, MotorDirection dir, uint16_t pwm);

/**
 * @brief  Enable motor driver (STBY HIGH).
 */
void motor_enable(void);

/**
 * @brief  Disable motor driver (STBY LOW, coast mode).
 */
void motor_disable(void);

/**
 * @brief  Brake both motors immediately.
 */
void motor_brake(void);

#ifdef __cplusplus
}
#endif

#endif /* MOTOR_H */
