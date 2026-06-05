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
namespace basic_information {

const uint8_t k_max_vendor_name_length = 32u;
const uint8_t k_max_product_name_length = 32u;
const uint8_t k_max_node_label_length = 32u;
const uint8_t k_max_location_length = 2u;
const uint8_t k_max_hardware_version_string_length = 64u;
const uint8_t k_max_software_version_string_length = 64u;
const uint8_t k_max_manufacturing_date_length = 16u;
const uint8_t k_max_part_number_length = 32u;
const uint16_t k_max_product_url_length = 256u;
const uint8_t k_max_product_label_length = 64u;
const uint8_t k_max_serial_number_length = 32u;
const uint8_t k_max_unique_id_length = 32u;
namespace attribute {
attribute_t *create_data_model_revision(cluster_t *cluster, uint16_t value);
attribute_t *create_vendor_name(cluster_t *cluster, char * value, uint16_t length);
attribute_t *create_vendor_id(cluster_t *cluster, uint16_t value);
attribute_t *create_product_name(cluster_t *cluster, char * value, uint16_t length);
attribute_t *create_product_id(cluster_t *cluster, uint16_t value);
attribute_t *create_node_label(cluster_t *cluster, char * value, uint16_t length);
attribute_t *create_location(cluster_t *cluster, char * value, uint16_t length);
attribute_t *create_hardware_version(cluster_t *cluster, uint16_t value);
attribute_t *create_hardware_version_string(cluster_t *cluster, char * value, uint16_t length);
attribute_t *create_software_version(cluster_t *cluster, uint32_t value);
attribute_t *create_software_version_string(cluster_t *cluster, char * value, uint16_t length);
attribute_t *create_manufacturing_date(cluster_t *cluster, char * value, uint16_t length);
attribute_t *create_part_number(cluster_t *cluster, char * value, uint16_t length);
attribute_t *create_product_url(cluster_t *cluster, char * value, uint16_t length);
attribute_t *create_product_label(cluster_t *cluster, char * value, uint16_t length);
attribute_t *create_serial_number(cluster_t *cluster, char * value, uint16_t length);
attribute_t *create_local_config_disabled(cluster_t *cluster, bool value);
attribute_t *create_reachable(cluster_t *cluster, bool value);
attribute_t *create_unique_id(cluster_t *cluster, char * value, uint16_t length);
attribute_t *create_capability_minima(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_product_appearance(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_specification_version(cluster_t *cluster, uint32_t value);
attribute_t *create_max_paths_per_invoke(cluster_t *cluster, uint16_t value);
attribute_t *create_configuration_version(cluster_t *cluster, uint32_t value);
} /* attribute */

namespace event {
event_t *create_start_up(cluster_t *cluster);
event_t *create_shut_down(cluster_t *cluster);
event_t *create_leave(cluster_t *cluster);
event_t *create_reachable_changed(cluster_t *cluster);
} /* event */

typedef struct config {
    uint16_t data_model_revision;
    char vendor_name[k_max_vendor_name_length + 1];
    uint16_t vendor_id;
    char product_name[k_max_product_name_length + 1];
    uint16_t product_id;
    char node_label[k_max_node_label_length + 1];
    char location[k_max_location_length + 1];
    uint16_t hardware_version;
    char hardware_version_string[k_max_hardware_version_string_length + 1];
    uint32_t software_version;
    char software_version_string[k_max_software_version_string_length + 1];
    char unique_id[k_max_unique_id_length + 1];
    uint32_t specification_version;
    uint16_t max_paths_per_invoke;
    uint32_t configuration_version;
    config() : data_model_revision(0), vendor_name{0}, vendor_id(0), product_name{0}, product_id(0), node_label{0}, location{0}, hardware_version(0), hardware_version_string{0}, software_version(0), software_version_string{0}, unique_id{0}, specification_version(0), max_paths_per_invoke(1), configuration_version(1) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* basic_information */
} /* cluster */
} /* esp_matter */
