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
namespace illuminance_measurement {

namespace attribute {
attribute_t *create_measured_value(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_min_measured_value(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_max_measured_value(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_tolerance(cluster_t *cluster, uint16_t value);
attribute_t *create_light_sensor_type(cluster_t *cluster, nullable<uint8_t> value);
} /* attribute */

typedef struct config {
    nullable<uint16_t> measured_value;
    nullable<uint16_t> min_measured_value;
    nullable<uint16_t> max_measured_value;
    config() : measured_value(0), min_measured_value(1), max_measured_value(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* illuminance_measurement */
} /* cluster */
} /* esp_matter */
