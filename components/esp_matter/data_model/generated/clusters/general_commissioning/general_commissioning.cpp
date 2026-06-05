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
#include <general_commissioning.h>
#include <general_commissioning_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "general_commissioning_cluster";
constexpr uint16_t cluster_revision = 2;

namespace esp_matter {
namespace cluster {
namespace general_commissioning {

namespace feature {
namespace terms_and_conditions {
uint32_t get_id()
{
    return TermsAndConditions::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_tc_accepted_version(cluster, config->tc_accepted_version);
    attribute::create_tc_min_required_version(cluster, config->tc_min_required_version);
    attribute::create_tc_acknowledgements(cluster, config->tc_acknowledgements);
    attribute::create_tc_acknowledgements_required(cluster, config->tc_acknowledgements_required);
    attribute::create_tc_update_deadline(cluster, config->tc_update_deadline);
    command::create_set_tc_acknowledgements(cluster);
    command::create_set_tc_acknowledgements_response(cluster);

    return ESP_OK;
}
} /* terms_and_conditions */

} /* feature */

namespace attribute {
attribute_t *create_breadcrumb(cluster_t *cluster, uint64_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Breadcrumb::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint64_t>(0)), esp_matter_attr_val(static_cast<uint64_t>(4294967294)));
    return attribute;
}

attribute_t *create_basic_commissioning_info(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, BasicCommissioningInfo::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_regulatory_config(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RegulatoryConfig::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(2), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_location_capability(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, LocationCapability::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(2), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_supports_concurrent_connection(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, SupportsConcurrentConnection::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_tc_accepted_version(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(terms_and_conditions), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, TCAcceptedVersion::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_tc_min_required_version(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(terms_and_conditions), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, TCMinRequiredVersion::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_tc_acknowledgements(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(terms_and_conditions), NULL);
    return esp_matter::attribute::create(cluster, TCAcknowledgements::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
}

attribute_t *create_tc_acknowledgements_required(cluster_t *cluster, bool value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(terms_and_conditions), NULL);
    return esp_matter::attribute::create(cluster, TCAcknowledgementsRequired::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_tc_update_deadline(cluster_t *cluster, nullable<uint32_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(terms_and_conditions), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, TCUpdateDeadline::Id, ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_arm_fail_safe(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ArmFailSafe::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_arm_fail_safe_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ArmFailSafeResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_set_regulatory_config(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SetRegulatoryConfig::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_set_regulatory_config_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SetRegulatoryConfigResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_commissioning_complete(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, CommissioningComplete::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_commissioning_complete_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, CommissioningCompleteResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_set_tc_acknowledgements(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(terms_and_conditions), NULL);
    return esp_matter::command::create(cluster, SetTCAcknowledgements::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_set_tc_acknowledgements_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(terms_and_conditions), NULL);
    return esp_matter::command::create(cluster, SetTCAcknowledgementsResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, general_commissioning::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, general_commissioning::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterGeneralCommissioningPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_breadcrumb(cluster, config->breadcrumb);
        attribute::create_regulatory_config(cluster, config->regulatory_config);
        attribute::create_location_capability(cluster, config->location_capability);
        attribute::create_supports_concurrent_connection(cluster, config->supports_concurrent_connection);
        attribute::create_basic_commissioning_info(cluster, NULL, 0, 0);
        command::create_arm_fail_safe(cluster);
        command::create_arm_fail_safe_response(cluster);
        command::create_set_regulatory_config(cluster);
        command::create_set_regulatory_config_response(cluster);
        command::create_commissioning_complete(cluster);
        command::create_commissioning_complete_response(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterGeneralCommissioningClusterServerInitCallback,
                                                 ESPMatterGeneralCommissioningClusterServerShutdownCallback);
    }

    return cluster;
}

} /* general_commissioning */
} /* cluster */
} /* esp_matter */
