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
#include <ambient_sensing_union.h>
#include <ambient_sensing_union_ids.h>
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
namespace ambient_sensing_union {

namespace attribute {
attribute_t *create_union_name(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, UnionName::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length), k_max_union_name_length + 1);
}

attribute_t *create_union_health(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, UnionHealth::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(UnionHealth::Min), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(UnionHealth::Max), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_union_contributor_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, UnionContributorList::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

} /* attribute */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, ambient_sensing_union::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, ambient_sensing_union::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterAmbientSensingUnionPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_union_name(cluster, config->union_name, sizeof(config->union_name));
        attribute::create_union_health(cluster, config->union_health);
        attribute::create_union_contributor_list(cluster, NULL, 0, 0);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterAmbientSensingUnionClusterServerInitCallback,
                                                 ESPMatterAmbientSensingUnionClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* ambient_sensing_union */
} /* cluster */
} /* esp_matter */
