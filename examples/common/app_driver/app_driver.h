/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <esp_err.h>

#define SRC_MAX_NAMELEN 20

/**
 * @brief The param update callbacks notify the registered sources that the param
 *        was updated by other source.
 *
 */
typedef struct app_driver_param_callback {
    void (*update_power)(bool power);                   /* Update power (On/Off) */
    void (*update_brightness)(uint8_t brightness);      /* Update brightness */
    void (*update_hue)(uint16_t hue);                   /* Update color hue */
    void (*update_saturation)(uint8_t saturation);      /* Update color saturation */
    void (*update_temperature)(uint32_t temperature);   /* Update color temperature */
} app_driver_param_callback_t;

/**
 * @brief Initializes the application driver layer.
 *
 * @return
 *      - ESP_OK on success
 *
 */
esp_err_t app_driver_init(void);

/**
 * @brief Register a control source.
 *
 * @param[in]  name       The source name.
 * @param[in]  callbacks  The param update callbacks.
 *
 * @note Each source can just register the param callbacks it cares about, and leave the others as NULL.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if any of the param is invalid
 *      - ESP_ERR_NO_MEM if register failed due to out of memory
 *
 */
esp_err_t app_driver_register_src(const char *name, app_driver_param_callback_t *callbacks);

/**
 * @brief Update the power (On/Off) param and report to all other registered control sources.
 *
 * @param[in]  power   The new power value.
 * @param[in]  src     The source that the change comes from.
 *
 * @return
 *      - ESP_OK on success
 *
 */
esp_err_t app_driver_update_and_report_power(bool power, const char *src);

/**
 * @brief Update the brightness param and report to all other registered control sources.
 *
 * @param[in]  brightness   The new brightness value.
 * @param[in]  src          The source that the change comes from.
 *
 * @return
 *      - ESP_OK on success
 *
 */
esp_err_t app_driver_update_and_report_brightness(uint8_t brightness, const char *src);

/**
 * @brief Update the hue param and report to all other registered control sources.
 *
 * @param[in]  hue      The new hue value.
 * @param[in]  src      The source that the change comes from.
 *
 * @return
 *      - ESP_OK on success
 *
 */
esp_err_t app_driver_update_and_report_hue(uint16_t hue, const char *src);

/**
 * @brief Update the saturation param and report to all other registered control sources.
 *
 * @param[in] saturation    The new saturation value.
 * @param[in] src           The source that the change comes from.
 *
 * @return
 *      - ESP_OK on success
 *
 */
esp_err_t app_driver_update_and_report_saturation(uint8_t saturation, const char *src);

/**
 * @brief Update the color temperature param and report to all other registered control sources.
 *
 * @param[in]  temperature   The new temperature value.
 * @param[in]  src           The source that the change comes from.
 *
 * return
 *     - ESP_OK on success
 *
 */
esp_err_t app_driver_update_and_report_temperature(uint32_t temperature, const char *src);

/**
 * @brief Get the power (On/Off) value.
 *
 * @return The current power (On/Off) value.
 *
 */
bool app_driver_get_power(void);

/**
 * @brief Get the brightness value.
 *
 * @return The current brightness value.
 *
 */
uint8_t app_driver_get_brightness(void);

/**
 * @brief Get the hue value.
 *
 * @return The current hue value.
 *
 */
uint16_t app_driver_get_hue(void);

/**
 * @brief Get the saturation value.
 *
 * @return The current saturation value.
 *
 */
uint8_t app_driver_get_saturation(void);

/**
 * @brief Get the temperature value.
 *
 * @return The current temperature value.
 *
 */
uint32_t app_driver_get_temperature(void);

#ifdef __cplusplus
}
#endif
