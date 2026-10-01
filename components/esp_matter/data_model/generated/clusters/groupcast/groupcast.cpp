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
#include <groupcast.h>
#include <groupcast_ids.h>
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
namespace groupcast {

namespace feature {
namespace listener {
uint32_t get_id()
{
    return Listener::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command::create_configure_auxiliary_acl(cluster);

    return ESP_OK;
}
} /* listener */

namespace sender {
uint32_t get_id()
{
    return Sender::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* sender */

namespace per_group {
uint32_t get_id()
{
    return PerGroup::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* per_group */

} /* feature */

namespace attribute {
attribute_t *create_membership(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, Membership::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_max_membership_count(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxMembershipCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(MaxMembershipCount::Min)), esp_matter_attr_val(static_cast<uint16_t>(MaxMembershipCount::Max)));
    return attribute;
}

attribute_t *create_max_mcast_addr_count(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxMcastAddrCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(MaxMcastAddrCount::Min)), esp_matter_attr_val(static_cast<uint16_t>(MaxMcastAddrCount::Max)));
    return attribute;
}

attribute_t *create_used_mcast_addr_count(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, UsedMcastAddrCount::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_fabric_under_test(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, FabricUnderTest::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

} /* attribute */
namespace command {
command_t *create_join_group(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, JoinGroup::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_leave_group(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, LeaveGroup::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_leave_group_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, LeaveGroupResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_update_group_key(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, UpdateGroupKey::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_configure_auxiliary_acl(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(listener), NULL);
    return esp_matter::command::create(cluster, ConfigureAuxiliaryACL::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_groupcast_testing(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GroupcastTesting::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

namespace event {
event_t *create_groupcast_testing(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, GroupcastTesting::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, groupcast::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, groupcast::Id));
#if defined(CONFIG_SUPPORT_GROUPCAST_CLUSTER)
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterGroupcastPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_max_membership_count(cluster, config->max_membership_count);
        attribute::create_max_mcast_addr_count(cluster, config->max_mcast_addr_count);
        attribute::create_used_mcast_addr_count(cluster, config->used_mcast_addr_count);
        attribute::create_fabric_under_test(cluster, config->fabric_under_test);
        attribute::create_membership(cluster, NULL, 0, 0);

        uint32_t feature_map = config->feature_flags;
        VALIDATE_FEATURES_AT_LEAST_ONE("Listener,Sender",
                                       feature::listener::get_id(), feature::sender::get_id());
        if (feature_map & feature::listener::get_id()) {
            VerifyOrReturnValue(feature::listener::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::sender::get_id()) {
            VerifyOrReturnValue(feature::sender::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::per_group::get_id()) {
            VerifyOrReturnValue(feature::per_group::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        command::create_join_group(cluster);
        command::create_leave_group(cluster);
        command::create_leave_group_response(cluster);
        command::create_update_group_key(cluster);
        command::create_groupcast_testing(cluster);
        /* Events */
        event::create_groupcast_testing(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterGroupcastClusterServerInitCallback,
                                                 ESPMatterGroupcastClusterServerShutdownCallback);
    }
#endif // defined(CONFIG_SUPPORT_GROUPCAST_CLUSTER)

    return cluster;
}

} /* groupcast */
} /* cluster */
} /* esp_matter */
