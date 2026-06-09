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
#include <level_control.h>
#include <level_control_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "level_control_cluster";
constexpr uint16_t cluster_revision = 7;

static esp_err_t esp_matter_command_callback_move_to_level(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                           void *opaque_ptr)
{
    chip::app::Clusters::LevelControl::Commands::MoveToLevel::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterMoveToLevelCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                  void *opaque_ptr)
{
    chip::app::Clusters::LevelControl::Commands::Move::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterMoveCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_step(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                  void *opaque_ptr)
{
    chip::app::Clusters::LevelControl::Commands::Step::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterStepCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_stop(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                  void *opaque_ptr)
{
    chip::app::Clusters::LevelControl::Commands::Stop::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterStopCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move_to_level_with_on_off(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                       void *opaque_ptr)
{
    chip::app::Clusters::LevelControl::Commands::MoveToLevelWithOnOff::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterMoveToLevelWithOnOffCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move_with_on_off(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                              void *opaque_ptr)
{
    chip::app::Clusters::LevelControl::Commands::MoveWithOnOff::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterMoveWithOnOffCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_step_with_on_off(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                              void *opaque_ptr)
{
    chip::app::Clusters::LevelControl::Commands::StepWithOnOff::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterStepWithOnOffCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_stop_with_on_off(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                              void *opaque_ptr)
{
    chip::app::Clusters::LevelControl::Commands::StopWithOnOff::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterStopWithOnOffCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

namespace esp_matter {
namespace cluster {
namespace level_control {

namespace feature {
namespace on_off {
uint32_t get_id()
{
    return OnOff::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* on_off */

namespace lighting {
uint32_t get_id()
{
    return Lighting::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_remaining_time(cluster, config->remaining_time);
    attribute::create_start_up_current_level(cluster, config->start_up_current_level);

    return ESP_OK;
}
} /* lighting */

} /* feature */

namespace attribute {
attribute_t *create_current_level(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, CurrentLevel::Id, ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(254)));
    return attribute;
}

attribute_t *create_remaining_time(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(lighting), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, RemainingTime::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_min_level(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MinLevel::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_max_level(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxLevel::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_options(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Options::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint8_t>(3), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_on_off_transition_time(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, OnOffTransitionTime::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_on_level(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, OnLevel::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(254)));
    return attribute;
}

attribute_t *create_on_transition_time(cluster_t *cluster, nullable<uint16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, OnTransitionTime::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint16_t>(0)), esp_matter_attr_val(nullable<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_off_transition_time(cluster_t *cluster, nullable<uint16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, OffTransitionTime::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint16_t>(0)), esp_matter_attr_val(nullable<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_default_move_rate(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, DefaultMoveRate::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(1)), esp_matter_attr_val(nullable<uint8_t>(254)));
    return attribute;
}

attribute_t *create_start_up_current_level(cluster_t *cluster, nullable<uint8_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(lighting), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, StartUpCurrentLevel::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(254)));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_move_to_level(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, MoveToLevel::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_move_to_level);
}

command_t *create_move(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Move::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_move);
}

command_t *create_step(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Step::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_step);
}

command_t *create_stop(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Stop::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_stop);
}

command_t *create_move_to_level_with_on_off(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, MoveToLevelWithOnOff::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_move_to_level_with_on_off);
}

command_t *create_move_with_on_off(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, MoveWithOnOff::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_move_with_on_off);
}

command_t *create_step_with_on_off(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, StepWithOnOff::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_step_with_on_off);
}

command_t *create_stop_with_on_off(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, StopWithOnOff::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_stop_with_on_off);
}

} /* command */

static void create_default_binding_cluster(endpoint_t *endpoint)
{
    binding::config_t config;
    binding::create(endpoint, &config, CLUSTER_FLAG_SERVER);
}

const function_generic_t function_list[] = {
    (function_generic_t)emberAfLevelControlClusterServerInitCallback,
    (function_generic_t)MatterLevelControlClusterServerShutdownCallback,
};

const int function_flags = CLUSTER_FLAG_INIT_FUNCTION | CLUSTER_FLAG_SHUTDOWN_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, level_control::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, level_control::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterLevelControlPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_current_level(cluster, config->current_level);
        attribute::create_options(cluster, config->options);
        attribute::create_on_level(cluster, config->on_level);
        command::create_move_to_level(cluster);
        command::create_move(cluster);
        command::create_step(cluster);
        command::create_stop(cluster);
        command::create_move_to_level_with_on_off(cluster);
        command::create_move_with_on_off(cluster);
        command::create_step_with_on_off(cluster);
        command::create_stop_with_on_off(cluster);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* level_control */
} /* cluster */
} /* esp_matter */
