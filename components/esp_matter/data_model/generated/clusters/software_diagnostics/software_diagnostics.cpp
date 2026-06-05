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
#include <app-common/zap-generated/callback.h>
#include <app/InteractionModelEngine.h>
#include <zap_common/app/PluginApplicationCallbacks.h>
#include <software_diagnostics.h>
#include <software_diagnostics_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "software_diagnostics_cluster";
constexpr uint16_t cluster_revision = 1;

namespace esp_matter {
namespace cluster {
namespace software_diagnostics {

namespace feature {
namespace watermarks {
uint32_t get_id()
{
    return Watermarks::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_current_heap_high_watermark(cluster, config->current_heap_high_watermark);
    command::create_reset_watermarks(cluster);

    return ESP_OK;
}
} /* watermarks */

} /* feature */

namespace attribute {
attribute_t *create_thread_metrics(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ThreadMetrics::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_current_heap_free(cluster_t *cluster, uint64_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, CurrentHeapFree::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint64_t>(0)), esp_matter_attr_val(static_cast<uint64_t>(4294967294)));
    return attribute;
}

attribute_t *create_current_heap_used(cluster_t *cluster, uint64_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, CurrentHeapUsed::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint64_t>(0)), esp_matter_attr_val(static_cast<uint64_t>(4294967294)));
    return attribute;
}

attribute_t *create_current_heap_high_watermark(cluster_t *cluster, uint64_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(watermarks), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, CurrentHeapHighWatermark::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint64_t>(0)), esp_matter_attr_val(static_cast<uint64_t>(4294967294)));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_reset_watermarks(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(watermarks), NULL);
    return esp_matter::command::create(cluster, ResetWatermarks::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

namespace event {
event_t *create_software_fault(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, SoftwareFault::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, software_diagnostics::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, software_diagnostics::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterSoftwareDiagnosticsPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterSoftwareDiagnosticsClusterServerInitCallback,
                                                 ESPMatterSoftwareDiagnosticsClusterServerShutdownCallback);
    }

    return cluster;
}

} /* software_diagnostics */
} /* cluster */
} /* esp_matter */
