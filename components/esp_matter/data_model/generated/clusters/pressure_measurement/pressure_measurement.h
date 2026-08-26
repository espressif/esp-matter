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
namespace pressure_measurement {

namespace feature {
namespace extended {
typedef struct config {
    nullable<int16_t> scaled_value;
    nullable<int16_t> min_scaled_value;
    nullable<int16_t> max_scaled_value;
    int8_t scale;
    config() : scaled_value(), min_scaled_value(), max_scaled_value(), scale(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* extended */

} /* feature */

namespace attribute {
attribute_t *create_measured_value(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_min_measured_value(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_max_measured_value(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_tolerance(cluster_t *cluster, uint16_t value);
attribute_t *create_scaled_value(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_min_scaled_value(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_max_scaled_value(cluster_t *cluster, nullable<int16_t> value);
attribute_t *create_scaled_tolerance(cluster_t *cluster, uint16_t value);
attribute_t *create_scale(cluster_t *cluster, int8_t value);
} /* attribute */

typedef struct config {
    nullable<int16_t> measured_value;
    nullable<int16_t> min_measured_value;
    nullable<int16_t> max_measured_value;
    config() : measured_value(), min_measured_value(), max_measured_value() {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* pressure_measurement */
} /* cluster */
} /* esp_matter */
