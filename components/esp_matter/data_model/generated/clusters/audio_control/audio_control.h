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
namespace audio_control {

namespace feature {
namespace decibel {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* decibel */

namespace basic_equalizer {
typedef struct config {
    int16_t min_correction;
    int16_t max_correction;
    config() : min_correction(0), max_correction(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* basic_equalizer */

} /* feature */

namespace attribute {
attribute_t *create_soft_muted(cluster_t *cluster, bool value);
attribute_t *create_physically_muted(cluster_t *cluster, bool value);
attribute_t *create_volume(cluster_t *cluster, uint16_t value);
attribute_t *create_min_device_volume(cluster_t *cluster, uint16_t value);
attribute_t *create_max_device_volume(cluster_t *cluster, uint16_t value);
attribute_t *create_max_device_volume_db(cluster_t *cluster, uint16_t value);
attribute_t *create_max_user_volume(cluster_t *cluster, uint16_t value);
attribute_t *create_default_step_size(cluster_t *cluster, uint16_t value);
attribute_t *create_set_volume_unmute_policy(cluster_t *cluster, uint8_t value);
attribute_t *create_increase_volume_unmute_policy(cluster_t *cluster, uint8_t value);
attribute_t *create_increase_volume_unmute_volume(cluster_t *cluster, uint8_t value);
attribute_t *create_decrease_volume_unmute_policy(cluster_t *cluster, uint8_t value);
attribute_t *create_start_up_muted(cluster_t *cluster, nullable<bool> value);
attribute_t *create_start_up_volume(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_bass(cluster_t *cluster, int16_t value);
attribute_t *create_mid(cluster_t *cluster, int16_t value);
attribute_t *create_treble(cluster_t *cluster, int16_t value);
attribute_t *create_min_correction(cluster_t *cluster, int16_t value);
attribute_t *create_max_correction(cluster_t *cluster, int16_t value);
} /* attribute */

namespace command {
command_t *create_mute(cluster_t *cluster);
command_t *create_unmute(cluster_t *cluster);
command_t *create_toggle_muted(cluster_t *cluster);
command_t *create_set_volume(cluster_t *cluster);
command_t *create_increase_volume(cluster_t *cluster);
command_t *create_decrease_volume(cluster_t *cluster);
} /* command */

typedef struct config {
    bool soft_muted;
    uint16_t volume;
    uint16_t min_device_volume;
    uint16_t max_device_volume;
    uint16_t default_step_size;
    uint8_t set_volume_unmute_policy;
    uint8_t increase_volume_unmute_policy;
    uint8_t increase_volume_unmute_volume;
    uint8_t decrease_volume_unmute_policy;
    config() : soft_muted(false), volume(0), min_device_volume(1), max_device_volume(0), default_step_size(1), set_volume_unmute_policy(0), increase_volume_unmute_policy(0), increase_volume_unmute_volume(0), decrease_volume_unmute_policy(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* audio_control */
} /* cluster */
} /* esp_matter */
