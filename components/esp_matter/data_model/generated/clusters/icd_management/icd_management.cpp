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
#include <icd_management.h>
#include <icd_management_ids.h>
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
namespace icd_management {

namespace feature {
namespace check_in_protocol_support {
uint32_t get_id()
{
    return CheckInProtocolSupport::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_icd_counter(cluster, config->icd_counter);
    attribute::create_clients_supported_per_fabric(cluster, config->clients_supported_per_fabric);
    attribute::create_maximum_check_in_backoff(cluster, config->maximum_check_in_backoff);
    attribute::create_registered_clients(cluster, NULL, 0, 0);
    command::create_register_client(cluster);
    command::create_register_client_response(cluster);
    command::create_unregister_client(cluster);

    return ESP_OK;
}
} /* check_in_protocol_support */

namespace user_active_mode_trigger {
uint32_t get_id()
{
    return UserActiveModeTrigger::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_user_active_mode_trigger_hint(cluster, config->user_active_mode_trigger_hint);

    return ESP_OK;
}
} /* user_active_mode_trigger */

namespace long_idle_time_support {
uint32_t get_id()
{
    return LongIdleTimeSupport::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_operating_mode(cluster, config->operating_mode);
    command::create_stay_active_request(cluster);
    command::create_stay_active_response(cluster);

    return ESP_OK;
}
} /* long_idle_time_support */

namespace dynamic_sit_lit_support {
uint32_t get_id()
{
    return DynamicSitLitSupport::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(long_idle_time_support), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* dynamic_sit_lit_support */

} /* feature */

namespace attribute {
attribute_t *create_idle_mode_duration(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, IdleModeDuration::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(IdleModeDuration::Min)), esp_matter_attr_val(static_cast<uint32_t>(IdleModeDuration::Max)));
    return attribute;
}

attribute_t *create_active_mode_duration(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, ActiveModeDuration::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_active_mode_threshold(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ActiveModeThreshold::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_registered_clients(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(check_in_protocol_support), NULL);
    return esp_matter::attribute::create(cluster, RegisteredClients::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_icd_counter(cluster_t *cluster, uint32_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(check_in_protocol_support), NULL);
    return esp_matter::attribute::create(cluster, ICDCounter::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_clients_supported_per_fabric(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(check_in_protocol_support), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, ClientsSupportedPerFabric::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(ClientsSupportedPerFabric::Min)), esp_matter_attr_val(static_cast<uint16_t>(ClientsSupportedPerFabric::Max)));
    return attribute;
}

attribute_t *create_user_active_mode_trigger_hint(cluster_t *cluster, uint32_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(user_active_mode_trigger), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, UserActiveModeTriggerHint::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(UserActiveModeTriggerHint::Min), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint32_t>(UserActiveModeTriggerHint::Max), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_user_active_mode_trigger_instruction(cluster_t *cluster, char *value, uint16_t length)
{
    VerifyOrReturnValue(length <= k_max_user_active_mode_trigger_instruction_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound. cluster_id: 0x%08" PRIX32, icd_management::Id));
    return esp_matter::attribute::create(cluster, UserActiveModeTriggerInstruction::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length), k_max_user_active_mode_trigger_instruction_length + 1);
}

attribute_t *create_operating_mode(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(long_idle_time_support), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, OperatingMode::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(OperatingMode::Min), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(OperatingMode::Max), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_maximum_check_in_backoff(cluster_t *cluster, uint32_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(check_in_protocol_support), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaximumCheckInBackoff::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(MaximumCheckInBackoff::Min)), esp_matter_attr_val(static_cast<uint32_t>(MaximumCheckInBackoff::Max)));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_register_client(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(check_in_protocol_support), NULL);
    return esp_matter::command::create(cluster, RegisterClient::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_register_client_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(check_in_protocol_support), NULL);
    return esp_matter::command::create(cluster, RegisterClientResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_unregister_client(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(check_in_protocol_support), NULL);
    return esp_matter::command::create(cluster, UnregisterClient::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_stay_active_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, StayActiveRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_stay_active_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, StayActiveResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, icd_management::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, icd_management::Id));
#if CHIP_CONFIG_ENABLE_ICD_SERVER
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_idle_mode_duration(cluster, config->idle_mode_duration);
        attribute::create_active_mode_duration(cluster, config->active_mode_duration);
        attribute::create_active_mode_threshold(cluster, config->active_mode_threshold);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterIcdManagementClusterServerInitCallback,
                                                 ESPMatterIcdManagementClusterServerShutdownCallback);
    }
#endif // CHIP_CONFIG_ENABLE_ICD_SERVER

    return cluster;
}

} /* icd_management */
} /* cluster */
} /* esp_matter */
