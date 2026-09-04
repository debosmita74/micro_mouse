/**
 * @file    maze.h
 * @brief   Maze data structure and flood fill solver.
 */

#ifndef MAZE_H
#define MAZE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

/** @brief Cardinal directions. */
typedef enum {
    DIR_NORTH = 0,
    DIR_EAST  = 1,
    DIR_SOUTH = 2,
    DIR_WEST  = 3
} Direction;

/** @brief Robot pose in the maze. */
typedef struct {
    uint8_t   x;        /**< Column (0–15)      */
    uint8_t   y;        /**< Row (0–15)          */
    Direction dir;      /**< Facing direction    */
} Pose;

/**
 * @brief  Initialize maze — clear all walls, set outer boundary.
 */
void maze_init(void);

/**
 * @brief  Update walls for cell (x, y) based on sensor readings.
 * @param  x      Cell column.
 * @param  y      Cell row.
 * @param  dir    Robot facing direction.
 * @param  front  true if front wall detected.
 * @param  left   true if left wall detected.
 * @param  right  true if right wall detected.
 */
void maze_update_walls(uint8_t x, uint8_t y, Direction dir,
                       bool front, bool left, bool right);

/**
 * @brief  Run flood fill from goal cells.
 *         Updates distance[][] array.
 */
void maze_flood_fill(void);

/**
 * @brief  Run flood fill from a specific target cell.
 * @param  target_x  Target column.
 * @param  target_y  Target row.
 */
void maze_flood_fill_to(uint8_t target_x, uint8_t target_y);

/**
 * @brief  Choose best next direction based on flood fill distances.
 * @param  pose  Current robot pose.
 * @return Best direction to move.
 */
Direction maze_best_direction(const Pose *pose);

/**
 * @brief  Check if cell (x, y) is in the goal area.
 * @return true if goal cell.
 */
bool maze_is_goal(uint8_t x, uint8_t y);

/**
 * @brief  Mark cell as visited.
 * @param  x  Cell column.
 * @param  y  Cell row.
 */
void maze_mark_visited(uint8_t x, uint8_t y);

/**
 * @brief  Check if cell has been visited.
 * @return true if visited.
 */
bool maze_is_visited(uint8_t x, uint8_t y);

/**
 * @brief  Get wall data for a cell.
 * @param  x  Cell column.
 * @param  y  Cell row.
 * @return Bitmask of walls (WALL_N | WALL_E | WALL_S | WALL_W).
 */
uint8_t maze_get_walls(uint8_t x, uint8_t y);

/**
 * @brief  Get flood fill distance for a cell.
 * @param  x  Cell column.
 * @param  y  Cell row.
 * @return Distance value.
 */
uint8_t maze_get_distance(uint8_t x, uint8_t y);

/** @brief External access to maze arrays (for debug/simulation). */
extern uint8_t maze_walls[MAZE_SIZE][MAZE_SIZE];
extern uint8_t maze_distance[MAZE_SIZE][MAZE_SIZE];

#ifdef __cplusplus
}
#endif

#endif /* MAZE_H */
