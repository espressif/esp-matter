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
#include <on_off.h>
#include <on_off_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 6;

static esp_err_t esp_matter_command_callback_off(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                 void *opaque_ptr)
{
    chip::app::Clusters::OnOff::Commands::Off::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOnOffClusterOffCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_on(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                void *opaque_ptr)
{
    chip::app::Clusters::OnOff::Commands::On::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOnOffClusterOnCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_toggle(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                    void *opaque_ptr)
{
    chip::app::Clusters::OnOff::Commands::Toggle::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOnOffClusterToggleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_off_with_effect(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                             void *opaque_ptr)
{
    chip::app::Clusters::OnOff::Commands::OffWithEffect::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOnOffClusterOffWithEffectCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_on_with_recall_global_scene(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                         void *opaque_ptr)
{
    chip::app::Clusters::OnOff::Commands::OnWithRecallGlobalScene::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOnOffClusterOnWithRecallGlobalSceneCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_on_with_timed_off(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                               void *opaque_ptr)
{
    chip::app::Clusters::OnOff::Commands::OnWithTimedOff::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOnOffClusterOnWithTimedOffCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

namespace esp_matter {
namespace cluster {
namespace on_off {

namespace feature {
namespace lighting {
uint32_t get_id()
{
    return Lighting::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(!(has_feature(off_only)), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_global_scene_control(cluster, config->global_scene_control);
    attribute::create_on_time(cluster, config->on_time);
    attribute::create_off_wait_time(cluster, config->off_wait_time);
    attribute::create_start_up_on_off(cluster, config->start_up_on_off);
    command::create_off_with_effect(cluster);
    command::create_on_with_recall_global_scene(cluster);
    command::create_on_with_timed_off(cluster);

    return ESP_OK;
}
} /* lighting */

namespace dead_front_behavior {
uint32_t get_id()
{
    return DeadFrontBehavior::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(!(has_feature(off_only)), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* dead_front_behavior */

namespace off_only {
uint32_t get_id()
{
    return OffOnly::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(!(((has_feature(lighting)) || (has_feature(dead_front_behavior)))), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command_t *on = esp_matter::command::get(cluster, command::On::Id, COMMAND_FLAG_ACCEPTED);
    if (on) {
        esp_matter::command::destroy(cluster, on);
    }
    command_t *toggle = esp_matter::command::get(cluster, command::Toggle::Id, COMMAND_FLAG_ACCEPTED);
    if (toggle) {
        esp_matter::command::destroy(cluster, toggle);
    }

    return ESP_OK;
}
} /* off_only */

} /* feature */

namespace attribute {
attribute_t *create_on_off(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, OnOff::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_global_scene_control(cluster_t *cluster, bool value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(lighting), NULL);
    return esp_matter::attribute::create(cluster, GlobalSceneControl::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_on_time(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(lighting), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, OnTime::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_off_wait_time(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(lighting), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, OffWaitTime::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_start_up_on_off(cluster_t *cluster, nullable<uint8_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(lighting), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, StartUpOnOff::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(nullable<uint8_t>(2), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_off(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Off::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_off);
}

command_t *create_on(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(!(has_feature(off_only)), NULL);
    return esp_matter::command::create(cluster, On::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_on);
}

command_t *create_toggle(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(!(has_feature(off_only)), NULL);
    return esp_matter::command::create(cluster, Toggle::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_toggle);
}

command_t *create_off_with_effect(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(lighting), NULL);
    return esp_matter::command::create(cluster, OffWithEffect::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_off_with_effect);
}

command_t *create_on_with_recall_global_scene(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(lighting), NULL);
    return esp_matter::command::create(cluster, OnWithRecallGlobalScene::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_on_with_recall_global_scene);
}

command_t *create_on_with_timed_off(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(lighting), NULL);
    return esp_matter::command::create(cluster, OnWithTimedOff::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_on_with_timed_off);
}

} /* command */

const function_generic_t function_list[] = {
    (function_generic_t)emberAfOnOffClusterServerInitCallback,
    (function_generic_t)MatterOnOffClusterServerShutdownCallback,
};

const int function_flags = CLUSTER_FLAG_INIT_FUNCTION | CLUSTER_FLAG_SHUTDOWN_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, on_off::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, on_off::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterOnOffPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_on_off(cluster, config->on_off);
        command::create_off(cluster);
        command::create_on(cluster);
        command::create_toggle(cluster);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* on_off */
} /* cluster */
} /* esp_matter */
