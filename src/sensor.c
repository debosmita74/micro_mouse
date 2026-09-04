/**
 * @file    sensor.c
 * @brief   IR wall sensor driver with ambient light compensation.
 * @author  Debosmita Paul
 * @date    2026-09-04
 *
 * 4-channel IR wall sensors:
 *   Front-Left   (PA0, ADC1_IN0)
 *   Front-Right  (PA1, ADC1_IN1)
 *   Diag-Left    (PA2, ADC1_IN2)
 *   Diag-Right   (PA3, ADC1_IN3)
 *
 * IR emitters driven via MOSFET on PB8 and PB9.
 *
 * Reading procedure:
 *   1. Emitter OFF → read ambient
 *   2. Emitter ON  → read reflected
 *   3. Signal = reflected - ambient
 */

#include "sensor.h"
#include "config.h"

/* ── Calibration Values ─────────────────────────────────── */
static uint16_t cal_threshold[SENSOR_COUNT] = {
    WALL_THRESHOLD_FL,
    WALL_THRESHOLD_FR,
    WALL_THRESHOLD_DL,
    WALL_THRESHOLD_DR
};

/* ── Internal Helpers ───────────────────────────────────── */
static uint16_t adc_read_channel(uint8_t channel);
static void     emitter_on(uint8_t group);
static void     emitter_off(uint8_t group);
static void     delay_us(uint32_t us);

/* ── Initialization ─────────────────────────────────────── */
void sensor_init(void)
{
    /*
     * TODO: Configure PA0-PA3 as analog inputs.
     * TODO: Configure ADC1 with channels 0-3.
     * TODO: Configure PB8, PB9 as GPIO outputs (IR emitter enable).
     */

    emitter_off(0);
    emitter_off(1);
}

/* ── Read All Sensors ───────────────────────────────────── */
void sensor_read_all(SensorData *data)
{
    if (data == (void *)0) return;

    uint16_t ambient[SENSOR_COUNT];
    uint16_t active[SENSOR_COUNT];

    /* Step 1: Read ambient (emitters OFF) */
    emitter_off(0);
    emitter_off(1);
    delay_us(50);

    ambient[SENSOR_FRONT_LEFT]  = adc_read_channel(0);
    ambient[SENSOR_FRONT_RIGHT] = adc_read_channel(1);
    ambient[SENSOR_DIAG_LEFT]   = adc_read_channel(2);
    ambient[SENSOR_DIAG_RIGHT]  = adc_read_channel(3);

    /* Step 2: Read with front emitters ON */
    emitter_on(0);
    delay_us(100);

    active[SENSOR_FRONT_LEFT]  = adc_read_channel(0);
    active[SENSOR_FRONT_RIGHT] = adc_read_channel(1);

    emitter_off(0);

    /* Step 3: Read with diagonal emitters ON */
    emitter_on(1);
    delay_us(100);

    active[SENSOR_DIAG_LEFT]  = adc_read_channel(2);
    active[SENSOR_DIAG_RIGHT] = adc_read_channel(3);

    emitter_off(1);

    /* Step 4: Compute filtered signal */
    for (int i = 0; i < SENSOR_COUNT; i++) {
        data->raw[i] = active[i];

        if (active[i] > ambient[i]) {
            data->filtered[i] = active[i] - ambient[i];
        } else {
            data->filtered[i] = 0;
        }

        data->wall_detected[i] = (data->filtered[i] > cal_threshold[i]);
    }
}

/* ── Wall Detection Helpers ─────────────────────────────── */
bool sensor_front_wall(const SensorData *data)
{
    return data->wall_detected[SENSOR_FRONT_LEFT] &&
           data->wall_detected[SENSOR_FRONT_RIGHT];
}

bool sensor_left_wall(const SensorData *data)
{
    return data->wall_detected[SENSOR_DIAG_LEFT];
}

bool sensor_right_wall(const SensorData *data)
{
    return data->wall_detected[SENSOR_DIAG_RIGHT];
}

/* ── Wall Following Error ───────────────────────────────── */
int16_t sensor_get_wall_error(const SensorData *data)
{
    bool has_left  = data->wall_detected[SENSOR_DIAG_LEFT];
    bool has_right = data->wall_detected[SENSOR_DIAG_RIGHT];

    if (has_left && has_right) {
        /* Both walls: error = left - right (centered when equal) */
        return (int16_t)data->filtered[SENSOR_DIAG_LEFT] -
               (int16_t)data->filtered[SENSOR_DIAG_RIGHT];
    }
    else if (has_left) {
        /* Left wall only: error relative to setpoint */
        return (int16_t)data->filtered[SENSOR_DIAG_LEFT] -
               (int16_t)WALL_SETPOINT_LEFT;
    }
    else if (has_right) {
        /* Right wall only: inverted error relative to setpoint */
        return (int16_t)WALL_SETPOINT_RIGHT -
               (int16_t)data->filtered[SENSOR_DIAG_RIGHT];
    }

    /* No walls detected — no correction */
    return 0;
}

/* ── Calibration ────────────────────────────────────────── */
void sensor_calibrate(void)
{
    /*
     * TODO: Advanced calibration procedure:
     * 1. Place robot in a known cell (walls on both sides).
     * 2. Read sensor values at known distances.
     * 3. Store calibrated thresholds and setpoints.
     *
     * For now, using compile-time defaults from config.h.
     */
}

/* ── Internal: ADC Read ─────────────────────────────────── */
static uint16_t adc_read_channel(uint8_t channel)
{
    (void)channel;
    /*
     * TODO: Configure ADC1 to read the specified channel.
     * TODO: Start conversion and wait for completion.
     * return HAL_ADC_GetValue(&hadc1);
     */
    return 0; /* placeholder */
}

/* ── Internal: Emitter Control ──────────────────────────── */
static void emitter_on(uint8_t group)
{
    if (group == 0) {
        /* TODO: HAL_GPIO_WritePin(IR_EMITTER_1_PORT, IR_EMITTER_1_PIN, GPIO_PIN_SET); */
    } else {
        /* TODO: HAL_GPIO_WritePin(IR_EMITTER_2_PORT, IR_EMITTER_2_PIN, GPIO_PIN_SET); */
    }
}

static void emitter_off(uint8_t group)
{
    if (group == 0) {
        /* TODO: HAL_GPIO_WritePin(IR_EMITTER_1_PORT, IR_EMITTER_1_PIN, GPIO_PIN_RESET); */
    } else {
        /* TODO: HAL_GPIO_WritePin(IR_EMITTER_2_PORT, IR_EMITTER_2_PIN, GPIO_PIN_RESET); */
    }
}

static void delay_us(uint32_t us)
{
    volatile uint32_t i;
    for (i = 0; i < us * 8; i++) {
        __asm__("nop");
    }
}
