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
#include <thread_network_directory.h>
#include <thread_network_directory_ids.h>
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
namespace thread_network_directory {

namespace attribute {
attribute_t *create_preferred_extended_pan_id(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, PreferredExtendedPanID::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length));
}

attribute_t *create_thread_networks(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ThreadNetworks::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_thread_network_table_size(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ThreadNetworkTableSize::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_add_network(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AddNetwork::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_remove_network(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RemoveNetwork::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_get_operational_dataset(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GetOperationalDataset::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_operational_dataset_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OperationalDatasetResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, thread_network_directory::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, thread_network_directory::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterThreadNetworkDirectoryPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_preferred_extended_pan_id(cluster, config->preferred_extended_pan_id, sizeof(config->preferred_extended_pan_id));
        attribute::create_thread_network_table_size(cluster, config->thread_network_table_size);
        attribute::create_thread_networks(cluster, NULL, 0, 0);
        command::create_add_network(cluster);
        command::create_remove_network(cluster);
        command::create_get_operational_dataset(cluster);
        command::create_operational_dataset_response(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterThreadNetworkDirectoryClusterServerInitCallback,
                                                 ESPMatterThreadNetworkDirectoryClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* thread_network_directory */
} /* cluster */
} /* esp_matter */
