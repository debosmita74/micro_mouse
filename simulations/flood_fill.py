#!/usr/bin/env python3
"""
Flood Fill Algorithm — Python Implementation

A standalone implementation of the flood fill maze-solving algorithm
for testing and visualization without hardware.

Usage:
    from flood_fill import FloodFillSolver
    solver = FloodFillSolver(size=16)
    solver.set_wall(x, y, direction)
    solver.compute()
    path = solver.get_path(start_x, start_y)
"""

from collections import deque
from typing import List, Tuple, Optional

# Directions
NORTH, EAST, SOUTH, WEST = 0, 1, 2, 3
DIRECTION_NAMES = ["North", "East", "South", "West"]

# Wall bit masks
WALL_N, WALL_E, WALL_S, WALL_W = 0x01, 0x02, 0x04, 0x08

# Direction offsets (dx, dy)
DX = [0, 1, 0, -1]
DY = [1, 0, -1, 0]

# Opposite walls
OPPOSITE = {WALL_N: WALL_S, WALL_E: WALL_W, WALL_S: WALL_N, WALL_W: WALL_E}
DIR_TO_WALL = {NORTH: WALL_N, EAST: WALL_E, SOUTH: WALL_S, WEST: WALL_W}


class FloodFillSolver:
    """Flood fill maze solver for a square grid maze."""

    def __init__(self, size: int = 16):
        self.size = size
        self.walls = [[0] * size for _ in range(size)]
        self.distance = [[255] * size for _ in range(size)]

        # Goal: center 2x2
        self.goals = []
        mid = size // 2
        for gx in [mid - 1, mid]:
            for gy in [mid - 1, mid]:
                self.goals.append((gx, gy))

        # Set boundary walls
        for i in range(size):
            self.walls[i][0] |= WALL_S
            self.walls[i][size - 1] |= WALL_N
            self.walls[0][i] |= WALL_W
            self.walls[size - 1][i] |= WALL_E

    def set_wall(self, x: int, y: int, direction: int) -> None:
        """Set a wall on a cell in the given direction."""
        wall = DIR_TO_WALL[direction]
        self.walls[x][y] |= wall

        # Set matching wall on neighbor
        nx, ny = x + DX[direction], y + DY[direction]
        if 0 <= nx < self.size and 0 <= ny < self.size:
            opp = OPPOSITE[wall]
            self.walls[nx][ny] |= opp

    def has_wall(self, x: int, y: int, direction: int) -> bool:
        """Check if cell has a wall in the given direction."""
        return bool(self.walls[x][y] & DIR_TO_WALL[direction])

    def compute(self) -> None:
        """Run flood fill from goal cells."""
        self.distance = [[255] * self.size for _ in range(self.size)]
        queue = deque()

        for gx, gy in self.goals:
            self.distance[gx][gy] = 0
            queue.append((gx, gy))

        while queue:
            cx, cy = queue.popleft()

            for d in range(4):
                if self.walls[cx][cy] & DIR_TO_WALL[d]:
                    continue

                nx, ny = cx + DX[d], cy + DY[d]
                if not (0 <= nx < self.size and 0 <= ny < self.size):
                    continue

                new_dist = self.distance[cx][cy] + 1
                if new_dist < self.distance[nx][ny]:
                    self.distance[nx][ny] = new_dist
                    queue.append((nx, ny))

    def get_path(self, start_x: int = 0, start_y: int = 0) -> List[Tuple[int, int]]:
        """Get shortest path from start to goal using computed distances."""
        path = [(start_x, start_y)]
        x, y = start_x, start_y

        while (x, y) not in self.goals:
            best_dir = None
            best_dist = 255

            for d in range(4):
                if self.walls[x][y] & DIR_TO_WALL[d]:
                    continue

                nx, ny = x + DX[d], y + DY[d]
                if not (0 <= nx < self.size and 0 <= ny < self.size):
                    continue

                if self.distance[nx][ny] < best_dist:
                    best_dist = self.distance[nx][ny]
                    best_dir = d

            if best_dir is None:
                break  # No path found

            x, y = x + DX[best_dir], y + DY[best_dir]
            path.append((x, y))

        return path

    def print_distances(self) -> None:
        """Print distance map to console."""
        for y in range(self.size - 1, -1, -1):
            row = ""
            for x in range(self.size):
                d = self.distance[x][y]
                if d == 255:
                    row += " -- "
                else:
                    row += f" {d:2d} "
            print(row)


if __name__ == "__main__":
    solver = FloodFillSolver(16)

    # Add some example walls
    solver.set_wall(0, 0, EAST)
    solver.set_wall(1, 0, NORTH)
    solver.set_wall(2, 0, NORTH)

    solver.compute()

    print("Distance Map:")
    solver.print_distances()

    path = solver.get_path(0, 0)
    print(f"\nPath from (0,0) to goal: {len(path)} cells")
    print(path[:10], "..." if len(path) > 10 else "")
