/**
 * @file    motion.h
 * @brief   Motion profiling and navigation commands.
 */

#ifndef MOTION_H
#define MOTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "maze.h"

/** @brief Motion state. */
typedef enum {
    MOTION_IDLE,
    MOTION_ACCELERATING,
    MOTION_CRUISING,
    MOTION_DECELERATING,
    MOTION_COMPLETE
} MotionState;

/**
 * @brief  Initialize motion controller.
 */
void motion_init(void);

/**
 * @brief  Move forward one cell (180 mm center-to-center).
 */
void motion_move_cell(void);

/**
 * @brief  Move forward a specified distance.
 * @param  distance_mm  Distance in mm.
 * @param  end_speed    Desired speed at end (mm/s), 0 to stop.
 */
void motion_move(float distance_mm, float end_speed);

/**
 * @brief  Execute an in-place turn.
 * @param  angle_deg  Turn angle: +90 = left, -90 = right, 180 = U-turn.
 */
void motion_turn(float angle_deg);

/**
 * @brief  Execute movement toward the best next cell.
 * @param  current  Current pose (updated after move).
 */
void motion_execute_direction(Pose *current, Direction target_dir);

/**
 * @brief  Stop all motion and brake.
 */
void motion_stop(void);

/**
 * @brief  Update motion controller. Call every control period.
 */
void motion_update(void);

/**
 * @brief  Check if current motion command is complete.
 * @return true if motion finished.
 */
bool motion_is_complete(void);

#ifdef __cplusplus
}
#endif

#endif /* MOTION_H */
