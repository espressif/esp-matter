// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <esp_matter_core.h>
#include <stdint.h>

#define ESP_MATTER_NONE_FEATURE_ID 0x0000
#define ESP_MATTER_ON_OFF_CLUSTER_LIGHTING_FEATURE_ID 0x0001
#define ESP_MATTER_LEVEL_CONTROL_CLUSTER_ON_OFF_FEATURE_ID 0x0001
#define ESP_MATTER_LEVEL_CONTROL_CLUSTER_LIGHTING_FEATURE_ID 0x0002
#define ESP_MATTER_COLOR_CONTROL_CLUSTER_HUE_SATURATION_FEATURE_ID 0x0001

#define ON_OFF_CLUSTER_LIGHTING_CONFIG_DEFAULT()    \
    {                                               \
        .global_scene_control = 1,                  \
        .on_time = 0,                               \
        .off_wait_time = 0,                         \
        .start_up_on_off = 0,                       \
    }

#define LEVEL_CONTROL_CLUSTER_LIGHTING_CONFIG_DEFAULT()     \
    {                                                       \
        .remaining_time = 0,                                \
        .min_level = 1,                                     \
        .max_level = 254,                                   \
        .start_up_current_level = 0,                        \
    }

#define COLOR_CONTROL_CLUSTER_HUE_SATURATION_CONFIG_DEFAULT()   \
    {                                                           \
        .current_hue = 0,                                       \
        .current_saturation = 0,                                \
    }

typedef struct esp_matter_on_off_cluster_lighting_config {
    bool global_scene_control;
    uint16_t on_time;
    uint16_t off_wait_time;
    uint8_t start_up_on_off;
} esp_matter_on_off_cluster_lighting_config_t;

typedef struct esp_matter_level_control_cluster_lighting_config {
    uint16_t remaining_time;
    uint8_t min_level;
    uint8_t max_level;
    uint8_t start_up_current_level;
} esp_matter_level_control_cluster_lighting_config_t;

typedef struct esp_matter_color_control_cluster_hue_saturation_config {
    uint8_t current_hue;
    uint8_t current_saturation;
} esp_matter_color_control_cluster_hue_saturation_config_t;

esp_err_t esp_matter_on_off_cluster_add_feature_lighting(esp_matter_cluster_t *cluster,
                                                         esp_matter_on_off_cluster_lighting_config_t *config);

esp_err_t esp_matter_level_control_cluster_add_feature_on_off(esp_matter_cluster_t *cluster);
esp_err_t esp_matter_level_control_cluster_add_feature_lighting(esp_matter_cluster_t *cluster,
                                                            esp_matter_level_control_cluster_lighting_config_t *config);

esp_err_t esp_matter_color_control_cluster_add_feature_hue_saturation(esp_matter_cluster_t *cluster,
                                                    esp_matter_color_control_cluster_hue_saturation_config_t *config);
