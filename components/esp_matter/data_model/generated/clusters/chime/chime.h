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
namespace chime {

namespace attribute {
attribute_t *create_installed_chime_sounds(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_selected_chime(cluster_t *cluster, uint8_t value);
attribute_t *create_enabled(cluster_t *cluster, bool value);
} /* attribute */

namespace command {
command_t *create_play_chime_sound(cluster_t *cluster);
} /* command */

namespace event {
event_t *create_chime_started_playing(cluster_t *cluster);
} /* event */

typedef struct config {
    uint8_t selected_chime;
    bool enabled;
    void *delegate;
    config() : selected_chime(0), enabled(false), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* chime */
} /* cluster */
} /* esp_matter */
