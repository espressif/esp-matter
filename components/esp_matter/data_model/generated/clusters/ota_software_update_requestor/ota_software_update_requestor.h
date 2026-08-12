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
namespace ota_software_update_requestor {

namespace attribute {
attribute_t *create_default_ota_providers(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_update_possible(cluster_t *cluster, bool value);
attribute_t *create_update_state(cluster_t *cluster, uint8_t value);
attribute_t *create_update_state_progress(cluster_t *cluster, nullable<uint8_t> value);
} /* attribute */

namespace command {
command_t *create_announce_ota_provider(cluster_t *cluster);
} /* command */

namespace event {
event_t *create_state_transition(cluster_t *cluster);
event_t *create_version_applied(cluster_t *cluster);
event_t *create_download_error(cluster_t *cluster);
} /* event */

typedef struct config {
    bool update_possible;
    uint8_t update_state;
    nullable<uint8_t> update_state_progress;
    config() : update_possible(true), update_state(0), update_state_progress() {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* ota_software_update_requestor */
} /* cluster */
} /* esp_matter */
