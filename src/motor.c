/**
 * @file    motor.c
 * @brief   Motor driver implementation for TB6612FNG.
 * @author  Debosmita Paul
 * @date    2026-09-04
 *
 * Controls two DC motors via TB6612FNG H-bridge.
 * PWM generated on TIM1 CH1 (left) and CH2 (right).
 */

#include "motor.h"
#include "config.h"

/*
 * NOTE: This implementation uses pseudo-register access as a reference.
 * Replace with actual STM32 HAL or LL calls for your project.
 */

/* ── Initialization ─────────────────────────────────────── */
void motor_init(void)
{
    /*
     * TODO: Initialize GPIO pins as outputs:
     *   PB12 (AIN1), PB13 (AIN2) — left motor direction
     *   PB14 (BIN1), PB15 (BIN2) — right motor direction
     *   PB5  (STBY)              — standby control
     *
     * TODO: Initialize TIM1 in PWM mode:
     *   CH1 (PA8) — left motor PWM
     *   CH2 (PA9) — right motor PWM
     *   Period: 999 (for 72kHz PWM at 72MHz clock with prescaler=0)
     */

    /* Start with motors disabled */
    motor_disable();
}

/* ── Set Motor Speed & Direction ────────────────────────── */
void motor_set(MotorId motor, MotorDirection dir, uint16_t pwm)
{
    /* Clamp PWM to valid range */
    if (pwm > 999) {
        pwm = 999;
    }

    if (motor == MOTOR_LEFT) {
        switch (dir) {
            case MOTOR_FORWARD:
                /* AIN1 = HIGH, AIN2 = LOW */
                /* TODO: HAL_GPIO_WritePin(MOTOR_L_IN1_PORT, MOTOR_L_IN1_PIN, GPIO_PIN_SET);   */
                /* TODO: HAL_GPIO_WritePin(MOTOR_L_IN2_PORT, MOTOR_L_IN2_PIN, GPIO_PIN_RESET); */
                break;
            case MOTOR_BACKWARD:
                /* AIN1 = LOW, AIN2 = HIGH */
                /* TODO: HAL_GPIO_WritePin(MOTOR_L_IN1_PORT, MOTOR_L_IN1_PIN, GPIO_PIN_RESET); */
                /* TODO: HAL_GPIO_WritePin(MOTOR_L_IN2_PORT, MOTOR_L_IN2_PIN, GPIO_PIN_SET);   */
                break;
            case MOTOR_BRAKE:
                /* AIN1 = HIGH, AIN2 = HIGH */
                /* TODO: HAL_GPIO_WritePin(MOTOR_L_IN1_PORT, MOTOR_L_IN1_PIN, GPIO_PIN_SET); */
                /* TODO: HAL_GPIO_WritePin(MOTOR_L_IN2_PORT, MOTOR_L_IN2_PIN, GPIO_PIN_SET); */
                break;
            case MOTOR_COAST:
                /* AIN1 = LOW, AIN2 = LOW */
                /* TODO: HAL_GPIO_WritePin(MOTOR_L_IN1_PORT, MOTOR_L_IN1_PIN, GPIO_PIN_RESET); */
                /* TODO: HAL_GPIO_WritePin(MOTOR_L_IN2_PORT, MOTOR_L_IN2_PIN, GPIO_PIN_RESET); */
                break;
        }
        /* Set PWM duty on TIM1_CH1 */
        /* TODO: __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm); */
    }
    else { /* MOTOR_RIGHT */
        switch (dir) {
            case MOTOR_FORWARD:
                /* TODO: HAL_GPIO_WritePin(MOTOR_R_IN1_PORT, MOTOR_R_IN1_PIN, GPIO_PIN_SET);   */
                /* TODO: HAL_GPIO_WritePin(MOTOR_R_IN2_PORT, MOTOR_R_IN2_PIN, GPIO_PIN_RESET); */
                break;
            case MOTOR_BACKWARD:
                /* TODO: HAL_GPIO_WritePin(MOTOR_R_IN1_PORT, MOTOR_R_IN1_PIN, GPIO_PIN_RESET); */
                /* TODO: HAL_GPIO_WritePin(MOTOR_R_IN2_PORT, MOTOR_R_IN2_PIN, GPIO_PIN_SET);   */
                break;
            case MOTOR_BRAKE:
                /* TODO: HAL_GPIO_WritePin(MOTOR_R_IN1_PORT, MOTOR_R_IN1_PIN, GPIO_PIN_SET); */
                /* TODO: HAL_GPIO_WritePin(MOTOR_R_IN2_PORT, MOTOR_R_IN2_PIN, GPIO_PIN_SET); */
                break;
            case MOTOR_COAST:
                /* TODO: HAL_GPIO_WritePin(MOTOR_R_IN1_PORT, MOTOR_R_IN1_PIN, GPIO_PIN_RESET); */
                /* TODO: HAL_GPIO_WritePin(MOTOR_R_IN2_PORT, MOTOR_R_IN2_PIN, GPIO_PIN_RESET); */
                break;
        }
        /* Set PWM duty on TIM1_CH2 */
        /* TODO: __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, pwm); */
    }
}

/* ── Enable / Disable ───────────────────────────────────── */
void motor_enable(void)
{
    /* STBY = HIGH */
    /* TODO: HAL_GPIO_WritePin(MOTOR_STBY_PORT, MOTOR_STBY_PIN, GPIO_PIN_SET); */
}

void motor_disable(void)
{
    /* STBY = LOW */
    /* TODO: HAL_GPIO_WritePin(MOTOR_STBY_PORT, MOTOR_STBY_PIN, GPIO_PIN_RESET); */
}

/* ── Brake Both Motors ──────────────────────────────────── */
void motor_brake(void)
{
    motor_set(MOTOR_LEFT,  MOTOR_BRAKE, 999);
    motor_set(MOTOR_RIGHT, MOTOR_BRAKE, 999);
}
