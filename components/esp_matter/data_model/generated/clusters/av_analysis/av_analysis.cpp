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

#include <esp_log.h>
#include <esp_matter_core.h>
#include <esp_matter.h>

#include <app-common/zap-generated/cluster-enums.h>
#include <zap_common/app/PluginApplicationCallbacks.h>
#include <av_analysis.h>
#include <av_analysis_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 1;

namespace esp_matter {
namespace cluster {
namespace av_analysis {

namespace feature {
namespace local_context_detection {
uint32_t get_id()
{
    return LocalContextDetection::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* local_context_detection */

namespace remote_context_detection {
uint32_t get_id()
{
    return RemoteContextDetection::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_max_analysis_stream_count(cluster, config->max_analysis_stream_count);
    attribute::create_current_analysis_stream_count(cluster, config->current_analysis_stream_count);
    attribute::create_analysis_streams(cluster, NULL, 0, 0);
    command::create_establish_analysis_stream(cluster);
    command::create_establish_analysis_stream_response(cluster);
    command::create_activate_analysis_stream(cluster);
    command::create_deactivate_analysis_stream(cluster);
    command::create_remove_analysis_stream(cluster);

    return ESP_OK;
}
} /* remote_context_detection */

namespace per_zone_context_detection {
uint32_t get_id()
{
    return PerZoneContextDetection::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* per_zone_context_detection */

} /* feature */

namespace attribute {
attribute_t *create_supported_ambient_contexts(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, SupportedAmbientContexts::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_active_ambient_context_triggers(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ActiveAmbientContextTriggers::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_max_analysis_stream_count(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(remote_context_detection), NULL);
    return esp_matter::attribute::create(cluster, MaxAnalysisStreamCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_current_analysis_stream_count(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(remote_context_detection), NULL);
    return esp_matter::attribute::create(cluster, CurrentAnalysisStreamCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_analysis_streams(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(remote_context_detection), NULL);
    return esp_matter::attribute::create(cluster, AnalysisStreams::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_tracking_enabled(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, TrackingEnabled::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

} /* attribute */
namespace command {
command_t *create_enable_context_triggers(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, EnableContextTriggers::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_disable_context_triggers(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DisableContextTriggers::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_establish_analysis_stream(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(remote_context_detection), NULL);
    return esp_matter::command::create(cluster, EstablishAnalysisStream::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_establish_analysis_stream_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(remote_context_detection), NULL);
    return esp_matter::command::create(cluster, EstablishAnalysisStreamResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_activate_analysis_stream(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(remote_context_detection), NULL);
    return esp_matter::command::create(cluster, ActivateAnalysisStream::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_deactivate_analysis_stream(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(remote_context_detection), NULL);
    return esp_matter::command::create(cluster, DeactivateAnalysisStream::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_remove_analysis_stream(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(remote_context_detection), NULL);
    return esp_matter::command::create(cluster, RemoveAnalysisStream::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, av_analysis::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, av_analysis::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterAvAnalysisPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_tracking_enabled(cluster, config->tracking_enabled);
        attribute::create_supported_ambient_contexts(cluster, NULL, 0, 0);
        attribute::create_active_ambient_context_triggers(cluster, NULL, 0, 0);

        uint32_t feature_map = config->feature_flags;
        VALIDATE_FEATURES_EXACT_ONE("LocalContextDetection,RemoteContextDetection",
                                    feature::local_context_detection::get_id(), feature::remote_context_detection::get_id());
        if (feature_map & feature::local_context_detection::get_id()) {
            VerifyOrReturnValue(feature::local_context_detection::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::remote_context_detection::get_id()) {
            VerifyOrReturnValue(feature::remote_context_detection::add(cluster, &(config->features.remote_context_detection)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::per_zone_context_detection::get_id()) {
            VerifyOrReturnValue(feature::per_zone_context_detection::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        command::create_enable_context_triggers(cluster);
        command::create_disable_context_triggers(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterAvAnalysisClusterServerInitCallback,
                                                 ESPMatterAvAnalysisClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* av_analysis */
} /* cluster */
} /* esp_matter */
