/**
 * @file    battery.c
 * @brief   Battery voltage monitoring via resistor divider + ADC.
 * @author  Debosmita Paul
 * @date    2026-09-04
 *
 * Battery voltage is read through a resistor divider on PA4 (ADC1_IN4).
 * V_adc = V_bat × R2 / (R1 + R2)
 */

#include "battery.h"
#include "config.h"

/* ── Initialization ─────────────────────────────────────── */
void battery_init(void)
{
    /*
     * TODO: Configure PA4 as analog input.
     * TODO: Configure ADC1 channel 4.
     */
}

/* ── Read Battery Voltage ───────────────────────────────── */
uint16_t battery_read_mv(void)
{
    /*
     * TODO: Trigger ADC conversion on channel 4.
     * uint16_t adc_raw = HAL_ADC_GetValue(&hadc1);
     */
    uint16_t adc_raw = 0; /* placeholder */

    /* Convert ADC to millivolts */
    /* ADC is 12-bit (0–4095), reference = 3300 mV */
    uint32_t adc_mv = ((uint32_t)adc_raw * 3300UL) / 4095UL;

    /* Scale back through divider: V_bat = V_adc × (R1 + R2) / R2 */
    uint32_t battery_mv = adc_mv * (BATTERY_R1 + BATTERY_R2) / BATTERY_R2;

    return (uint16_t)battery_mv;
}

/* ── Low Battery Check ──────────────────────────────────── */
bool battery_is_low(void)
{
    return (battery_read_mv() < BATTERY_LOW_MV);
}
