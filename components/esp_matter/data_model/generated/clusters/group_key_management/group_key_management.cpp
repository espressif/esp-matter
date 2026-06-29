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
#include <group_key_management.h>
#include <group_key_management_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 3;

namespace esp_matter {
namespace cluster {
namespace group_key_management {

namespace attribute {
attribute_t *create_group_key_map(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, GroupKeyMap::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_group_table(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, GroupTable::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_max_groups_per_fabric(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxGroupsPerFabric::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_max_group_keys_per_fabric(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxGroupKeysPerFabric::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(1)), esp_matter_attr_val(static_cast<uint16_t>(65535)));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_key_set_write(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, KeySetWrite::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_key_set_read(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, KeySetRead::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_key_set_read_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, KeySetReadResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_key_set_remove(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, KeySetRemove::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_key_set_read_all_indices(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, KeySetReadAllIndices::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_key_set_read_all_indices_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, KeySetReadAllIndicesResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, group_key_management::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, group_key_management::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterGroupKeyManagementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_max_groups_per_fabric(cluster, config->max_groups_per_fabric);
        attribute::create_max_group_keys_per_fabric(cluster, config->max_group_keys_per_fabric);
        attribute::create_group_key_map(cluster, NULL, 0, 0);
        attribute::create_group_table(cluster, NULL, 0, 0);
        command::create_key_set_write(cluster);
        command::create_key_set_read(cluster);
        command::create_key_set_read_response(cluster);
        command::create_key_set_remove(cluster);
        command::create_key_set_read_all_indices(cluster);
        command::create_key_set_read_all_indices_response(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterGroupKeyManagementClusterServerInitCallback,
                                                 ESPMatterGroupKeyManagementClusterServerShutdownCallback);
    }

    return cluster;
}

} /* group_key_management */
} /* cluster */
} /* esp_matter */
