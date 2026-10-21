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
namespace electrical_distribution {

namespace attribute {
attribute_t *create_max_continuous_current(cluster_t *cluster, nullable<int64_t> value);
attribute_t *create_max_voltage(cluster_t *cluster, nullable<int64_t> value);
attribute_t *create_number_of_poles(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_end_of_life(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_service_entrance_rated(cluster_t *cluster, nullable<bool> value);
} /* attribute */

typedef struct config {
    nullable<int64_t> max_continuous_current;
    nullable<int64_t> max_voltage;
    nullable<uint16_t> number_of_poles;
    nullable<uint8_t> end_of_life;
    nullable<bool> service_entrance_rated;
    config() : max_continuous_current(), max_voltage(), number_of_poles(), end_of_life(), service_entrance_rated(false) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* electrical_distribution */
} /* cluster */
} /* esp_matter */
