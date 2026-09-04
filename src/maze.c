/**
 * @file    maze.c
 * @brief   Maze data structure and Flood Fill solver.
 * @author  Debosmita Paul
 * @date    2026-09-04
 *
 * Implements a 16×16 maze with wall tracking and BFS-based flood fill.
 *
 * Coordinate system:
 *   (0,0) = bottom-left corner (start cell)
 *   X increases to the right (East)
 *   Y increases upward (North)
 *
 * Goal: center 2×2 cells: (7,7), (7,8), (8,7), (8,8)
 */

#include "maze.h"
#include <string.h>

/* ── Maze Arrays ────────────────────────────────────────── */
uint8_t maze_walls[MAZE_SIZE][MAZE_SIZE];
uint8_t maze_distance[MAZE_SIZE][MAZE_SIZE];

/* ── BFS Queue (simple circular buffer) ─────────────────── */
#define QUEUE_SIZE 256

typedef struct {
    uint8_t x;
    uint8_t y;
} QueueItem;

static QueueItem queue[QUEUE_SIZE];
static uint16_t  queue_head = 0;
static uint16_t  queue_tail = 0;

static void queue_clear(void)  { queue_head = queue_tail = 0; }
static bool queue_empty(void)  { return queue_head == queue_tail; }

static void queue_push(uint8_t x, uint8_t y)
{
    queue[queue_tail].x = x;
    queue[queue_tail].y = y;
    queue_tail = (queue_tail + 1) % QUEUE_SIZE;
}

static QueueItem queue_pop(void)
{
    QueueItem item = queue[queue_head];
    queue_head = (queue_head + 1) % QUEUE_SIZE;
    return item;
}

/* ── Direction Offsets ──────────────────────────────────── */
static const int8_t dx[] = { 0, 1, 0, -1 }; /* N, E, S, W */
static const int8_t dy[] = { 1, 0, -1, 0 };

/* Opposite wall mask for neighbor */
static const uint8_t opposite_wall[] = { WALL_S, WALL_W, WALL_N, WALL_E };
static const uint8_t dir_wall[]      = { WALL_N, WALL_E, WALL_S, WALL_W };

/* ── Initialize Maze ────────────────────────────────────── */
void maze_init(void)
{
    memset(maze_walls, 0, sizeof(maze_walls));
    memset(maze_distance, 0xFF, sizeof(maze_distance));

    /* Set outer boundary walls */
    for (uint8_t i = 0; i < MAZE_SIZE; i++) {
        maze_walls[i][0]             |= WALL_S;   /* Bottom row: south wall  */
        maze_walls[i][MAZE_SIZE - 1] |= WALL_N;   /* Top row: north wall     */
        maze_walls[0][i]             |= WALL_W;   /* Left column: west wall  */
        maze_walls[MAZE_SIZE - 1][i] |= WALL_E;   /* Right column: east wall */
    }

    /* Start cell (0,0) has east wall open, south and west walls */
    maze_walls[0][0] |= WALL_S | WALL_W;
}

/* ── Update Walls ───────────────────────────────────────── */
void maze_update_walls(uint8_t x, uint8_t y, Direction dir,
                       bool front, bool left, bool right)
{
    if (x >= MAZE_SIZE || y >= MAZE_SIZE) return;

    /* Map sensor readings to absolute directions */
    Direction abs_front = dir;
    Direction abs_left  = (dir + 3) % 4;  /* left = dir - 1 */
    Direction abs_right = (dir + 1) % 4;  /* right = dir + 1 */

    /* Set/clear front wall */
    if (front) {
        maze_walls[x][y] |= dir_wall[abs_front];
        /* Set matching wall in neighbor */
        int8_t nx = x + dx[abs_front];
        int8_t ny = y + dy[abs_front];
        if (nx >= 0 && nx < MAZE_SIZE && ny >= 0 && ny < MAZE_SIZE) {
            maze_walls[nx][ny] |= opposite_wall[abs_front];
        }
    }

    /* Set/clear left wall */
    if (left) {
        maze_walls[x][y] |= dir_wall[abs_left];
        int8_t nx = x + dx[abs_left];
        int8_t ny = y + dy[abs_left];
        if (nx >= 0 && nx < MAZE_SIZE && ny >= 0 && ny < MAZE_SIZE) {
            maze_walls[nx][ny] |= opposite_wall[abs_left];
        }
    }

    /* Set/clear right wall */
    if (right) {
        maze_walls[x][y] |= dir_wall[abs_right];
        int8_t nx = x + dx[abs_right];
        int8_t ny = y + dy[abs_right];
        if (nx >= 0 && nx < MAZE_SIZE && ny >= 0 && ny < MAZE_SIZE) {
            maze_walls[nx][ny] |= opposite_wall[abs_right];
        }
    }
}

/* ── Flood Fill (BFS from goal cells) ───────────────────── */
void maze_flood_fill(void)
{
    memset(maze_distance, 0xFF, sizeof(maze_distance));
    queue_clear();

    /* Seed goal cells with distance 0 */
    for (uint8_t gx = GOAL_X_MIN; gx <= GOAL_X_MAX; gx++) {
        for (uint8_t gy = GOAL_Y_MIN; gy <= GOAL_Y_MAX; gy++) {
            maze_distance[gx][gy] = 0;
            queue_push(gx, gy);
        }
    }

    /* BFS propagation */
    while (!queue_empty()) {
        QueueItem cur = queue_pop();

        for (uint8_t d = 0; d < 4; d++) {
            /* Check if wall blocks this direction */
            if (maze_walls[cur.x][cur.y] & dir_wall[d]) continue;

            int8_t nx = cur.x + dx[d];
            int8_t ny = cur.y + dy[d];

            if (nx < 0 || nx >= MAZE_SIZE || ny < 0 || ny >= MAZE_SIZE) continue;

            uint8_t new_dist = maze_distance[cur.x][cur.y] + 1;

            if (new_dist < maze_distance[nx][ny]) {
                maze_distance[nx][ny] = new_dist;
                queue_push((uint8_t)nx, (uint8_t)ny);
            }
        }
    }
}

/* ── Flood Fill to Specific Target ──────────────────────── */
void maze_flood_fill_to(uint8_t target_x, uint8_t target_y)
{
    memset(maze_distance, 0xFF, sizeof(maze_distance));
    queue_clear();

    maze_distance[target_x][target_y] = 0;
    queue_push(target_x, target_y);

    while (!queue_empty()) {
        QueueItem cur = queue_pop();

        for (uint8_t d = 0; d < 4; d++) {
            if (maze_walls[cur.x][cur.y] & dir_wall[d]) continue;

            int8_t nx = cur.x + dx[d];
            int8_t ny = cur.y + dy[d];

            if (nx < 0 || nx >= MAZE_SIZE || ny < 0 || ny >= MAZE_SIZE) continue;

            uint8_t new_dist = maze_distance[cur.x][cur.y] + 1;

            if (new_dist < maze_distance[nx][ny]) {
                maze_distance[nx][ny] = new_dist;
                queue_push((uint8_t)nx, (uint8_t)ny);
            }
        }
    }
}

/* ── Best Direction ─────────────────────────────────────── */
Direction maze_best_direction(const Pose *pose)
{
    uint8_t min_dist = 0xFF;
    Direction best = pose->dir;  /* default: keep going */

    for (uint8_t d = 0; d < 4; d++) {
        /* Check wall */
        if (maze_walls[pose->x][pose->y] & dir_wall[d]) continue;

        int8_t nx = pose->x + dx[d];
        int8_t ny = pose->y + dy[d];

        if (nx < 0 || nx >= MAZE_SIZE || ny < 0 || ny >= MAZE_SIZE) continue;

        if (maze_distance[nx][ny] < min_dist) {
            min_dist = maze_distance[nx][ny];
            best = (Direction)d;
        }
    }

    return best;
}

/* ── Goal Check ─────────────────────────────────────────── */
bool maze_is_goal(uint8_t x, uint8_t y)
{
    return (x >= GOAL_X_MIN && x <= GOAL_X_MAX &&
            y >= GOAL_Y_MIN && y <= GOAL_Y_MAX);
}

/* ── Visited Tracking ───────────────────────────────────── */
void maze_mark_visited(uint8_t x, uint8_t y)
{
    if (x < MAZE_SIZE && y < MAZE_SIZE) {
        maze_walls[x][y] |= CELL_VISITED;
    }
}

bool maze_is_visited(uint8_t x, uint8_t y)
{
    if (x >= MAZE_SIZE || y >= MAZE_SIZE) return false;
    return (maze_walls[x][y] & CELL_VISITED) != 0;
}

/* ── Accessors ──────────────────────────────────────────── */
uint8_t maze_get_walls(uint8_t x, uint8_t y)
{
    if (x >= MAZE_SIZE || y >= MAZE_SIZE) return 0x0F;
    return maze_walls[x][y] & 0x0F;
}

uint8_t maze_get_distance(uint8_t x, uint8_t y)
{
    if (x >= MAZE_SIZE || y >= MAZE_SIZE) return 0xFF;
    return maze_distance[x][y];
}
