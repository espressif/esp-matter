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
namespace laundry_washer_controls {

namespace feature {
namespace spin {
typedef struct config {
    nullable<uint8_t> spin_speed_current;
    config() : spin_speed_current() {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* spin */

namespace rinse {
typedef struct config {
    uint8_t number_of_rinses;
    config() : number_of_rinses(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* rinse */

} /* feature */

namespace attribute {
attribute_t *create_spin_speeds(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_spin_speed_current(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_number_of_rinses(cluster_t *cluster, uint8_t value);
attribute_t *create_supported_rinses(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
} /* attribute */

typedef struct config {
    void *delegate;
    struct {
        feature::spin::config_t spin;
        feature::rinse::config_t rinse;
    } features;
    uint32_t feature_flags;
    config() : delegate(nullptr), feature_flags(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* laundry_washer_controls */
} /* cluster */
} /* esp_matter */
