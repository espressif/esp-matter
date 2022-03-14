/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <esp_err.h>
#include <esp_matter.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Initialize ESP RainMaker
 *
 * This initializes the devices and params for RainMaker, corresponding to the endpoint and attributes. It also adds
 * RainMaker features like OTA, Scheduling, etc.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t app_rainmaker_init(void);

esp_err_t app_rainmaker_attribute_update(int endpoint_id, int cluster_id, int attribute_id, esp_matter_attr_val_t *val);

#ifdef __cplusplus
}
#endif
