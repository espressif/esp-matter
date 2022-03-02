/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief
 *
 * @return esp_err_t
 */
esp_err_t app_ble_mesh_init(void);

/**
 * @brief
 *
 * @param blemesh_addr
 * @param onoff
 *
 * @return esp_err_t
 */
esp_err_t app_ble_mesh_onoff_set(uint16_t blemesh_addr, bool onoff);

/**
 * @brief
 *
 * @param composition_data
 * @param blemesh_addr
 *
 * @return esp_err_t
 */
esp_err_t blemesh_bridge_match_bridged_onoff_light(uint8_t *composition_data, uint16_t blemesh_addr);

#ifdef __cplusplus
}
#endif
