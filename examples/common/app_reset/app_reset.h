/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <esp_err.h>

/** Register callbacks for Factory reset
 *
 * Register factory reset functionality on a button.
 *
 * @param[in] handle Button handle returned by iot_button_create().
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t app_reset_button_register(void *handle);
