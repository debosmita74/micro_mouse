#!/usr/bin/env python3
"""
Micro Mouse Maze Simulator

Interactive maze visualization with flood fill pathfinding.
Displays the maze grid, walls, distances, and computed path.

Usage:
    python maze_simulator.py
    python maze_simulator.py --maze mazes/maze_16x16.txt

Controls:
    Click cells to toggle walls (in edit mode)
    Press 'r' to reset
    Press 'f' to run flood fill
    Press 'q' to quit
"""

import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from flood_fill import FloodFillSolver, NORTH, EAST, SOUTH, WEST


def load_maze_from_file(filename: str, solver: FloodFillSolver) -> None:
    """
    Load maze walls from a text file.

    File format: each line is "x y direction"
    where direction is N, E, S, or W.
    Lines starting with # are comments.
    """
    dir_map = {"N": NORTH, "E": EAST, "S": SOUTH, "W": WEST}

    try:
        with open(filename, "r") as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith("#"):
                    continue
                parts = line.split()
                if len(parts) >= 3:
                    x, y = int(parts[0]), int(parts[1])
                    d = dir_map.get(parts[2].upper())
                    if d is not None:
                        solver.set_wall(x, y, d)
        print(f"Loaded maze from {filename}")
    except FileNotFoundError:
        print(f"Maze file not found: {filename}")
        print("Using empty maze with boundary walls only.")


def draw_maze(solver: FloodFillSolver, path=None) -> None:
    """Draw the maze with matplotlib."""
    size = solver.size
    fig, ax = plt.subplots(1, 1, figsize=(10, 10))
    ax.set_xlim(-0.5, size - 0.5)
    ax.set_ylim(-0.5, size - 0.5)
    ax.set_aspect("equal")
    ax.set_title("Micro Mouse Maze Simulator — Flood Fill", fontsize=14)

    # Draw grid
    for x in range(size):
        for y in range(size):
            # Cell background color based on distance
            d = solver.distance[x][y]
            if d < 255:
                intensity = 1.0 - min(d / 30.0, 0.8)
                color = (intensity, intensity, 1.0)
            else:
                color = (0.9, 0.9, 0.9)

            rect = patches.Rectangle(
                (x - 0.5, y - 0.5), 1, 1,
                linewidth=0.5, edgecolor="lightgray",
                facecolor=color
            )
            ax.add_patch(rect)

            # Distance label
            if d < 255:
                ax.text(x, y, str(d), ha="center", va="center",
                        fontsize=6, color="black")

    # Highlight goal cells
    for gx, gy in solver.goals:
        rect = patches.Rectangle(
            (gx - 0.5, gy - 0.5), 1, 1,
            linewidth=0, facecolor=(0.2, 0.8, 0.2, 0.3)
        )
        ax.add_patch(rect)

    # Highlight start cell
    rect = patches.Rectangle(
        (-0.5, -0.5), 1, 1,
        linewidth=0, facecolor=(1.0, 0.3, 0.3, 0.3)
    )
    ax.add_patch(rect)

    # Draw walls
    for x in range(size):
        for y in range(size):
            if solver.has_wall(x, y, NORTH):
                ax.plot([x - 0.5, x + 0.5], [y + 0.5, y + 0.5],
                        "k-", linewidth=2)
            if solver.has_wall(x, y, SOUTH):
                ax.plot([x - 0.5, x + 0.5], [y - 0.5, y - 0.5],
                        "k-", linewidth=2)
            if solver.has_wall(x, y, EAST):
                ax.plot([x + 0.5, x + 0.5], [y - 0.5, y + 0.5],
                        "k-", linewidth=2)
            if solver.has_wall(x, y, WEST):
                ax.plot([x - 0.5, x - 0.5], [y - 0.5, y + 0.5],
                        "k-", linewidth=2)

    # Draw path
    if path and len(path) > 1:
        px = [p[0] for p in path]
        py = [p[1] for p in path]
        ax.plot(px, py, "r-", linewidth=3, alpha=0.7)
        ax.plot(px[0], py[0], "ro", markersize=10, label="Start")
        ax.plot(px[-1], py[-1], "g*", markersize=15, label="Goal")
        ax.legend(loc="upper right")

    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    plt.tight_layout()
    plt.savefig("maze_output.png", dpi=150)
    print("Saved maze visualization to maze_output.png")
    plt.show()


def main():
    solver = FloodFillSolver(16)

    # Load maze from file if provided
    if len(sys.argv) > 1:
        maze_file = sys.argv[1]
        if maze_file.startswith("--maze="):
            maze_file = maze_file.split("=")[1]
        elif maze_file == "--maze" and len(sys.argv) > 2:
            maze_file = sys.argv[2]
        load_maze_from_file(maze_file, solver)
    else:
        # Generate a sample maze with some walls
        sample_walls = [
            (0, 0, NORTH), (1, 0, EAST), (2, 1, NORTH),
            (3, 0, NORTH), (3, 1, EAST), (4, 2, NORTH),
            (1, 3, EAST), (2, 3, NORTH), (5, 1, NORTH),
            (6, 0, NORTH), (6, 2, EAST), (3, 4, NORTH),
        ]
        for x, y, d in sample_walls:
            solver.set_wall(x, y, d)
        print("Using sample maze (no file provided)")

    # Run flood fill
    solver.compute()

    # Get path
    path = solver.get_path(0, 0)
    print(f"Shortest path: {len(path)} cells")

    # Print distance map
    print("\nDistance Map:")
    solver.print_distances()

    # Visualize
    draw_maze(solver, path)


if __name__ == "__main__":
    main()
