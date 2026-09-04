/**
 * @file    imu.c
 * @brief   MPU6050 IMU driver over I2C2.
 * @author  Debosmita Paul
 * @date    2026-09-04
 *
 * Provides gyroscope yaw integration for turn accuracy.
 * Connected via I2C2: SCL=PB10, SDA=PB11.
 */

#include "imu.h"
#include "config.h"

/* ── MPU6050 Register Addresses ─────────────────────────── */
#define MPU6050_REG_PWR_MGMT_1    0x6B
#define MPU6050_REG_GYRO_CONFIG   0x1B
#define MPU6050_REG_ACCEL_CONFIG  0x1C
#define MPU6050_REG_WHO_AM_I      0x75
#define MPU6050_REG_ACCEL_XOUT_H  0x3B
#define MPU6050_REG_GYRO_XOUT_H   0x43

/* ── Gyro Sensitivity ───────────────────────────────────── */
/* At ±250°/s range: 131 LSB per °/s */
#define GYRO_SENSITIVITY  131.0f

/* ── Internal State ─────────────────────────────────────── */
static float yaw_angle    = 0.0f;
static float gyro_z_bias  = 0.0f;
static bool  initialized  = false;

/* ── I2C Stubs ──────────────────────────────────────────── */
static bool    i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t data);
static bool    i2c_read_reg(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len);

/* ── Initialization ─────────────────────────────────────── */
bool imu_init(void)
{
    /*
     * TODO: Initialize I2C2 peripheral.
     * SCL = PB10, SDA = PB11
     * Speed: 400 kHz (Fast mode)
     */

    /* Check WHO_AM_I register */
    uint8_t who = 0;
    if (!i2c_read_reg(MPU6050_ADDR, MPU6050_REG_WHO_AM_I, &who, 1)) {
        return false;
    }
    if (who != 0x68) {
        return false;
    }

    /* Wake up MPU6050 (clear sleep bit) */
    i2c_write_reg(MPU6050_ADDR, MPU6050_REG_PWR_MGMT_1, 0x00);

    /* Set gyro range to ±250°/s */
    i2c_write_reg(MPU6050_ADDR, MPU6050_REG_GYRO_CONFIG, 0x00);

    /* Set accelerometer range to ±2g */
    i2c_write_reg(MPU6050_ADDR, MPU6050_REG_ACCEL_CONFIG, 0x00);

    initialized = true;
    return true;
}

/* ── Read IMU Data ──────────────────────────────────────── */
void imu_read(ImuData *data)
{
    if (!initialized || data == (void *)0) return;

    uint8_t buf[14];

    /* Read 14 bytes starting from ACCEL_XOUT_H */
    if (!i2c_read_reg(MPU6050_ADDR, MPU6050_REG_ACCEL_XOUT_H, buf, 14)) {
        return;
    }

    /* Parse accelerometer (big-endian) */
    data->accel_x = (int16_t)((buf[0]  << 8) | buf[1]);
    data->accel_y = (int16_t)((buf[2]  << 8) | buf[3]);
    data->accel_z = (int16_t)((buf[4]  << 8) | buf[5]);
    /* buf[6], buf[7] = temperature (skipped) */

    /* Parse gyroscope */
    data->gyro_x  = (int16_t)((buf[8]  << 8) | buf[9]);
    data->gyro_y  = (int16_t)((buf[10] << 8) | buf[11]);
    data->gyro_z  = (int16_t)((buf[12] << 8) | buf[13]);

    /* Convert gyro Z to deg/s and remove bias */
    data->yaw_rate = ((float)data->gyro_z / GYRO_SENSITIVITY) - gyro_z_bias;

    /* Integrate yaw */
    yaw_angle += data->yaw_rate * CONTROL_DT;
    data->yaw = yaw_angle;
}

/* ── Reset Yaw ──────────────────────────────────────────── */
void imu_reset_yaw(void)
{
    yaw_angle = 0.0f;
}

/* ── Get Yaw ────────────────────────────────────────────── */
float imu_get_yaw(void)
{
    return yaw_angle;
}

/* ── Calibrate Gyro Bias ────────────────────────────────── */
void imu_calibrate(void)
{
    if (!initialized) return;

    float sum = 0.0f;
    const int samples = 500;

    for (int i = 0; i < samples; i++) {
        uint8_t buf[2];
        if (i2c_read_reg(MPU6050_ADDR, MPU6050_REG_GYRO_XOUT_H + 4, buf, 2)) {
            int16_t gz = (int16_t)((buf[0] << 8) | buf[1]);
            sum += (float)gz / GYRO_SENSITIVITY;
        }
        /* Small delay between samples */
        volatile uint32_t d;
        for (d = 0; d < 7200; d++) { __asm__("nop"); }
    }

    gyro_z_bias = sum / (float)samples;
}

/* ── I2C Stubs (replace with HAL) ───────────────────────── */
static bool i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t data)
{
    (void)addr; (void)reg; (void)data;
    /*
     * TODO: HAL_I2C_Mem_Write(&hi2c2, addr << 1, reg,
     *       I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
     */
    return true;
}

static bool i2c_read_reg(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len)
{
    (void)addr; (void)reg; (void)buf; (void)len;
    /*
     * TODO: HAL_I2C_Mem_Read(&hi2c2, addr << 1, reg,
     *       I2C_MEMADD_SIZE_8BIT, buf, len, 100);
     */
    return true;
}
