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
namespace av_analysis {

namespace feature {
namespace local_context_detection {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* local_context_detection */

namespace remote_context_detection {
typedef struct config {
    uint8_t max_analysis_stream_count;
    uint8_t current_analysis_stream_count;
    config() : max_analysis_stream_count(0), current_analysis_stream_count(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* remote_context_detection */

namespace per_zone_context_detection {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* per_zone_context_detection */

} /* feature */

namespace attribute {
attribute_t *create_supported_ambient_contexts(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_active_ambient_context_triggers(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_max_analysis_stream_count(cluster_t *cluster, uint8_t value);
attribute_t *create_current_analysis_stream_count(cluster_t *cluster, uint8_t value);
attribute_t *create_analysis_streams(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_tracking_enabled(cluster_t *cluster, bool value);
} /* attribute */

namespace command {
command_t *create_enable_context_triggers(cluster_t *cluster);
command_t *create_disable_context_triggers(cluster_t *cluster);
command_t *create_establish_analysis_stream(cluster_t *cluster);
command_t *create_establish_analysis_stream_response(cluster_t *cluster);
command_t *create_activate_analysis_stream(cluster_t *cluster);
command_t *create_deactivate_analysis_stream(cluster_t *cluster);
command_t *create_remove_analysis_stream(cluster_t *cluster);
} /* command */

typedef struct config {
    bool tracking_enabled;
    struct {
        feature::remote_context_detection::config_t remote_context_detection;
    } features;
    uint32_t feature_flags;
    config() : tracking_enabled(false), feature_flags(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* av_analysis */
} /* cluster */
} /* esp_matter */
