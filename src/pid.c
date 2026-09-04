/**
 * @file    pid.c
 * @brief   PID controller with anti-windup clamping.
 * @author  Debosmita Paul
 * @date    2026-09-04
 */

#include "pid.h"

/* ── Initialize ─────────────────────────────────────────── */
void pid_init(PidController *pid, float kp, float ki, float kd,
              float out_min, float out_max)
{
    if (pid == (void *)0) return;

    pid->kp         = kp;
    pid->ki         = ki;
    pid->kd         = kd;
    pid->out_min    = out_min;
    pid->out_max    = out_max;
    pid->integral   = 0.0f;
    pid->prev_error = 0.0f;
}

/* ── Compute PID Output ─────────────────────────────────── */
float pid_compute(PidController *pid, float setpoint,
                  float measured, float dt)
{
    if (pid == (void *)0 || dt <= 0.0f) return 0.0f;

    float error = setpoint - measured;

    /* Proportional */
    float p_term = pid->kp * error;

    /* Integral with anti-windup */
    pid->integral += error * dt;
    float i_term = pid->ki * pid->integral;

    /* Clamp integral to prevent windup */
    if (i_term > pid->out_max) {
        i_term = pid->out_max;
        pid->integral = pid->out_max / pid->ki;
    } else if (i_term < pid->out_min) {
        i_term = pid->out_min;
        pid->integral = pid->out_min / pid->ki;
    }

    /* Derivative */
    float d_term = pid->kd * (error - pid->prev_error) / dt;
    pid->prev_error = error;

    /* Sum and clamp */
    float output = p_term + i_term + d_term;

    if (output > pid->out_max) output = pid->out_max;
    if (output < pid->out_min) output = pid->out_min;

    return output;
}

/* ── Reset ──────────────────────────────────────────────── */
void pid_reset(PidController *pid)
{
    if (pid == (void *)0) return;

    pid->integral   = 0.0f;
    pid->prev_error = 0.0f;
}
