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
#include <network_identity_management.h>
#include <network_identity_management_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "network_identity_management_cluster";
constexpr uint16_t cluster_revision = 1;

namespace esp_matter {
namespace cluster {
namespace network_identity_management {

namespace attribute {
attribute_t *create_active_network_identities(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ActiveNetworkIdentities::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_clients(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, Clients::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_client_table_size(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ClientTableSize::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(500)), esp_matter_attr_val(static_cast<uint16_t>(2047)));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_add_client(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AddClient::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_add_client_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AddClientResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_remove_client(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RemoveClient::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_query_identity(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, QueryIdentity::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_query_identity_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, QueryIdentityResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_import_admin_secret(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ImportAdminSecret::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_export_admin_secret(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ExportAdminSecret::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_export_admin_secret_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ExportAdminSecretResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */

static void create_default_binding_cluster(endpoint_t *endpoint)
{
    binding::config_t config;
    binding::create(endpoint, &config, CLUSTER_FLAG_SERVER);
}

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, network_identity_management::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, network_identity_management::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_client_table_size(cluster, config->client_table_size);
        attribute::create_active_network_identities(cluster, NULL, 0, 0);
        attribute::create_clients(cluster, NULL, 0, 0);
        command::create_add_client(cluster);
        command::create_add_client_response(cluster);
        command::create_remove_client(cluster);
        command::create_query_identity(cluster);
        command::create_query_identity_response(cluster);
        command::create_import_admin_secret(cluster);
        command::create_export_admin_secret(cluster);
        command::create_export_admin_secret_response(cluster);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* network_identity_management */
} /* cluster */
} /* esp_matter */
