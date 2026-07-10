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
namespace valve_configuration_and_control {

namespace feature {
namespace time_sync {
typedef struct config {
    nullable<uint64_t> auto_close_time;
    config() : auto_close_time(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* time_sync */

namespace level {
typedef struct config {
    nullable<uint8_t> current_level;
    nullable<uint8_t> target_level;
    config() : current_level(0), target_level(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* level */

} /* feature */

namespace attribute {
attribute_t *create_open_duration(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_default_open_duration(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_auto_close_time(cluster_t *cluster, nullable<uint64_t> value);
attribute_t *create_remaining_duration(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_current_state(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_target_state(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_current_level(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_target_level(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_default_open_level(cluster_t *cluster, uint8_t value);
attribute_t *create_valve_fault(cluster_t *cluster, uint16_t value);
attribute_t *create_level_step(cluster_t *cluster, uint8_t value);
} /* attribute */

namespace command {
command_t *create_open(cluster_t *cluster);
command_t *create_close(cluster_t *cluster);
} /* command */

namespace event {
event_t *create_valve_state_changed(cluster_t *cluster);
event_t *create_valve_fault(cluster_t *cluster);
} /* event */

typedef struct config {
    nullable<uint32_t> open_duration;
    nullable<uint32_t> default_open_duration;
    nullable<uint32_t> remaining_duration;
    nullable<uint8_t> current_state;
    nullable<uint8_t> target_state;
    void *delegate;
    config() : open_duration(1), default_open_duration(1), remaining_duration(0), current_state(0), target_state(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* valve_configuration_and_control */
} /* cluster */
} /* esp_matter */
