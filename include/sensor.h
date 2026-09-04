/**
 * @file    sensor.h
 * @brief   IR wall sensor driver and calibration.
 */

#ifndef SENSOR_H
#define SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/** @brief Sensor channel identifiers. */
typedef enum {
    SENSOR_FRONT_LEFT,
    SENSOR_FRONT_RIGHT,
    SENSOR_DIAG_LEFT,
    SENSOR_DIAG_RIGHT,
    SENSOR_COUNT
} SensorChannel;

/** @brief Processed sensor readings. */
typedef struct {
    uint16_t raw[SENSOR_COUNT];         /**< Raw ADC values                */
    uint16_t filtered[SENSOR_COUNT];    /**< Filtered (ambient-compensated)*/
    bool     wall_detected[SENSOR_COUNT]; /**< Wall present flags          */
} SensorData;

/**
 * @brief  Initialize ADC and IR emitter GPIO.
 */
void sensor_init(void);

/**
 * @brief  Read all sensors with ambient compensation.
 * @param[out] data  Pointer to SensorData struct to fill.
 */
void sensor_read_all(SensorData *data);

/**
 * @brief  Check if front wall is present.
 * @param  data  Pointer to current sensor data.
 * @return true if front wall detected.
 */
bool sensor_front_wall(const SensorData *data);

/**
 * @brief  Check if left wall is present.
 * @param  data  Pointer to current sensor data.
 * @return true if left wall detected.
 */
bool sensor_left_wall(const SensorData *data);

/**
 * @brief  Check if right wall is present.
 * @param  data  Pointer to current sensor data.
 * @return true if right wall detected.
 */
bool sensor_right_wall(const SensorData *data);

/**
 * @brief  Get wall-following error for PID correction.
 * @param  data  Pointer to current sensor data.
 * @return Signed error value (negative = too far left).
 */
int16_t sensor_get_wall_error(const SensorData *data);

/**
 * @brief  Calibrate sensor thresholds. Place robot in known position.
 */
void sensor_calibrate(void);

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_H */
