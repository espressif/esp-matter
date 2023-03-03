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
#include <string.h>

#include <esp_log.h>

#include <esp_matter_attribute_utils.h>
#include <app_bridged_device.h>

/**
 * @brief
 *
 * @param endpoint_id
 * @param cluster_id
 * @param attribute_id
 * @param val
 *
 * @return esp_err_t
 */
esp_err_t blemesh_bridge_attribute_update(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                                          esp_matter_attr_val_t *val, app_bridged_device_t *bridged_device);

#ifdef __cplusplus
}
#endif
