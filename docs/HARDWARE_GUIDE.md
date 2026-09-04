# 🔧 Hardware Assembly Guide

Step-by-step guide for building the Micro Mouse robot.

---

## 1. Component Checklist

Before starting, verify you have all components listed in [`hardware/BOM.csv`](../hardware/BOM.csv).

---

## 2. Assembly Order

### Step 1: Prepare the Chassis
- Use a 3D-printed chassis or PCB-based chassis plate
- Ensure motor mounts are aligned and symmetric
- Target dimensions: 80–100mm wide × 90–120mm long

### Step 2: Mount Motors
- Press-fit or screw N20 gear motors into motor mounts
- Ensure both motors are oriented the same way
- Attach wheels firmly — check for wobble

### Step 3: Install Caster/Skid
- Mount ball caster or PTFE skid at the front
- Ensure the robot sits level on a flat surface

### Step 4: Solder the PCB / Wiring
- Follow the circuit diagram in the README
- Solder in this order:
  1. Voltage regulator (AMS1117-3.3V)
  2. STM32 microcontroller (Blue Pill board)
  3. TB6612FNG motor driver
  4. Encoder connections
  5. IR sensor circuits (LEDs + phototransistors + MOSFETs)
  6. MPU6050 breakout (I2C connections)
  7. Battery connector + power switch
  8. Status LED + button

### Step 5: IR Sensor Placement
- Mount 4 IR sensor pairs on the front edge
- Front-left and front-right: aimed straight ahead
- Diagonal-left and diagonal-right: aimed ~45° to sides
- Ensure sensors are below the 5cm wall height

### Step 6: Connect SWD Debugger
- Wire PA13 (SWDIO), PA14 (SWCLK), 3.3V, GND to ST-Link

---

## 3. Power-On Checklist

1. ☐ Check all solder joints for shorts
2. ☐ Verify 3.3V regulator output with multimeter
3. ☐ Connect battery through power switch
4. ☐ Verify STM32 boots (status LED)
5. ☐ Flash firmware via SWD
6. ☐ Test motors individually
7. ☐ Test encoder readings
8. ☐ Test IR sensor readings
9. ☐ Test IMU communication
10. ☐ Run sensor calibration

---

## 4. Common Issues

| Issue | Likely Cause | Fix |
|-------|-------------|-----|
| Motors don't spin | STBY pin LOW | Check PB5 is set HIGH |
| Robot curves when going straight | Motor speed mismatch | Tune PID gains |
| Sensors read 0 | Emitter not powered | Check MOSFET gate connections |
| IMU not detected | I2C wiring | Check pull-up resistors on SDA/SCL |
| Random resets | Battery voltage drop | Add bulk capacitor near motor driver |

See also: [TROUBLESHOOTING.md](TROUBLESHOOTING.md)
