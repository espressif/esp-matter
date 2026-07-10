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
#include <zap_common/app/PluginApplicationCallbacks.h>
#include <esp_matter_delegate_callbacks.h>
#include <window_covering.h>
#include <window_covering_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 8;

static esp_err_t esp_matter_command_callback_up_or_open(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                        void *opaque_ptr)
{
    chip::app::Clusters::WindowCovering::Commands::UpOrOpen::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfWindowCoveringClusterUpOrOpenCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_down_or_close(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                           void *opaque_ptr)
{
    chip::app::Clusters::WindowCovering::Commands::DownOrClose::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfWindowCoveringClusterDownOrCloseCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_stop_motion(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                         void *opaque_ptr)
{
    chip::app::Clusters::WindowCovering::Commands::StopMotion::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfWindowCoveringClusterStopMotionCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_go_to_lift_percentage(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                   void *opaque_ptr)
{
    chip::app::Clusters::WindowCovering::Commands::GoToLiftPercentage::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfWindowCoveringClusterGoToLiftPercentageCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_go_to_tilt_percentage(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                   void *opaque_ptr)
{
    chip::app::Clusters::WindowCovering::Commands::GoToTiltPercentage::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfWindowCoveringClusterGoToTiltPercentageCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

namespace esp_matter {
namespace cluster {
namespace window_covering {

namespace feature {
namespace lift {
uint32_t get_id()
{
    return Lift::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_target_position_lift_percent_100ths(cluster, config->target_position_lift_percent_100ths);
    attribute::create_current_position_lift_percent_100ths(cluster, config->current_position_lift_percent_100ths);
    command::create_go_to_lift_percentage(cluster);

    return ESP_OK;
}
} /* lift */

namespace tilt {
uint32_t get_id()
{
    return Tilt::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_target_position_tilt_percent_100ths(cluster, config->target_position_tilt_percent_100ths);
    attribute::create_current_position_tilt_percent_100ths(cluster, config->current_position_tilt_percent_100ths);
    command::create_go_to_tilt_percentage(cluster);

    return ESP_OK;
}
} /* tilt */

namespace position_aware_lift {
uint32_t get_id()
{
    return PositionAwareLift::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(lift), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_target_position_lift_percent_100ths(cluster, config->target_position_lift_percent_100ths);
    attribute::create_current_position_lift_percent_100ths(cluster, config->current_position_lift_percent_100ths);
    command::create_go_to_lift_percentage(cluster);

    return ESP_OK;
}
} /* position_aware_lift */

namespace position_aware_tilt {
uint32_t get_id()
{
    return PositionAwareTilt::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(tilt), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_target_position_tilt_percent_100ths(cluster, config->target_position_tilt_percent_100ths);
    attribute::create_current_position_tilt_percent_100ths(cluster, config->current_position_tilt_percent_100ths);
    command::create_go_to_tilt_percentage(cluster);

    return ESP_OK;
}
} /* position_aware_tilt */

} /* feature */

namespace attribute {
attribute_t *create_type(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Type::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(Type::Min), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(Type::Max), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_number_of_actuations_lift(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, NumberOfActuationsLift::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_number_of_actuations_tilt(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, NumberOfActuationsTilt::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_config_status(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ConfigStatus::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(ConfigStatus::Min), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint8_t>(ConfigStatus::Max), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_current_position_lift_percentage(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, CurrentPositionLiftPercentage::Id, ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_current_position_tilt_percentage(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, CurrentPositionTiltPercentage::Id, ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_operational_status(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, OperationalStatus::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(OperationalStatus::Min), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint8_t>(OperationalStatus::Max), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_target_position_lift_percent_100ths(cluster_t *cluster, nullable<uint16_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(lift)) && (has_feature(position_aware_lift))), NULL);
    return esp_matter::attribute::create(cluster, TargetPositionLiftPercent100ths::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
}

attribute_t *create_target_position_tilt_percent_100ths(cluster_t *cluster, nullable<uint16_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(tilt)) && (has_feature(position_aware_tilt))), NULL);
    return esp_matter::attribute::create(cluster, TargetPositionTiltPercent100ths::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
}

attribute_t *create_end_product_type(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, EndProductType::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(EndProductType::Min), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(EndProductType::Max), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_current_position_lift_percent_100ths(cluster_t *cluster, nullable<uint16_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(lift)) && (has_feature(position_aware_lift))), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, CurrentPositionLiftPercent100ths::Id, ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint16_t>(CurrentPositionLiftPercent100ths::Min)), esp_matter_attr_val(nullable<uint16_t>(CurrentPositionLiftPercent100ths::Max)));
    return attribute;
}

attribute_t *create_current_position_tilt_percent_100ths(cluster_t *cluster, nullable<uint16_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(tilt)) && (has_feature(position_aware_tilt))), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, CurrentPositionTiltPercent100ths::Id, ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint16_t>(CurrentPositionTiltPercent100ths::Min)), esp_matter_attr_val(nullable<uint16_t>(CurrentPositionTiltPercent100ths::Max)));
    return attribute;
}

attribute_t *create_mode(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Mode::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(Mode::Min), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint8_t>(Mode::Max), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_safety_status(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SafetyStatus::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(SafetyStatus::Min), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint16_t>(SafetyStatus::Max), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_up_or_open(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, UpOrOpen::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_up_or_open);
}

command_t *create_down_or_close(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DownOrClose::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_down_or_close);
}

command_t *create_stop_motion(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, StopMotion::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_stop_motion);
}

command_t *create_go_to_lift_percentage(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GoToLiftPercentage::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_go_to_lift_percentage);
}

command_t *create_go_to_tilt_percentage(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GoToTiltPercentage::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_go_to_tilt_percentage);
}

} /* command */

const function_generic_t function_list[] = {
    (function_generic_t)MatterWindowCoveringClusterServerAttributeChangedCallback,
};

const int function_flags = CLUSTER_FLAG_ATTRIBUTE_CHANGED_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, window_covering::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, window_covering::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = WindowCoveringDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterWindowCoveringPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_type(cluster, config->type);
        attribute::create_config_status(cluster, config->config_status);
        attribute::create_operational_status(cluster, config->operational_status);
        attribute::create_end_product_type(cluster, config->end_product_type);
        attribute::create_mode(cluster, config->mode);

        uint32_t feature_map = config->feature_flags;
        VALIDATE_FEATURES_AT_LEAST_ONE("Lift,Tilt",
                                       feature::lift::get_id(), feature::tilt::get_id());
        if (feature_map & feature::lift::get_id()) {
            VerifyOrReturnValue(feature::lift::add(cluster, &(config->features.lift)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::tilt::get_id()) {
            VerifyOrReturnValue(feature::tilt::add(cluster, &(config->features.tilt)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::position_aware_lift::get_id()) {
            VerifyOrReturnValue(feature::position_aware_lift::add(cluster, &(config->features.position_aware_lift)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::position_aware_tilt::get_id()) {
            VerifyOrReturnValue(feature::position_aware_tilt::add(cluster, &(config->features.position_aware_tilt)) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        command::create_up_or_open(cluster);
        command::create_down_or_close(cluster);
        command::create_stop_motion(cluster);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* window_covering */
} /* cluster */
} /* esp_matter */
