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
namespace access_control {

namespace feature {
namespace extension {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* extension */

namespace managed_device {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* managed_device */

} /* feature */

namespace attribute {
attribute_t *create_acl(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_extension(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_subjects_per_access_control_entry(cluster_t *cluster, uint16_t value);
attribute_t *create_targets_per_access_control_entry(cluster_t *cluster, uint16_t value);
attribute_t *create_access_control_entries_per_fabric(cluster_t *cluster, uint16_t value);
#if CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
attribute_t *create_commissioning_arl(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
#endif // CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
#if CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
attribute_t *create_arl(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
#endif // CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
} /* attribute */

namespace command {
#if CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
command_t *create_review_fabric_restrictions(cluster_t *cluster);
#endif // CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
command_t *create_review_fabric_restrictions_response(cluster_t *cluster);
} /* command */

namespace event {
event_t *create_access_control_entry_changed(cluster_t *cluster);
event_t *create_access_control_extension_changed(cluster_t *cluster);
event_t *create_fabric_restriction_review_update(cluster_t *cluster);
} /* event */

typedef struct config {
    uint16_t subjects_per_access_control_entry;
    uint16_t targets_per_access_control_entry;
    uint16_t access_control_entries_per_fabric;
    config() : subjects_per_access_control_entry(4), targets_per_access_control_entry(3), access_control_entries_per_fabric(4) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* access_control */
} /* cluster */
} /* esp_matter */
