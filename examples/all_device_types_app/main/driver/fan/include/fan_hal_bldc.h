/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include "bldc_control.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S3
#define DEFAULT_SPEED_MAX 100
#define DEFAULT_SPEED2MOTOR 10

#define LOW_MODE_PERCENT 33
#define MED_MODE_PERCENT 66
#define HIGH_MODE_PERCENT 100

typedef struct {
    uint8_t is_start;      /*!< motor status 0:off 1:start */
    uint8_t is_natural;    /*!< motor natural wind 0: off 1: start */
    uint16_t target_speed; /*!< motor target speed */
    uint16_t min_speed;    /*!< motor min speed */
    uint16_t max_speed;    /*!< motor max speed */
} motor_parameter_t;

/**
 * @brief variable init
 *
 */
void bldc_variable_init();

/**
 * @brief bldc init
 *
 * @param direction
 * @return esp_err_t
 */
esp_err_t hal_bldc_init(dir_enum_t direction);

/**
 * @brief set bldc start or stop
 *
 * @param status
 * @return esp_err_t
 */
esp_err_t hal_bldc_start_stop(uint8_t status);

/**
 * @brief set bldc fan mode
 *
 * @param status
 * @return esp_err_t
 */
esp_err_t hal_bldc_set_fanmode(uint8_t status);

/**
 * @brief set bldc speed through percent
 *
 * @param speed
 * @return esp_err_t
 */
esp_err_t hal_bldc_set_percent_speed(uint16_t speed);

/**
 * @brief set bldc wind mode
 *
 * @param status
 * @return esp_err_t
 */
esp_err_t hal_bldc_set_wind(uint8_t status);

/**
 * @brief set bldc step
 *
 * @param status, aWrap, aLowestOff
 * @return esp_err_t
 */
esp_err_t hal_bldc_set_step(uint8_t status,bool aWrap,bool aLowestOff);

/**
 * @brief set bldc direction
 *
 * @param status
 * @return esp_err_t
 */
esp_err_t hal_bldc_set_airflow_direction(bool status);

/**
 * @brief set bldc speed
 *
 * @param speed
 * @return esp_err_t
 */
esp_err_t hal_bldc_set_speed(uint16_t speed);

/**
 * @brief read and return bldc motor status
 *
 * @return esp_err_t
 */
esp_err_t hal_bldc_status();

/**
 * @brief read and return bldc motor speed
 *
 * @return esp_err_t
 */
esp_err_t hal_bldc_speed();

#ifdef __cplusplus
}
#endif
#endif
