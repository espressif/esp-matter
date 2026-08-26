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
namespace on_off {

namespace feature {
namespace lighting {
typedef struct config {
    bool global_scene_control;
    uint16_t on_time;
    uint16_t off_wait_time;
    nullable<uint8_t> start_up_on_off;
    config() : global_scene_control(false), on_time(0), off_wait_time(0), start_up_on_off() {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* lighting */

namespace dead_front_behavior {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* dead_front_behavior */

namespace off_only {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* off_only */

} /* feature */

namespace attribute {
attribute_t *create_on_off(cluster_t *cluster, bool value);
attribute_t *create_global_scene_control(cluster_t *cluster, bool value);
attribute_t *create_on_time(cluster_t *cluster, uint16_t value);
attribute_t *create_off_wait_time(cluster_t *cluster, uint16_t value);
attribute_t *create_start_up_on_off(cluster_t *cluster, nullable<uint8_t> value);
} /* attribute */

namespace command {
command_t *create_off(cluster_t *cluster);
command_t *create_on(cluster_t *cluster);
command_t *create_toggle(cluster_t *cluster);
command_t *create_off_with_effect(cluster_t *cluster);
command_t *create_on_with_recall_global_scene(cluster_t *cluster);
command_t *create_on_with_timed_off(cluster_t *cluster);
} /* command */

typedef struct config {
    bool on_off;
    config() : on_off(false) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* on_off */
} /* cluster */
} /* esp_matter */
