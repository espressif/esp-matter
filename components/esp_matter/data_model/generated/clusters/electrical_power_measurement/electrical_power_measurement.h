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
namespace electrical_power_measurement {

namespace feature {
namespace direct_current {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* direct_current */

namespace alternating_current {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* alternating_current */

namespace polyphase_power {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* polyphase_power */

namespace harmonics {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* harmonics */

namespace power_quality {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* power_quality */

} /* feature */

namespace attribute {
attribute_t *create_power_mode(cluster_t *cluster, uint8_t value);
attribute_t *create_number_of_measurement_types(cluster_t *cluster, uint8_t value);
attribute_t *create_accuracy(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_ranges(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_voltage(cluster_t *cluster, nullable<int64_t> value);
attribute_t *create_active_current(cluster_t *cluster, nullable<int64_t> value);
attribute_t *create_reactive_current(cluster_t *cluster, nullable<int64_t> value);
attribute_t *create_apparent_current(cluster_t *cluster, nullable<int64_t> value);
attribute_t *create_active_power(cluster_t *cluster, nullable<int64_t> value);
attribute_t *create_reactive_power(cluster_t *cluster, nullable<int64_t> value);
attribute_t *create_apparent_power(cluster_t *cluster, nullable<int64_t> value);
attribute_t *create_rms_voltage(cluster_t *cluster, nullable<int64_t> value);
attribute_t *create_rms_current(cluster_t *cluster, nullable<int64_t> value);
attribute_t *create_rms_power(cluster_t *cluster, nullable<int64_t> value);
attribute_t *create_frequency(cluster_t *cluster, nullable<int64_t> value);
attribute_t *create_harmonic_currents(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_harmonic_phases(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_power_factor(cluster_t *cluster, nullable<int64_t> value);
attribute_t *create_neutral_current(cluster_t *cluster, nullable<int64_t> value);
} /* attribute */

namespace event {
event_t *create_measurement_period_ranges(cluster_t *cluster);
} /* event */

typedef struct config {
    uint8_t power_mode;
    uint8_t number_of_measurement_types;
    nullable<int64_t> active_power;
    void *delegate;
    uint32_t feature_flags;
    config() : power_mode(0), number_of_measurement_types(0), active_power(), delegate(nullptr), feature_flags(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* electrical_power_measurement */
} /* cluster */
} /* esp_matter */
