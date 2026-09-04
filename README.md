# 🐭 Micro Mouse

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-STM32F103C8T6-blue)]()
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](CONTRIBUTING.md)
[![GitHub Issues](https://img.shields.io/github/issues/debosmita74/micro_mouse)](https://github.com/debosmita74/micro_mouse/issues)
[![GitHub Stars](https://img.shields.io/github/stars/debosmita74/micro_mouse)](https://github.com/debosmita74/micro_mouse)

---

## 📋 Table of Contents

- [Description](#description)
- [Features](#features)
- [System Architecture](#system-architecture)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Circuit Diagram](#circuit-diagram)
- [Pin Mapping](#pin-mapping)
- [Project Structure](#project-structure)
- [Installation](#installation)
- [Usage](#usage)
- [Algorithms](#algorithms)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgements](#acknowledgements)

---

## 📖 Description

**Micro Mouse** is an autonomous maze-solving robot designed to navigate a standard 16×16 cell maze, map the environment, compute the shortest path using the **Flood Fill algorithm**, and execute a high-speed optimized run. This project provides the complete architecture — hardware design, firmware source code, circuit diagrams, PID control, and a Python maze simulator.

Built on the **STM32F103C8T6** (ARM Cortex-M3) microcontroller with a **TB6612FNG** dual motor driver, **N20 micro gear motors** with quadrature encoders, **custom IR wall sensors**, and an **MPU6050 IMU**.

> Designed to comply with classic Micromouse competition rules (NTF standard).

---

## ✨ Features

- ✅ **Flood Fill Algorithm** — BFS-based maze solving with dynamic wall updates
- ✅ **PID Motor Control** — Closed-loop speed control using quadrature encoders
- ✅ **Wall Following** — IR sensor-based corridor centering with PID correction
- ✅ **Motion Profiling** — Trapezoidal velocity profiles for smooth acceleration/deceleration
- ✅ **IMU Integration** — MPU6050 gyroscope for accurate turn correction
- ✅ **State Machine** — Clean operational flow: Idle → Calibration → Search → Speed Run
- ✅ **Battery Monitoring** — Low-voltage detection via ADC
- ✅ **Python Maze Simulator** — Visualize and test flood fill algorithm offline
- 🚧 **Diagonal Movement** — Planned for future optimization
- 🚧 **A* Pathfinding** — Alternative solver under development

---

## 🏗️ System Architecture

```
Micromouse Robot
│
├── Mechanical System
│   ├── Compact chassis (< 25cm × 25cm)
│   ├── Differential drive (2 × N20 motors)
│   ├── Rubber wheels (~25mm diameter)
│   └── Front/diagonal sensor mounts
│
├── Electronics
│   ├── STM32F103C8T6 MCU (ARM Cortex-M3, 72MHz)
│   ├── TB6612FNG dual H-bridge motor driver
│   ├── N20 gear motors with quadrature encoders
│   ├── 4× Custom IR wall sensors (emitter + phototransistor)
│   ├── MPU6050 6-axis IMU (I2C)
│   ├── 1S LiPo battery (3.7V, 300-500mAh)
│   ├── AMS1117-3.3 voltage regulator
│   └── Battery voltage divider → ADC
│
├── Firmware (C, bare-metal / HAL)
│   ├── Hardware Abstraction Layer (motor, encoder, sensor, IMU, battery)
│   ├── Control Layer (speed PID, wall-following PID, turn controller)
│   └── Application Layer (state machine, maze solver, motion planner)
│
└── Simulation (Python)
    ├── Flood fill visualizer
    └── Maze editor & path analyzer
```

---

## 🔩 Hardware Requirements

| Component | Specification | Quantity |
|-----------|--------------|----------|
| Microcontroller | STM32F103C8T6 (Blue Pill) | 1 |
| Motor Driver | TB6612FNG Dual H-Bridge | 1 |
| DC Motors | N20 Micro Gear Motor w/ Encoder (6V, ~300RPM) | 2 |
| IR Emitters | 940nm IR LED | 4 |
| IR Receivers | Phototransistor / Photodiode | 4 |
| IMU | MPU6050 (GY-521 breakout) | 1 |
| Battery | 1S LiPo 3.7V 300–500mAh | 1 |
| Voltage Regulator | AMS1117-3.3V | 1 |
| Wheels | ~25mm diameter rubber wheels | 2 |
| Caster / Skid | Ball caster or PTFE skid | 1 |
| Resistors | Various (see BOM) | — |
| Capacitors | 100nF ceramic + 10µF electrolytic | — |
| N-MOSFET | 2N7000 or BSS138 (for IR LED drive) | 4 |
| PCB / Perfboard | Custom PCB or prototype board | 1 |

> Full BOM: [`hardware/BOM.csv`](hardware/BOM.csv)

---

## 💻 Software Requirements

| Tool | Version | Purpose |
|------|---------|---------|
| GCC ARM Toolchain | >= 10.3 | Cross-compilation |
| STM32CubeMX | >= 6.x | MCU configuration & code generation |
| OpenOCD | >= 0.11 | Flashing & debugging |
| Make | >= 4.0 | Build automation |
| Python | >= 3.8 | Maze simulation |
| Git | >= 2.30 | Version control |

---

## ⚡ Circuit Diagram

> Full resolution: [`docs/images/circuit_diagram.png`](docs/images/circuit_diagram.png)

```
                            ┌──────────────────────────────┐
       1S LiPo              │       STM32F103C8T6          │
      ┌──────┐              │                              │
      │ 3.7V ├──┬──[SW]──┬──┤ 3.3V (via AMS1117)          │
      └──────┘  │         │  │                              │
                │     ┌───┘  │  PA0 ◄── IR Sensor FL (ADC) │
                │     │      │  PA1 ◄── IR Sensor FR (ADC) │
                │  AMS1117   │  PA2 ◄── IR Sensor DL (ADC) │
                │  3.3V REG  │  PA3 ◄── IR Sensor DR (ADC) │
                │     │      │  PA4 ◄── Battery VMON (ADC) │
                │     └──┐   │                              │
                │        │   │  PA6 ◄── Left Enc A  (TIM3) │
                │      3.3V  │  PA7 ◄── Left Enc B  (TIM3) │
                │        │   │  PB6 ◄── Right Enc A (TIM4) │
                │        │   │  PB7 ◄── Right Enc B (TIM4) │
                │        │   │                              │
                │        │   │  PA8 ──► PWMA (TIM1_CH1)    │
                │        │   │  PA9 ──► PWMB (TIM1_CH2)    │
    ┌───────────┤        │   │  PB12 ──► AIN1              │
    │           │        │   │  PB13 ──► AIN2              │
    │     ┌─────┤        │   │  PB14 ──► BIN1              │
    │     │     │        │   │  PB15 ──► BIN2              │
    │     │     │        │   │  PB5  ──► STBY              │
    │     │     │        │   │                              │
    │     │     │        │   │  PB10 ──► I2C2_SCL (MPU6050)│
    │     │     │        │   │  PB11 ◄─► I2C2_SDA (MPU6050)│
    │     │     │        │   │                              │
    │     │     │        │   │  PB8  ──► IR Emitter EN 1   │
    │     │     │        │   │  PB9  ──► IR Emitter EN 2   │
    │     │     │        │   │                              │
    │     │     │        │   │  PB0  ──► Status LED        │
    │     │     │        │   │  PC13 ◄── User Button       │
    │     │     │        │   │                              │
    │     │     │        │   │  PA13 ──► SWDIO (Debug)     │
    │     │     │        │   │  PA14 ──► SWCLK (Debug)     │
    │     │     │        │   └──────────────────────────────┘
    │     │     │        │
    │  ┌──┴──┐  │     ┌──┴───────┐        ┌─────────────┐
    │  │TB6612│  │     │ MPU6050  │        │ IR Sensors  │
    │  │FNG   │  │     │ (GY-521) │        │ ×4 channels │
    │  │      │  │     │          │        │             │
    │  │ VM◄──┘  │     │ VCC◄─3.3V│        │ LED+MOSFET  │
    │  │VCC◄─3.3V│     │ SCL──PB10│        │ ──► Wall    │
    │  │STBY◄PB5 │     │ SDA──PB11│        │             │
    │  │      │  │     │ GND──GND │        │ Phototrans  │
    │  │AO1───┼──┼──►  Left Motor │        │ ──► ADC     │
    │  │AO2───┘  │     └──────────┘        └─────────────┘
    │  │BO1──────┼──►  Right Motor
    │  │BO2──────┘
    │  └─────┘
    │
   GND (common)
```

---

## 📌 Pin Mapping

### Motor Driver (TB6612FNG)

| Function | STM32 Pin | Peripheral | TB6612 Pin |
|----------|-----------|------------|------------|
| Left Motor PWM | PA8 | TIM1_CH1 | PWMA |
| Left Motor IN1 | PB12 | GPIO | AIN1 |
| Left Motor IN2 | PB13 | GPIO | AIN2 |
| Right Motor PWM | PA9 | TIM1_CH2 | PWMB |
| Right Motor IN1 | PB14 | GPIO | BIN1 |
| Right Motor IN2 | PB15 | GPIO | BIN2 |
| Standby | PB5 | GPIO | STBY |

### Encoders

| Function | STM32 Pin | Peripheral |
|----------|-----------|------------|
| Left Encoder A | PA6 | TIM3_CH1 |
| Left Encoder B | PA7 | TIM3_CH2 |
| Right Encoder A | PB6 | TIM4_CH1 |
| Right Encoder B | PB7 | TIM4_CH2 |

### IR Wall Sensors (ADC)

| Sensor | STM32 Pin | ADC Channel |
|--------|-----------|-------------|
| Front-Left | PA0 | ADC1_IN0 |
| Front-Right | PA1 | ADC1_IN1 |
| Diagonal-Left | PA2 | ADC1_IN2 |
| Diagonal-Right | PA3 | ADC1_IN3 |

### IMU (MPU6050 — I2C2)

| MPU6050 Pin | STM32 Pin |
|-------------|-----------|
| SCL | PB10 |
| SDA | PB11 |
| VCC | 3.3V |
| GND | GND |

### Misc

| Function | STM32 Pin |
|----------|-----------|
| Battery Voltage Monitor | PA4 (ADC1_IN4) |
| Status LED | PB0 |
| User Button | PC13 |
| IR Emitter Enable 1 | PB8 |
| IR Emitter Enable 2 | PB9 |
| SWDIO | PA13 |
| SWCLK | PA14 |

---

## 📁 Project Structure

```
micro_mouse/
├── README.md
├── LICENSE
├── .gitignore
├── CONTRIBUTING.md
├── CODE_OF_CONDUCT.md
├── CHANGELOG.md
├── CMakeLists.txt
├── Makefile
├── requirements.txt
│
├── src/                       # Firmware source code
│   ├── main.c                 # Entry point & state machine
│   ├── motor.c                # Motor driver (TB6612FNG)
│   ├── encoder.c              # Quadrature encoder interface
│   ├── sensor.c               # IR wall sensor reading & calibration
│   ├── imu.c                  # MPU6050 I2C driver
│   ├── pid.c                  # PID controller
│   ├── maze.c                 # Maze data structure & flood fill
│   ├── motion.c               # Motion profiling & navigation
│   └── battery.c              # Battery voltage monitoring
│
├── include/                   # Header files
│   ├── config.h               # Global configuration & pin definitions
│   ├── motor.h
│   ├── encoder.h
│   ├── sensor.h
│   ├── imu.h
│   ├── pid.h
│   ├── maze.h
│   ├── motion.h
│   └── battery.h
│
├── docs/                      # Documentation
│   ├── images/
│   │   └── .gitkeep
│   ├── ALGORITHMS.md          # Detailed algorithm explanations
│   ├── HARDWARE_GUIDE.md      # Hardware assembly guide
│   └── TROUBLESHOOTING.md     # Common issues & fixes
│
├── hardware/                  # Hardware design files
│   ├── BOM.csv                # Bill of Materials
│   ├── schematics/
│   │   └── .gitkeep
│   └── datasheets/
│       └── .gitkeep
│
├── tests/                     # Unit tests
│   ├── test_pid.c
│   └── test_maze.c
│
├── simulations/               # Python maze simulator
│   ├── maze_simulator.py
│   ├── flood_fill.py
│   └── mazes/
│       └── maze_16x16.txt
│
└── configs/                   # Configuration files
    └── openocd.cfg
```

---

## 🚀 Installation

### 1. Clone the Repository

```bash
git clone https://github.com/debosmita74/micro_mouse.git
cd micro_mouse
```

### 2. Install ARM Toolchain

```bash
# Ubuntu/Debian
sudo apt-get install gcc-arm-none-eabi openocd cmake make

# macOS (Homebrew)
brew install --cask gcc-arm-embedded
brew install openocd cmake
```

### 3. Install Python Dependencies (for simulation)

```bash
pip install -r requirements.txt
```

### 4. Build Firmware

```bash
mkdir build && cd build
cmake ..
make
```

### 5. Flash to STM32

```bash
make flash
# OR manually:
openocd -f configs/openocd.cfg -c "program build/micro_mouse.elf verify reset exit"
```

---

## 📦 Usage

### Running the Robot

1. Power on the robot (LiPo connected, power switch ON)
2. Wait for status LED to indicate ready
3. Press the user button to start the search run
4. Robot explores the maze and maps walls
5. After reaching the goal, robot returns to start
6. Press button again for the optimized speed run

### Running the Simulation

```bash
cd simulations
python maze_simulator.py
```

### Configuration

Edit `include/config.h` to adjust:
- PID tuning parameters (`KP_SPEED`, `KI_SPEED`, `KD_SPEED`)
- Sensor thresholds (`WALL_THRESHOLD_FRONT`, etc.)
- Motor speed limits (`MAX_SPEED`, `SEARCH_SPEED`)
- Maze dimensions (`MAZE_SIZE`)

---

## 🧠 Algorithms

### Flood Fill (BFS-based)

The primary maze-solving algorithm. Assigns distance values from goal to every reachable cell:

1. Initialize all cells with distance = ∞
2. Set goal cells (center 2×2) to distance = 0
3. BFS propagation through open passages
4. Robot moves toward the neighbor with lowest distance
5. On discovering new walls → recalculate flood fill
6. After full exploration → compute shortest path for speed run

> Detailed explanation: [`docs/ALGORITHMS.md`](docs/ALGORITHMS.md)

### PID Controller

```
error = setpoint - measured
P = Kp × error
I = Ki × Σ(error × dt)
D = Kd × Δerror / dt
output = P + I + D   (clamped to [min, max])
```

Applied to:
- **Motor speed control** — encoder feedback → PWM adjustment
- **Wall centering** — IR sensor difference → differential motor correction
- **Turn accuracy** — gyro yaw → rotation correction

---

## 🤝 Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

---

## 📄 License

This project is licensed under the **MIT License** — see [LICENSE](LICENSE) for details.

---

## 🙏 Acknowledgements

- [NTF Micromouse Rules](https://ntf.or.jp/mouse/rule/kitei_classic-EN.html) — Competition standard
- [STM32 Reference Manual](https://www.st.com/en/microcontrollers-microprocessors/stm32f103c8.html)
- [TB6612FNG Datasheet](https://toshiba.semicon-storage.com/eu/semiconductor/product/motor-driver-ics/brushed-dc-motor-driver-ics/detail.TB6612FNG.html)
- [MPU6050 Datasheet](https://invensense.tdk.com/products/motion-tracking/6-axis/mpu-6050/)

---

*Built with ❤️ by [Debosmita Paul](https://github.com/debosmita74)*
