# 🧠 Algorithms

Detailed explanation of algorithms used in the Micro Mouse robot.

---

## 1. Flood Fill Algorithm

### Overview

Flood Fill is a BFS-based (Breadth-First Search) pathfinding algorithm. It assigns each maze cell a **distance value** representing the minimum number of cells between it and the goal.

### How It Works

1. **Initialize** all cells with distance = ∞ (255)
2. **Set goal cells** (center 2×2 area) to distance = 0
3. **Enqueue** all goal cells
4. **BFS propagation**: For each cell in the queue:
   - For each of the 4 neighbors (N, E, S, W):
     - If no wall blocks the path AND the neighbor's distance > current + 1:
       - Update neighbor's distance = current + 1
       - Enqueue the neighbor
5. **Movement decision**: Robot moves to the adjacent accessible cell with the **lowest distance value**

### Pseudocode

```c
void flood_fill() {
    set_all_distances(INFINITY);

    for each goal_cell:
        distance[goal_cell] = 0;
        queue.push(goal_cell);

    while (!queue.empty()) {
        cell = queue.pop();

        for each direction in {N, E, S, W}:
            if no_wall(cell, direction):
                neighbor = cell + direction;
                if distance[neighbor] > distance[cell] + 1:
                    distance[neighbor] = distance[cell] + 1;
                    queue.push(neighbor);
    }
}
```

### Search vs Speed Run

| Phase | Behavior |
|-------|----------|
| **Search Run** | Explore unknown maze, update walls, recompute flood fill after each cell |
| **Speed Run** | Use known shortest path, higher speed, optimized motion profiles |

### Complexity

- **Time**: O(N²) where N = maze dimension (16 → 256 cells)
- **Space**: O(N²) for distance array + O(N²) worst-case queue

---

## 2. PID Controller

### Overview

A **Proportional-Integral-Derivative** controller provides closed-loop feedback control for motor speed and wall centering.

### Formula

```
error(t) = setpoint - measured_value

P = Kp × error(t)
I = Ki × Σ(error × dt)
D = Kd × (error(t) - error(t-1)) / dt

output = P + I + D
```

### Anti-Windup

The integral term is clamped to prevent accumulation when the system is saturated:

```c
if (i_term > out_max) {
    i_term = out_max;
    integral = out_max / ki;
}
```

### Applications in Micromouse

| Controller | Setpoint | Measurement | Output |
|-----------|----------|-------------|--------|
| Motor Speed PID | Target speed (mm/s) | Encoder speed | PWM duty |
| Wall Following PID | 0 (centered) | IR sensor error | Differential motor correction |
| Turn PID | Target angle (°) | Gyro yaw angle | Motor rotation speed |

### Tuning Guidelines

1. Start with Kp only (Ki = 0, Kd = 0)
2. Increase Kp until oscillation begins
3. Reduce Kp by ~30%
4. Add Kd to dampen oscillations
5. Add small Ki if steady-state error persists

---

## 3. Trapezoidal Motion Profile

### Overview

Instead of instantly jumping to target speed, the robot follows a trapezoidal velocity curve:

```
speed
  ^
  |       ┌───────────────── max speed
  |      /│                 \
  |     / │                  \
  |    /  │                   \
  |   /   │                    \
  |__/    │                     \___
  +──────────────────────────────────> distance
     accel     cruise       decel
```

### Parameters

| Parameter | Description |
|-----------|-------------|
| max_speed | Maximum velocity (mm/s) |
| acceleration | Rate of speed increase (mm/s²) |
| deceleration | Rate of speed decrease (mm/s²) |
| end_speed | Desired final speed (0 = stop) |

### Deceleration Distance

```
d_decel = (v_current² - v_end²) / (2 × deceleration)
```

The robot starts decelerating when remaining distance ≤ d_decel.
