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
namespace general_commissioning {

namespace feature {
namespace terms_and_conditions {
typedef struct config {
    uint16_t tc_accepted_version;
    uint16_t tc_min_required_version;
    uint16_t tc_acknowledgements;
    bool tc_acknowledgements_required;
    nullable<uint32_t> tc_update_deadline;
    config() : tc_accepted_version(0), tc_min_required_version(0), tc_acknowledgements(0), tc_acknowledgements_required(true), tc_update_deadline() {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* terms_and_conditions */

} /* feature */

namespace attribute {
attribute_t *create_breadcrumb(cluster_t *cluster, uint64_t value);
attribute_t *create_basic_commissioning_info(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_regulatory_config(cluster_t *cluster, uint8_t value);
attribute_t *create_location_capability(cluster_t *cluster, uint8_t value);
attribute_t *create_supports_concurrent_connection(cluster_t *cluster, bool value);
attribute_t *create_tc_accepted_version(cluster_t *cluster, uint16_t value);
attribute_t *create_tc_min_required_version(cluster_t *cluster, uint16_t value);
attribute_t *create_tc_acknowledgements(cluster_t *cluster, uint16_t value);
attribute_t *create_tc_acknowledgements_required(cluster_t *cluster, bool value);
attribute_t *create_tc_update_deadline(cluster_t *cluster, nullable<uint32_t> value);
} /* attribute */

namespace command {
command_t *create_arm_fail_safe(cluster_t *cluster);
command_t *create_arm_fail_safe_response(cluster_t *cluster);
command_t *create_set_regulatory_config(cluster_t *cluster);
command_t *create_set_regulatory_config_response(cluster_t *cluster);
command_t *create_commissioning_complete(cluster_t *cluster);
command_t *create_commissioning_complete_response(cluster_t *cluster);
command_t *create_set_tc_acknowledgements(cluster_t *cluster);
command_t *create_set_tc_acknowledgements_response(cluster_t *cluster);
} /* command */

typedef struct config {
    uint64_t breadcrumb;
    uint8_t regulatory_config;
    uint8_t location_capability;
    bool supports_concurrent_connection;
    config() : breadcrumb(0), regulatory_config(0), location_capability(0), supports_concurrent_connection(true) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* general_commissioning */
} /* cluster */
} /* esp_matter */
