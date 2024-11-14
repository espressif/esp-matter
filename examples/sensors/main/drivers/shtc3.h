/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

// This file implements the SHTC3 temperature and humidity sensor driver.
// This is implemented keeping the Matter requirements in mind.
//
// Datasheet: https://sensirion.com/media/documents/643F9C8E/63A5A436/Datasheet_SHTC3.pdf

#pragma once

#include <esp_err.h>

using shtc3_sensor_cb_t = void (*)(uint16_t endpoint_id, float value, void *user_data);

typedef struct {
    struct {
        // This callback functon will be called periodically to report the temperature.
        shtc3_sensor_cb_t cb = NULL;
        // endpoint_id associated with temperature sensor
        uint16_t endpoint_id;
    } temperature;

    struct {
        // This callback functon will be called periodically to report the humidity.
        shtc3_sensor_cb_t cb = NULL;
        // endpoint_id associated with humidity sensor
        uint16_t endpoint_id;
    } humidity;

    // user data
    void *user_data = NULL;

    // polling interval in milliseconds, defaults to 5000 ms
    uint32_t interval_ms = 5000;
} shtc3_sensor_config_t;

/**
 * @brief Initialize sensor driver. This function should be called only once
 *        When initializing, at least one callback should be provided, else it
 *        returns ESP_ERR_INVALID_ARG.
 *
 * @param config sensor configurations. This should last for the lifetime of the driver
 *               as driver layer do not make a copy of this object.
 *
 * @return esp_err_t - ESP_OK on success,
 *                     ESP_ERR_INVALID_ARG if config is NULL
 *                     ESP_ERR_INVALID_STATE if driver is already initialized
 *                     appropriate error code otherwise
 */
esp_err_t shtc3_sensor_init(shtc3_sensor_config_t *config);
