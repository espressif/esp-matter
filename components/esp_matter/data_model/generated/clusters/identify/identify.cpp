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
#include <identify.h>
#include <identify_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 6;

namespace esp_matter {
namespace cluster {
namespace identify {

namespace attribute {
attribute_t *create_identify_time(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, IdentifyTime::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value));
}

attribute_t *create_identify_type(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, IdentifyType::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(IdentifyType::Min), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(IdentifyType::Max), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_identify(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Identify::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_trigger_effect(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, TriggerEffect::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, identify::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, identify::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterIdentifyPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_identify_time(cluster, config->identify_time);
        attribute::create_identify_type(cluster, config->identify_type);
        command::create_identify(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterIdentifyClusterServerInitCallback,
                                                 ESPMatterIdentifyClusterServerShutdownCallback);
    }

    return cluster;
}

} /* identify */
} /* cluster */
} /* esp_matter */
