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
namespace electrical_energy_measurement {

namespace feature {
namespace imported_energy {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* imported_energy */

namespace exported_energy {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* exported_energy */

namespace cumulative_energy {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* cumulative_energy */

namespace periodic_energy {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* periodic_energy */

} /* feature */

namespace attribute {
attribute_t *create_accuracy(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_cumulative_energy_imported(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_cumulative_energy_exported(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_periodic_energy_imported(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_periodic_energy_exported(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_cumulative_energy_reset(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
} /* attribute */

namespace event {
event_t *create_cumulative_energy_measured(cluster_t *cluster);
event_t *create_periodic_energy_measured(cluster_t *cluster);
} /* event */

typedef struct config {
    void *delegate;
    uint32_t feature_flags;
    config() : delegate(nullptr), feature_flags(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* electrical_energy_measurement */
} /* cluster */
} /* esp_matter */
