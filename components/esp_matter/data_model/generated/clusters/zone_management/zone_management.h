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
namespace zone_management {

namespace feature {
namespace two_dimensional_cartesian_zone {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* two_dimensional_cartesian_zone */

namespace per_zone_sensitivity {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* per_zone_sensitivity */

namespace user_defined {
typedef struct config {
    uint8_t max_user_defined_zones;
    config() : max_user_defined_zones(5) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* user_defined */

namespace focus_zones {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* focus_zones */

} /* feature */

namespace attribute {
attribute_t *create_max_user_defined_zones(cluster_t *cluster, uint8_t value);
attribute_t *create_max_zones(cluster_t *cluster, uint8_t value);
attribute_t *create_zones(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_triggers(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_sensitivity_max(cluster_t *cluster, uint8_t value);
attribute_t *create_sensitivity(cluster_t *cluster, uint8_t value);
attribute_t *create_two_d_cartesian_max(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
} /* attribute */

namespace command {
command_t *create_create_two_d_cartesian_zone(cluster_t *cluster);
command_t *create_create_two_d_cartesian_zone_response(cluster_t *cluster);
command_t *create_update_two_d_cartesian_zone(cluster_t *cluster);
command_t *create_remove_zone(cluster_t *cluster);
command_t *create_create_or_update_trigger(cluster_t *cluster);
command_t *create_remove_trigger(cluster_t *cluster);
} /* command */

namespace event {
event_t *create_zone_triggered(cluster_t *cluster);
event_t *create_zone_stopped(cluster_t *cluster);
} /* event */

typedef struct config {
    uint8_t max_zones;
    uint8_t sensitivity_max;
    uint8_t sensitivity;
    config() : max_zones(1), sensitivity_max(2), sensitivity(1) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* zone_management */
} /* cluster */
} /* esp_matter */
