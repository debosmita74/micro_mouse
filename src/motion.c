/**
 * @file    motion.c
 * @brief   Motion profiling and navigation.
 * @author  Debosmita Paul
 * @date    2026-09-04
 *
 * Implements trapezoidal velocity profiles and cell-to-cell movement.
 */

#include "motion.h"
#include "motor.h"
#include "encoder.h"
#include "sensor.h"
#include "imu.h"
#include "pid.h"
#include "config.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ── Motion State ───────────────────────────────────────── */
static MotionState current_state = MOTION_IDLE;
static float target_distance  = 0.0f;
static float current_speed    = 0.0f;
static float target_speed     = 0.0f;
static float end_speed_target = 0.0f;
static float distance_covered = 0.0f;

/* ── PID Controllers (local to motion) ──────────────────── */
static PidController pid_speed_l;
static PidController pid_speed_r;
static PidController pid_wall_follow;

/* ── Initialization ─────────────────────────────────────── */
void motion_init(void)
{
    pid_init(&pid_speed_l, KP_SPEED, KI_SPEED, KD_SPEED,
             PID_OUTPUT_MIN, PID_OUTPUT_MAX);
    pid_init(&pid_speed_r, KP_SPEED, KI_SPEED, KD_SPEED,
             PID_OUTPUT_MIN, PID_OUTPUT_MAX);
    pid_init(&pid_wall_follow, KP_WALL, KI_WALL, KD_WALL,
             -200, 200);

    current_state = MOTION_IDLE;
}

/* ── Move One Cell ──────────────────────────────────────── */
void motion_move_cell(void)
{
    motion_move((float)CELL_SIZE_MM, 0.0f);
}

/* ── Move Distance ──────────────────────────────────────── */
void motion_move(float distance_mm, float end_speed)
{
    target_distance  = distance_mm;
    end_speed_target = end_speed;
    target_speed     = (float)SEARCH_SPEED;
    distance_covered = 0.0f;
    current_speed    = 0.0f;
    current_state    = MOTION_ACCELERATING;

    encoder_reset();
    pid_reset(&pid_speed_l);
    pid_reset(&pid_speed_r);
    pid_reset(&pid_wall_follow);

    /* Run motion loop until complete */
    while (current_state != MOTION_COMPLETE &&
           current_state != MOTION_IDLE) {
        motion_update();
        /* Small delay for control period */
        volatile uint32_t d;
        for (d = 0; d < 72; d++) { __asm__("nop"); }
    }
}

/* ── Turn ───────────────────────────────────────────────── */
void motion_turn(float angle_deg)
{
    /* Calculate arc length each wheel must travel */
    float arc = (float)M_PI * (float)WHEEL_TRACK_MM * fabsf(angle_deg) / 360.0f;

    encoder_reset();
    imu_reset_yaw();

    PidController pid_turn;
    pid_init(&pid_turn, KP_TURN, KI_TURN, KD_TURN, -500, 500);

    float target_yaw = angle_deg;
    float current_yaw = 0.0f;

    /* Turn loop */
    int timeout = 5000;  /* safety timeout */
    while (timeout > 0) {
        current_yaw = imu_get_yaw();
        float error = target_yaw - current_yaw;

        if (fabsf(error) < 2.0f) {
            break;  /* Close enough */
        }

        float correction = pid_compute(&pid_turn, target_yaw,
                                        current_yaw, CONTROL_DT);
        int16_t pwm = (int16_t)fabsf(correction);
        if (pwm > 400) pwm = 400;

        if (correction > 0) {
            /* Turn left: left backward, right forward */
            motor_set(MOTOR_LEFT,  MOTOR_BACKWARD, (uint16_t)pwm);
            motor_set(MOTOR_RIGHT, MOTOR_FORWARD,  (uint16_t)pwm);
        } else {
            /* Turn right: left forward, right backward */
            motor_set(MOTOR_LEFT,  MOTOR_FORWARD,  (uint16_t)pwm);
            motor_set(MOTOR_RIGHT, MOTOR_BACKWARD, (uint16_t)pwm);
        }

        encoder_update();
        ImuData imu;
        imu_read(&imu);

        timeout--;
        volatile uint32_t d;
        for (d = 0; d < 72; d++) { __asm__("nop"); }
    }

    motor_brake();
    current_state = MOTION_COMPLETE;
}

/* ── Execute Direction Change + Move ────────────────────── */
void motion_execute_direction(Pose *current, Direction target_dir)
{
    /* Calculate turn angle */
    int turn = (int)target_dir - (int)current->dir;

    /* Normalize to [-2, 2] */
    if (turn > 2)  turn -= 4;
    if (turn < -2) turn += 4;

    float angle = (float)turn * 90.0f;

    /* Turn if needed */
    if (turn != 0) {
        motion_turn(angle);
    }

    /* Move one cell forward */
    motion_move_cell();

    /* Update pose */
    current->dir = target_dir;

    static const int8_t dx[] = { 0, 1, 0, -1 };
    static const int8_t dy[] = { 1, 0, -1, 0 };

    current->x = (uint8_t)((int8_t)current->x + dx[target_dir]);
    current->y = (uint8_t)((int8_t)current->y + dy[target_dir]);
}

/* ── Stop ───────────────────────────────────────────────── */
void motion_stop(void)
{
    motor_brake();
    current_state = MOTION_IDLE;
    current_speed = 0.0f;
}

/* ── Update (call every control period) ─────────────────── */
void motion_update(void)
{
    encoder_update();

    /* Get distance from encoders */
    float left_mm  = encoder_ticks_to_mm(encoder_get_left());
    float right_mm = encoder_ticks_to_mm(encoder_get_right());
    distance_covered = (left_mm + right_mm) / 2.0f;

    float remaining = target_distance - distance_covered;

    /* Trapezoidal profile */
    float decel_dist = (current_speed * current_speed -
                        end_speed_target * end_speed_target) /
                       (2.0f * (float)DECELERATION);

    switch (current_state) {
        case MOTION_ACCELERATING:
            current_speed += (float)ACCELERATION * CONTROL_DT;
            if (current_speed >= target_speed) {
                current_speed = target_speed;
                current_state = MOTION_CRUISING;
            }
            if (remaining <= decel_dist) {
                current_state = MOTION_DECELERATING;
            }
            break;

        case MOTION_CRUISING:
            if (remaining <= decel_dist) {
                current_state = MOTION_DECELERATING;
            }
            break;

        case MOTION_DECELERATING:
            current_speed -= (float)DECELERATION * CONTROL_DT;
            if (current_speed <= end_speed_target || remaining <= 0.0f) {
                current_speed = end_speed_target;
                current_state = MOTION_COMPLETE;
                if (end_speed_target == 0.0f) {
                    motor_brake();
                }
                return;
            }
            break;

        case MOTION_COMPLETE:
        case MOTION_IDLE:
            return;
    }

    /* Get wall-following correction */
    SensorData sd;
    sensor_read_all(&sd);
    int16_t wall_err = sensor_get_wall_error(&sd);
    float correction = pid_compute(&pid_wall_follow, 0.0f,
                                    (float)wall_err, CONTROL_DT);

    /* Apply speed + correction to motors */
    float left_target  = current_speed - correction;
    float right_target = current_speed + correction;

    /* Speed PID */
    float left_actual  = encoder_ticks_to_mm(encoder_get_left_speed()) /
                         CONTROL_DT;
    float right_actual = encoder_ticks_to_mm(encoder_get_right_speed()) /
                         CONTROL_DT;

    float left_pwm  = pid_compute(&pid_speed_l, left_target,
                                   left_actual, CONTROL_DT);
    float right_pwm = pid_compute(&pid_speed_r, right_target,
                                   right_actual, CONTROL_DT);

    /* Apply to motors */
    MotorDirection l_dir = (left_pwm >= 0) ? MOTOR_FORWARD : MOTOR_BACKWARD;
    MotorDirection r_dir = (right_pwm >= 0) ? MOTOR_FORWARD : MOTOR_BACKWARD;

    motor_set(MOTOR_LEFT,  l_dir, (uint16_t)fabsf(left_pwm));
    motor_set(MOTOR_RIGHT, r_dir, (uint16_t)fabsf(right_pwm));
}

/* ── Check Complete ─────────────────────────────────────── */
bool motion_is_complete(void)
{
    return (current_state == MOTION_COMPLETE);
}
