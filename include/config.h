/**
 * @file    config.h
 * @brief   Global configuration, pin definitions, and tuning constants.
 * @author  Debosmita Paul
 * @date    2026-09-04
 */

#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ── Maze Configuration ─────────────────────────────────── */
#define MAZE_SIZE           16      /**< Maze grid dimension (16×16)       */
#define CELL_SIZE_MM        180     /**< Cell size in mm (18 cm)           */
#define GOAL_X_MIN          7       /**< Goal area X start                 */
#define GOAL_X_MAX          8       /**< Goal area X end                   */
#define GOAL_Y_MIN          7       /**< Goal area Y start                 */
#define GOAL_Y_MAX          8       /**< Goal area Y end                   */

/* ── Wall Bit Masks ─────────────────────────────────────── */
#define WALL_N              0x01
#define WALL_E              0x02
#define WALL_S              0x04
#define WALL_W              0x08
#define CELL_VISITED        0x10

/* ── Motor Pins (TB6612FNG) ─────────────────────────────── */
#define MOTOR_L_PWM_PIN     GPIO_PIN_8   /* PA8  — TIM1_CH1  */
#define MOTOR_L_PWM_PORT    GPIOA
#define MOTOR_L_IN1_PIN     GPIO_PIN_12  /* PB12 — AIN1      */
#define MOTOR_L_IN1_PORT    GPIOB
#define MOTOR_L_IN2_PIN     GPIO_PIN_13  /* PB13 — AIN2      */
#define MOTOR_L_IN2_PORT    GPIOB

#define MOTOR_R_PWM_PIN     GPIO_PIN_9   /* PA9  — TIM1_CH2  */
#define MOTOR_R_PWM_PORT    GPIOA
#define MOTOR_R_IN1_PIN     GPIO_PIN_14  /* PB14 — BIN1      */
#define MOTOR_R_IN1_PORT    GPIOB
#define MOTOR_R_IN2_PIN     GPIO_PIN_15  /* PB15 — BIN2      */
#define MOTOR_R_IN2_PORT    GPIOB

#define MOTOR_STBY_PIN      GPIO_PIN_5   /* PB5  — STBY      */
#define MOTOR_STBY_PORT     GPIOB

/* ── Encoder Pins ───────────────────────────────────────── */
#define ENC_L_A_PIN         GPIO_PIN_6   /* PA6  — TIM3_CH1  */
#define ENC_L_B_PIN         GPIO_PIN_7   /* PA7  — TIM3_CH2  */
#define ENC_R_A_PIN         GPIO_PIN_6   /* PB6  — TIM4_CH1  */
#define ENC_R_B_PIN         GPIO_PIN_7   /* PB7  — TIM4_CH2  */

/* ── Encoder Constants ──────────────────────────────────── */
#define ENCODER_CPR         12      /**< Counts per motor revolution       */
#define GEAR_RATIO          100     /**< Motor gear ratio                  */
#define WHEEL_DIAMETER_MM   25      /**< Wheel diameter in mm              */
#define WHEEL_TRACK_MM      75      /**< Distance between wheel centers    */
#define TICKS_PER_REV       (ENCODER_CPR * GEAR_RATIO)  /**< Total ticks per wheel rev */

/* ── IR Sensor Pins (ADC) ───────────────────────────────── */
#define SENSOR_FL_PIN       GPIO_PIN_0   /* PA0  — ADC1_IN0  */
#define SENSOR_FR_PIN       GPIO_PIN_1   /* PA1  — ADC1_IN1  */
#define SENSOR_DL_PIN       GPIO_PIN_2   /* PA2  — ADC1_IN2  */
#define SENSOR_DR_PIN       GPIO_PIN_3   /* PA3  — ADC1_IN3  */

#define IR_EMITTER_1_PIN    GPIO_PIN_8   /* PB8              */
#define IR_EMITTER_1_PORT   GPIOB
#define IR_EMITTER_2_PIN    GPIO_PIN_9   /* PB9              */
#define IR_EMITTER_2_PORT   GPIOB

/* ── IR Sensor Thresholds ───────────────────────────────── */
#define WALL_THRESHOLD_FL   800     /**< Front-left wall detection         */
#define WALL_THRESHOLD_FR   800     /**< Front-right wall detection        */
#define WALL_THRESHOLD_DL   600     /**< Diagonal-left wall detection      */
#define WALL_THRESHOLD_DR   600     /**< Diagonal-right wall detection     */
#define FRONT_STOP_VALUE    2500    /**< Front wall stopping distance      */

/* ── Wall Following Setpoints ───────────────────────────── */
#define WALL_SETPOINT_LEFT  1200    /**< Target ADC for left wall center   */
#define WALL_SETPOINT_RIGHT 1200    /**< Target ADC for right wall center  */

/* ── IMU (MPU6050) ──────────────────────────────────────── */
#define MPU6050_ADDR        0x68    /**< I2C address (AD0 = LOW)           */
#define I2C_SCL_PIN         GPIO_PIN_10  /* PB10 — I2C2_SCL  */
#define I2C_SDA_PIN         GPIO_PIN_11  /* PB11 — I2C2_SDA  */

/* ── Battery Monitoring ─────────────────────────────────── */
#define BATTERY_ADC_PIN     GPIO_PIN_4   /* PA4  — ADC1_IN4  */
#define BATTERY_LOW_MV      3200    /**< Low battery threshold (mV)        */
#define BATTERY_R1          10000   /**< Divider upper resistor (Ω)        */
#define BATTERY_R2          10000   /**< Divider lower resistor (Ω)        */

/* ── PID Tuning ─────────────────────────────────────────── */
#define KP_SPEED            2.0f    /**< Speed PID proportional gain       */
#define KI_SPEED            0.1f    /**< Speed PID integral gain           */
#define KD_SPEED            0.5f    /**< Speed PID derivative gain         */

#define KP_WALL             0.8f    /**< Wall-follow PID proportional gain */
#define KI_WALL             0.0f    /**< Wall-follow PID integral gain     */
#define KD_WALL             0.3f    /**< Wall-follow PID derivative gain   */

#define KP_TURN             3.0f    /**< Turn PID proportional gain        */
#define KI_TURN             0.0f    /**< Turn PID integral gain            */
#define KD_TURN             1.0f    /**< Turn PID derivative gain          */

#define PID_OUTPUT_MIN     -1000
#define PID_OUTPUT_MAX      1000

/* ── Motion Profile ─────────────────────────────────────── */
#define MAX_SPEED           500     /**< Maximum speed (mm/s)              */
#define SEARCH_SPEED        200     /**< Search run speed (mm/s)           */
#define TURN_SPEED          150     /**< In-place turn speed (mm/s)        */
#define ACCELERATION        1000    /**< Acceleration (mm/s²)              */
#define DECELERATION        1000    /**< Deceleration (mm/s²)              */

/* ── Control Loop ───────────────────────────────────────── */
#define CONTROL_FREQ_HZ     1000    /**< Main control loop frequency       */
#define CONTROL_DT          (1.0f / CONTROL_FREQ_HZ)

/* ── Misc Pins ──────────────────────────────────────────── */
#define LED_PIN             GPIO_PIN_0   /* PB0  — Status LED */
#define LED_PORT            GPIOB
#define BUTTON_PIN          GPIO_PIN_13  /* PC13 — User button */
#define BUTTON_PORT         GPIOC

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_H */
