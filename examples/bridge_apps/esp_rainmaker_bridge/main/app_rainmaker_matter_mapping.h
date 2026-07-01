/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <esp_err.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <esp_matter_attribute_utils.h>

#define APP_RAINMAKER_INVALID_MATTER_DEVICE_TYPE 0xFFFF

#define MATTER_HUE_MAX_VALUE 254
#define MATTER_SATURATION_MAX_VALUE 254
#define MATTER_LEVEL_MAX_VALUE 254

#define STANDARD_TEMPERATURE_FACTOR 1000000

#define RMAKER_HUE_MAX_VALUE 360
#define RMAKER_SATURATION_MAX_VALUE 100
#define RMAKER_LEVEL_MAX_VALUE 100

esp_err_t app_map_rainmaker_nodes_to_matter(char *nodes_json, size_t nodes_json_len, const char *group_id);
esp_err_t app_map_rainmaker_node_params_to_matter(const char *node_id, const char *payload, size_t payload_len);
esp_err_t app_map_rainmaker_node_connectivity_to_matter(const char *node_id, bool connected);
esp_err_t app_map_matter_attribute_to_rainmaker(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                                                esp_matter_attr_val_t *val);
