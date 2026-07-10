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
namespace time_synchronization {

const uint8_t k_max_default_ntp_length = 128u;
namespace feature {
namespace time_zone {
typedef struct config {
    nullable<uint64_t> local_time;
    uint8_t time_zone_database;
    uint8_t time_zone_list_max_size;
    uint8_t dst_offset_list_max_size;
    config() : local_time(0), time_zone_database(0), time_zone_list_max_size(1), dst_offset_list_max_size(1) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* time_zone */

namespace ntp_client {
typedef struct config {
    char default_ntp[k_max_default_ntp_length + 1];
    bool supports_dns_resolve;
    config() : supports_dns_resolve(false) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* ntp_client */

namespace ntp_server {
typedef struct config {
    bool ntp_server_available;
    config() : ntp_server_available(false) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* ntp_server */

namespace time_sync_client {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* time_sync_client */

} /* feature */

namespace attribute {
attribute_t *create_utc_time(cluster_t *cluster, nullable<uint64_t> value);
attribute_t *create_granularity(cluster_t *cluster, uint8_t value);
attribute_t *create_time_source(cluster_t *cluster, uint8_t value);
attribute_t *create_trusted_time_source(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_default_ntp(cluster_t *cluster, char * value, uint16_t length);
attribute_t *create_time_zone(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_dst_offset(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_local_time(cluster_t *cluster, nullable<uint64_t> value);
attribute_t *create_time_zone_database(cluster_t *cluster, uint8_t value);
attribute_t *create_ntp_server_available(cluster_t *cluster, bool value);
attribute_t *create_time_zone_list_max_size(cluster_t *cluster, uint8_t value);
attribute_t *create_dst_offset_list_max_size(cluster_t *cluster, uint8_t value);
attribute_t *create_supports_dns_resolve(cluster_t *cluster, bool value);
} /* attribute */

namespace command {
command_t *create_set_utc_time(cluster_t *cluster);
command_t *create_set_trusted_time_source(cluster_t *cluster);
command_t *create_set_time_zone(cluster_t *cluster);
command_t *create_set_time_zone_response(cluster_t *cluster);
command_t *create_set_dst_offset(cluster_t *cluster);
command_t *create_set_default_ntp(cluster_t *cluster);
} /* command */

namespace event {
event_t *create_dst_table_empty(cluster_t *cluster);
event_t *create_dst_status(cluster_t *cluster);
event_t *create_time_zone_status(cluster_t *cluster);
event_t *create_time_failure(cluster_t *cluster);
event_t *create_missing_trusted_time_source(cluster_t *cluster);
} /* event */

typedef struct config {
    nullable<uint64_t> utc_time;
    uint8_t granularity;
    void *delegate;
    config() : utc_time(0), granularity(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* time_synchronization */
} /* cluster */
} /* esp_matter */
