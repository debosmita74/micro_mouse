/**
 * @file    imu.h
 * @brief   MPU6050 IMU driver (I2C).
 */

#ifndef IMU_H
#define IMU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/** @brief IMU data structure. */
typedef struct {
    int16_t accel_x;    /**< Accelerometer X (raw)  */
    int16_t accel_y;    /**< Accelerometer Y (raw)  */
    int16_t accel_z;    /**< Accelerometer Z (raw)  */
    int16_t gyro_x;     /**< Gyroscope X (raw)      */
    int16_t gyro_y;     /**< Gyroscope Y (raw)      */
    int16_t gyro_z;     /**< Gyroscope Z (raw)      */
    float   yaw;        /**< Integrated yaw (deg)   */
    float   yaw_rate;   /**< Yaw rate (deg/s)       */
} ImuData;

/**
 * @brief  Initialize MPU6050 over I2C2.
 * @return true on success, false if device not found.
 */
bool imu_init(void);

/**
 * @brief  Read all IMU axes and update yaw integration.
 * @param[out] data  Pointer to ImuData struct.
 */
void imu_read(ImuData *data);

/**
 * @brief  Reset integrated yaw angle to zero.
 */
void imu_reset_yaw(void);

/**
 * @brief  Get current integrated yaw angle.
 * @return Yaw in degrees.
 */
float imu_get_yaw(void);

/**
 * @brief  Calibrate gyroscope bias. Robot must be stationary.
 */
void imu_calibrate(void);

#ifdef __cplusplus
}
#endif

#endif /* IMU_H */
