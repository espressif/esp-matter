/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include "esp_err.h"

/**
 * @brief Initializes the matter stack.
 *
 * @return
 *      - ESP_OK on success
 *
 */
esp_err_t app_matter_init(void);
