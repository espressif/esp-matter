/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <esp_matter_attribute_utils.h>
#include <app_bridged_device.h>
#include <stdint.h>

#define MATTER_HUE_MAX_VALUE 254
#define MATTER_SATURATION_MAX_VALUE 254
#define MATTER_LEVEL_MAX_VALUE 254

#define STANDARD_TEMPERATURE_FACTOR 1000000

#define RMAKER_HUE_MAX_VALUE 360
#define RMAKER_SATURATION_MAX_VALUE 100
#define RMAKER_LEVEL_MAX_VALUE 100

esp_err_t rainmaker_bridge_attribute_update(uint16_t endpoint_id, uint32_t cluster_id,
                                            uint32_t attribute_id, esp_matter_attr_val_t *val);

/* Init Rainmaker */
void rainmaker_init();