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
#include <joint_fabric_administrator.h>
#include <joint_fabric_administrator_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "joint_fabric_administrator_cluster";
constexpr uint16_t cluster_revision = 1;

namespace esp_matter {
namespace cluster {
namespace joint_fabric_administrator {

namespace attribute {
attribute_t *create_administrator_fabric_index(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, AdministratorFabricIndex::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(1)), esp_matter_attr_val(nullable<uint8_t>(254)));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_icaccsr_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ICACCSRRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_icaccsr_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ICACCSRResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_add_icac(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AddICAC::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_icac_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ICACResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_open_joint_commissioning_window(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OpenJointCommissioningWindow::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_transfer_anchor_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, TransferAnchorRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_transfer_anchor_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, TransferAnchorResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_transfer_anchor_complete(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, TransferAnchorComplete::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_announce_joint_fabric_administrator(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AnnounceJointFabricAdministrator::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, joint_fabric_administrator::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, joint_fabric_administrator::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterJointFabricAdministratorPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_administrator_fabric_index(cluster, config->administrator_fabric_index);
        command::create_icaccsr_request(cluster);
        command::create_icaccsr_response(cluster);
        command::create_add_icac(cluster);
        command::create_icac_response(cluster);
        command::create_open_joint_commissioning_window(cluster);
        command::create_transfer_anchor_request(cluster);
        command::create_transfer_anchor_response(cluster);
        command::create_transfer_anchor_complete(cluster);
        command::create_announce_joint_fabric_administrator(cluster);
    }

    return cluster;
}

} /* joint_fabric_administrator */
} /* cluster */
} /* esp_matter */
