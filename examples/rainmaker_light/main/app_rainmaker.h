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
 * This adds the custom RainMaker cluster.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t app_rainmaker_init(void);

/** Start ESP RainMaker
 *
 * This initializes the devices and params for RainMaker, corresponding to the endpoint and attributes. It also adds
 * RainMaker features like OTA, Scheduling, etc.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t app_rainmaker_start(void);

/** ESP RainMaker Update
 *
 * This API should be called to update ESP RainMaker for the attribute being updated.
 * This is usually called from the common `app_attribute_update_cb()`.
 *
 * @param[in] endpoint_id Endpoint ID of the attribute.
 * @param[in] cluster_id Cluster ID of the attribute.
 * @param[in] attribute_id Attribute ID of the attribute.
 * @param[in] val Pointer to `esp_matter_attr_val_t`. Use appropriate elements as per the value type.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t app_rainmaker_attribute_update(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                                         esp_matter_attr_val_t *val);

#ifdef __cplusplus
}
#endif
