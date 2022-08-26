/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <esp_matter_attribute_utils.h>
#include <esp_zigbee_api_HA_standard.h>
#include <esp_zigbee_api_core.h>
#include <stdint.h>

void zigbee_bridge_find_bridged_on_off_light_cb(zb_uint8_t zdo_status, zb_uint16_t addr, zb_uint8_t endpoint);

esp_err_t zigbee_bridge_attribute_update(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                                         esp_matter_attr_val_t *val);
