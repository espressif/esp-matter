// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
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

/* THIS IS A GENERATED FILE, DO NOT EDIT */

#pragma once
#include <esp_matter_data_model.h>

namespace esp_matter {
namespace cluster {
namespace camera_av_settings_user_level_management {

namespace feature {
namespace digital_ptz {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* digital_ptz */

namespace mechanical_pan {
typedef struct config {
    int16_t pan_min;
    int16_t pan_max;
    uint8_t movement_state;
    config() : pan_min(0), pan_max(1), movement_state(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* mechanical_pan */

namespace mechanical_tilt {
typedef struct config {
    int16_t tilt_min;
    int16_t tilt_max;
    uint8_t movement_state;
    config() : tilt_min(0), tilt_max(1), movement_state(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* mechanical_tilt */

namespace mechanical_zoom {
typedef struct config {
    uint8_t zoom_max;
    uint8_t movement_state;
    config() : zoom_max(2), movement_state(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* mechanical_zoom */

namespace mechanical_presets {
typedef struct config {
    uint8_t max_presets;
    config() : max_presets(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* mechanical_presets */

} /* feature */

namespace attribute {
attribute_t *create_mptz_position(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_max_presets(cluster_t *cluster, uint8_t value);
attribute_t *create_mptz_presets(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_dptz_streams(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_zoom_max(cluster_t *cluster, uint8_t value);
attribute_t *create_tilt_min(cluster_t *cluster, int16_t value);
attribute_t *create_tilt_max(cluster_t *cluster, int16_t value);
attribute_t *create_pan_min(cluster_t *cluster, int16_t value);
attribute_t *create_pan_max(cluster_t *cluster, int16_t value);
attribute_t *create_movement_state(cluster_t *cluster, uint8_t value);
} /* attribute */

namespace command {
command_t *create_mptz_set_position(cluster_t *cluster);
command_t *create_mptz_relative_move(cluster_t *cluster);
command_t *create_mptz_move_to_preset(cluster_t *cluster);
command_t *create_mptz_save_preset(cluster_t *cluster);
command_t *create_mptz_remove_preset(cluster_t *cluster);
command_t *create_dptz_set_viewport(cluster_t *cluster);
command_t *create_dptz_relative_move(cluster_t *cluster);
} /* command */

typedef struct config {
    struct {
        feature::mechanical_pan::config_t mechanical_pan;
        feature::mechanical_tilt::config_t mechanical_tilt;
        feature::mechanical_zoom::config_t mechanical_zoom;
        feature::mechanical_presets::config_t mechanical_presets;
    } features;
    uint32_t feature_flags;
    config() : feature_flags(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* camera_av_settings_user_level_management */
} /* cluster */
} /* esp_matter */
