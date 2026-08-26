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
namespace webrtc_transport_provider {

namespace attribute {
attribute_t *create_current_sessions(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
} /* attribute */

namespace command {
command_t *create_solicit_offer(cluster_t *cluster);
command_t *create_solicit_offer_response(cluster_t *cluster);
command_t *create_provide_offer(cluster_t *cluster);
command_t *create_provide_offer_response(cluster_t *cluster);
command_t *create_provide_answer(cluster_t *cluster);
command_t *create_provide_ice_candidates(cluster_t *cluster);
command_t *create_end_session(cluster_t *cluster);
} /* command */

typedef struct config {
    void *delegate;
    config() : delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* webrtc_transport_provider */
} /* cluster */
} /* esp_matter */
