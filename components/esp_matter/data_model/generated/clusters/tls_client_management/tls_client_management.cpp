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
#include <tls_client_management.h>
#include <tls_client_management_ids.h>
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
namespace tls_client_management {

namespace attribute {
attribute_t *create_max_provisioned(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxProvisioned::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(MaxProvisioned::Min)), esp_matter_attr_val(static_cast<uint8_t>(MaxProvisioned::Max)));
    return attribute;
}

attribute_t *create_provisioned_endpoints(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ProvisionedEndpoints::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

} /* attribute */
namespace command {
command_t *create_provision_endpoint(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ProvisionEndpoint::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_provision_endpoint_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ProvisionEndpointResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_find_endpoint(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, FindEndpoint::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_find_endpoint_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, FindEndpointResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_remove_endpoint(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RemoveEndpoint::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, tls_client_management::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, tls_client_management::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterTlsClientManagementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_max_provisioned(cluster, config->max_provisioned);
        attribute::create_provisioned_endpoints(cluster, NULL, 0, 0);
        command::create_provision_endpoint(cluster);
        command::create_provision_endpoint_response(cluster);
        command::create_find_endpoint(cluster);
        command::create_find_endpoint_response(cluster);
        command::create_remove_endpoint(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterTlsClientManagementClusterServerInitCallback,
                                                 ESPMatterTlsClientManagementClusterServerShutdownCallback);
    }

    return cluster;
}

} /* tls_client_management */
} /* cluster */
} /* esp_matter */
