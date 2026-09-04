/**
 * @file    test_maze.c
 * @brief   Unit tests for maze data structure and flood fill.
 *
 * Compile: gcc -Wall -Wextra -Iinclude tests/test_maze.c src/maze.c -o test_maze
 * Run:     ./test_maze
 */

#include <stdio.h>
#include <assert.h>
#include "maze.h"

static void test_maze_init(void)
{
    maze_init();

    /* Check boundary walls */
    /* Bottom-left corner should have S and W walls */
    uint8_t walls = maze_get_walls(0, 0);
    assert((walls & WALL_S) != 0);
    assert((walls & WALL_W) != 0);

    /* Top-right corner should have N and E walls */
    walls = maze_get_walls(15, 15);
    assert((walls & WALL_N) != 0);
    assert((walls & WALL_E) != 0);

    printf("  [PASS] test_maze_init\n");
}

static void test_maze_goal(void)
{
    assert(maze_is_goal(7, 7) == true);
    assert(maze_is_goal(7, 8) == true);
    assert(maze_is_goal(8, 7) == true);
    assert(maze_is_goal(8, 8) == true);
    assert(maze_is_goal(0, 0) == false);
    assert(maze_is_goal(6, 7) == false);

    printf("  [PASS] test_maze_goal\n");
}

static void test_maze_flood_fill(void)
{
    maze_init();
    maze_flood_fill();

    /* Goal cells should have distance 0 */
    assert(maze_get_distance(7, 7) == 0);
    assert(maze_get_distance(8, 8) == 0);

    /* Start cell (0,0) should have a distance > 0 */
    assert(maze_get_distance(0, 0) > 0);

    /* Adjacent to goal should be 1 */
    /* (6,7) should be 1 if no walls block it */
    assert(maze_get_distance(6, 7) == 1);

    printf("  [PASS] test_maze_flood_fill\n");
}

static void test_maze_visited(void)
{
    maze_init();

    assert(maze_is_visited(0, 0) == false);
    maze_mark_visited(0, 0);
    assert(maze_is_visited(0, 0) == true);

    printf("  [PASS] test_maze_visited\n");
}

static void test_maze_wall_update(void)
{
    maze_init();

    /* Robot at (1,1) facing North, front wall detected */
    maze_update_walls(1, 1, DIR_NORTH, true, false, false);

    uint8_t walls = maze_get_walls(1, 1);
    assert((walls & WALL_N) != 0);

    /* Neighbor (1,2) should have south wall set */
    uint8_t neighbor_walls = maze_get_walls(1, 2);
    assert((neighbor_walls & WALL_S) != 0);

    printf("  [PASS] test_maze_wall_update\n");
}

static void test_maze_best_direction(void)
{
    maze_init();
    maze_flood_fill();

    Pose pose = { .x = 0, .y = 0, .dir = DIR_NORTH };
    Direction best = maze_best_direction(&pose);

    /* From (0,0), best direction should lead toward lower distance */
    /* The exact direction depends on wall configuration */
    /* At minimum, it should be a valid direction */
    assert(best >= DIR_NORTH && best <= DIR_WEST);

    printf("  [PASS] test_maze_best_direction\n");
}

int main(void)
{
    printf("Running Maze tests...\n");

    test_maze_init();
    test_maze_goal();
    test_maze_flood_fill();
    test_maze_visited();
    test_maze_wall_update();
    test_maze_best_direction();

    printf("All Maze tests passed!\n");
    return 0;
}
