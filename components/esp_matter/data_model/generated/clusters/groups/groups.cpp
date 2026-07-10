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
#include <groups.h>
#include <groups_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 4;

namespace esp_matter {
namespace cluster {
namespace groups {

namespace feature {
namespace group_names {
uint32_t get_id()
{
    return GroupNames::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* group_names */

} /* feature */

namespace attribute {
attribute_t *create_name_support(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, NameSupport::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(NameSupport::Min), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint8_t>(NameSupport::Max), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_add_group(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AddGroup::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_add_group_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AddGroupResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_view_group(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ViewGroup::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_view_group_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ViewGroupResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_get_group_membership(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GetGroupMembership::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_get_group_membership_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GetGroupMembershipResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_remove_group(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RemoveGroup::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_remove_group_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RemoveGroupResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_remove_all_groups(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RemoveAllGroups::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_add_group_if_identifying(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AddGroupIfIdentifying::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, groups::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, groups::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterGroupsPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_name_support(cluster, config->name_support);
        command::create_add_group(cluster);
        command::create_add_group_response(cluster);
        command::create_view_group(cluster);
        command::create_view_group_response(cluster);
        command::create_get_group_membership(cluster);
        command::create_get_group_membership_response(cluster);
        command::create_remove_group(cluster);
        command::create_remove_group_response(cluster);
        command::create_remove_all_groups(cluster);
        command::create_add_group_if_identifying(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterGroupsClusterServerInitCallback,
                                                 ESPMatterGroupsClusterServerShutdownCallback);
    }

    return cluster;
}

} /* groups */
} /* cluster */
} /* esp_matter */
