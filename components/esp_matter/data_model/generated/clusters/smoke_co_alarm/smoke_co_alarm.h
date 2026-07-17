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
namespace smoke_co_alarm {

namespace feature {
namespace smoke_alarm {
typedef struct config {
    uint8_t smoke_state;
    config() : smoke_state(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* smoke_alarm */

namespace co_alarm {
typedef struct config {
    uint8_t co_state;
    config() : co_state(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* co_alarm */

} /* feature */

namespace attribute {
attribute_t *create_expressed_state(cluster_t *cluster, uint8_t value);
attribute_t *create_smoke_state(cluster_t *cluster, uint8_t value);
attribute_t *create_co_state(cluster_t *cluster, uint8_t value);
attribute_t *create_battery_alert(cluster_t *cluster, uint8_t value);
attribute_t *create_device_muted(cluster_t *cluster, uint8_t value);
attribute_t *create_test_in_progress(cluster_t *cluster, bool value);
attribute_t *create_hardware_fault_alert(cluster_t *cluster, bool value);
attribute_t *create_end_of_service_alert(cluster_t *cluster, uint8_t value);
attribute_t *create_interconnect_smoke_alarm(cluster_t *cluster, uint8_t value);
attribute_t *create_interconnect_co_alarm(cluster_t *cluster, uint8_t value);
attribute_t *create_contamination_state(cluster_t *cluster, uint8_t value);
attribute_t *create_smoke_sensitivity_level(cluster_t *cluster, uint8_t value);
attribute_t *create_expiry_date(cluster_t *cluster, uint32_t value);
attribute_t *create_unmounted(cluster_t *cluster, bool value);
} /* attribute */

namespace command {
command_t *create_self_test_request(cluster_t *cluster);
} /* command */

namespace event {
event_t *create_smoke_alarm(cluster_t *cluster);
event_t *create_co_alarm(cluster_t *cluster);
event_t *create_low_battery(cluster_t *cluster);
event_t *create_hardware_fault(cluster_t *cluster);
event_t *create_end_of_service(cluster_t *cluster);
event_t *create_self_test_complete(cluster_t *cluster);
event_t *create_alarm_muted(cluster_t *cluster);
event_t *create_mute_ended(cluster_t *cluster);
event_t *create_interconnect_smoke_alarm(cluster_t *cluster);
event_t *create_interconnect_co_alarm(cluster_t *cluster);
event_t *create_all_clear(cluster_t *cluster);
} /* event */

typedef struct config {
    uint8_t expressed_state;
    uint8_t battery_alert;
    bool test_in_progress;
    bool hardware_fault_alert;
    uint8_t end_of_service_alert;
    void *delegate;
    struct {
        feature::smoke_alarm::config_t smoke_alarm;
        feature::co_alarm::config_t co_alarm;
    } features;
    uint32_t feature_flags;
    config() : expressed_state(0), battery_alert(0), test_in_progress(false), hardware_fault_alert(false), end_of_service_alert(0), delegate(nullptr), feature_flags(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* smoke_co_alarm */
} /* cluster */
} /* esp_matter */
