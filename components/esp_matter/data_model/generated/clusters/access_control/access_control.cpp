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
#include <access_control.h>
#include <access_control_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "access_control_cluster";
constexpr uint16_t cluster_revision = 3;

namespace esp_matter {
namespace cluster {
namespace access_control {

namespace feature {
namespace extension {
uint32_t get_id()
{
    return Extension::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_extension(cluster, NULL, 0, 0);
    event::create_access_control_extension_changed(cluster);

    return ESP_OK;
}
} /* extension */

namespace managed_device {
uint32_t get_id()
{
    return ManagedDevice::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
#if CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
    attribute::create_commissioning_arl(cluster, NULL, 0, 0);
#endif // CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
#if CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
    attribute::create_arl(cluster, NULL, 0, 0);
#endif // CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
#if CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
    command::create_review_fabric_restrictions(cluster);
#endif // CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
    command::create_review_fabric_restrictions_response(cluster);
    event::create_fabric_restriction_review_update(cluster);

    return ESP_OK;
}
} /* managed_device */

} /* feature */

namespace attribute {
attribute_t *create_acl(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ACL::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_extension(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(extension), NULL);
    return esp_matter::attribute::create(cluster, Extension::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_subjects_per_access_control_entry(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SubjectsPerAccessControlEntry::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(4)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_targets_per_access_control_entry(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TargetsPerAccessControlEntry::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(3)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_access_control_entries_per_fabric(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, AccessControlEntriesPerFabric::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(4)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

#if CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
attribute_t *create_commissioning_arl(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(managed_device), NULL);
    return esp_matter::attribute::create(cluster, CommissioningARL::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}
#endif // CHIP_CONFIG_USE_ACCESS_RESTRICTIONS

#if CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
attribute_t *create_arl(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(managed_device), NULL);
    return esp_matter::attribute::create(cluster, ARL::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}
#endif // CHIP_CONFIG_USE_ACCESS_RESTRICTIONS

} /* attribute */
namespace command {
#if CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
command_t *create_review_fabric_restrictions(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(managed_device), NULL);
    return esp_matter::command::create(cluster, ReviewFabricRestrictions::Id, COMMAND_FLAG_ACCEPTED, NULL);
}
#endif // CHIP_CONFIG_USE_ACCESS_RESTRICTIONS

command_t *create_review_fabric_restrictions_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(managed_device), NULL);
    return esp_matter::command::create(cluster, ReviewFabricRestrictionsResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */

namespace event {
event_t *create_access_control_entry_changed(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, AccessControlEntryChanged::Id);
}

event_t *create_access_control_extension_changed(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(extension), NULL);
    return esp_matter::event::create(cluster, AccessControlExtensionChanged::Id);
}

event_t *create_fabric_restriction_review_update(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(managed_device), NULL);
    return esp_matter::event::create(cluster, FabricRestrictionReviewUpdate::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, access_control::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, access_control::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterAccessControlPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_subjects_per_access_control_entry(cluster, config->subjects_per_access_control_entry);
        attribute::create_targets_per_access_control_entry(cluster, config->targets_per_access_control_entry);
        attribute::create_access_control_entries_per_fabric(cluster, config->access_control_entries_per_fabric);
        attribute::create_acl(cluster, NULL, 0, 0);
        /* Events */
        event::create_access_control_entry_changed(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterAccessControlClusterServerInitCallback,
                                                 ESPMatterAccessControlClusterServerShutdownCallback);
    }

    return cluster;
}

} /* access_control */
} /* cluster */
} /* esp_matter */
