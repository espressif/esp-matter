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
namespace boolean_state {

namespace feature {
namespace change_event {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* change_event */

} /* feature */

namespace attribute {
attribute_t *create_state_value(cluster_t *cluster, bool value);
} /* attribute */

namespace event {
event_t *create_state_change(cluster_t *cluster);
} /* event */

typedef struct config {
    bool state_value;
    config() : state_value(false) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* boolean_state */
} /* cluster */
} /* esp_matter */
