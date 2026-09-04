# 🔍 Troubleshooting Guide

Common issues and their solutions.

---

## Motor Issues

### Motors don't spin
- Verify STBY pin (PB5) is set HIGH in firmware
- Check TB6612FNG VCC (3.3V) and VM (battery) connections
- Verify PWM output with oscilloscope or LED

### One motor spins wrong direction
- Swap motor terminal wires (AO1/AO2 or BO1/BO2)
- OR invert direction logic in `motor.c`

### Motors are noisy/jerky
- Add 100nF ceramic capacitor across motor terminals
- Add 10µF bulk capacitor near TB6612FNG VM pin

---

## Sensor Issues

### All sensors read 0
- Check IR emitter enable pins (PB8, PB9)
- Verify MOSFET gate resistor values
- Check IR LED current-limiting resistors

### Inconsistent readings
- Implement ambient light compensation (already in `sensor.c`)
- Check for reflections from robot chassis
- Ensure sensor mounting is rigid

### Wall detection unreliable
- Recalibrate thresholds in known maze cell
- Use serial output to log raw ADC values
- Check that sensor angle hits the wall, not the floor

---

## IMU Issues

### MPU6050 not detected (WHO_AM_I fails)
- Verify I2C connections: SCL=PB10, SDA=PB11
- Check 2.2kΩ–10kΩ pull-up resistors on SDA and SCL
- Verify 3.3V power to MPU6050
- Check AD0 pin: LOW = address 0x68, HIGH = 0x69

### Yaw drifts over time
- Run `imu_calibrate()` with robot completely stationary
- Increase calibration samples (currently 500)
- Ensure control loop timing is accurate

---

## Navigation Issues

### Robot overshoots cells
- Reduce SEARCH_SPEED
- Tune deceleration profile
- Check encoder counts per revolution

### Turns are inaccurate
- Calibrate WHEEL_TRACK_MM measurement
- Use gyro-assisted turns (IMU feedback)
- Tune KP_TURN, KD_TURN gains

### Robot gets stuck in a loop
- Ensure flood fill recalculates after every wall update
- Check that wall updates are symmetric (both cells)
- Verify coordinate system consistency

---

## Build Issues

### Compilation errors
- Ensure GCC ARM toolchain is installed: `arm-none-eabi-gcc --version`
- Check CMakeLists.txt paths
- Verify all header files are in `include/`

### Flash fails
- Check ST-Link connection (SWDIO, SWCLK, GND)
- Verify OpenOCD configuration in `configs/openocd.cfg`
- Try holding RESET while starting flash
