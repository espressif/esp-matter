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
namespace media_file_management {

namespace feature {
namespace media_sharing {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* media_sharing */

} /* feature */

namespace attribute {
attribute_t *create_total_storage(cluster_t *cluster, uint64_t value);
attribute_t *create_available_storage(cluster_t *cluster, uint64_t value);
attribute_t *create_available_files(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_supported_mime_types(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
} /* attribute */

namespace command {
command_t *create_add_file(cluster_t *cluster);
command_t *create_add_file_response(cluster_t *cluster);
command_t *create_delete_file(cluster_t *cluster);
command_t *create_request_shared_files(cluster_t *cluster);
command_t *create_get_shared_file(cluster_t *cluster);
command_t *create_get_shared_file_response(cluster_t *cluster);
command_t *create_offer_file(cluster_t *cluster);
} /* command */

namespace event {
event_t *create_shared_files_added(cluster_t *cluster);
} /* event */

typedef struct config {
    uint64_t total_storage;
    uint64_t available_storage;
    config() : total_storage(0), available_storage(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* media_file_management */
} /* cluster */
} /* esp_matter */
