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
#include <esp_matter_delegate_callbacks.h>
#include <thread_border_router_management.h>
#include <thread_border_router_management_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "thread_border_router_management_cluster";
constexpr uint16_t cluster_revision = 1;

namespace esp_matter {
namespace cluster {
namespace thread_border_router_management {

namespace feature {
namespace pan_change {
uint32_t get_id()
{
    return PANChange::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command::create_set_pending_dataset_request(cluster);

    return ESP_OK;
}
} /* pan_change */

} /* feature */

namespace attribute {
attribute_t *create_border_router_name(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BorderRouterName::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length), k_max_border_router_name_length + 1);
}

attribute_t *create_border_agent_id(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BorderAgentID::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length));
}

attribute_t *create_thread_version(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ThreadVersion::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_interface_enabled(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, InterfaceEnabled::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_active_dataset_timestamp(cluster_t *cluster, nullable<uint64_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ActiveDatasetTimestamp::Id, ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint64_t>(0)), esp_matter_attr_val(nullable<uint64_t>(4294967294)));
    return attribute;
}

attribute_t *create_pending_dataset_timestamp(cluster_t *cluster, nullable<uint64_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PendingDatasetTimestamp::Id, ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint64_t>(0)), esp_matter_attr_val(nullable<uint64_t>(4294967294)));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_get_active_dataset_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GetActiveDatasetRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_get_pending_dataset_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GetPendingDatasetRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_dataset_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DatasetResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_set_active_dataset_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SetActiveDatasetRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_set_pending_dataset_request(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(pan_change), NULL);
    return esp_matter::command::create(cluster, SetPendingDatasetRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
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
    cluster_t *cluster = esp_matter::cluster::create(endpoint, thread_border_router_management::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, thread_border_router_management::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = ThreadBorderRouterManagementDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterThreadBorderRouterManagementPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_border_router_name(cluster, config->border_router_name, sizeof(config->border_router_name));
        attribute::create_border_agent_id(cluster, config->border_agent_id, sizeof(config->border_agent_id));
        attribute::create_thread_version(cluster, config->thread_version);
        attribute::create_interface_enabled(cluster, config->interface_enabled);
        attribute::create_active_dataset_timestamp(cluster, config->active_dataset_timestamp);
        attribute::create_pending_dataset_timestamp(cluster, config->pending_dataset_timestamp);
        command::create_get_active_dataset_request(cluster);
        command::create_get_pending_dataset_request(cluster);
        command::create_dataset_response(cluster);
        command::create_set_active_dataset_request(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterThreadBorderRouterManagementClusterServerInitCallback,
                                                 ESPMatterThreadBorderRouterManagementClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* thread_border_router_management */
} /* cluster */
} /* esp_matter */
