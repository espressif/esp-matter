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
#include <color_control.h>
#include <color_control_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "color_control_cluster";
constexpr uint16_t cluster_revision = 9;

static esp_err_t esp_matter_command_callback_move_to_hue(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                         void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::MoveToHue::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveToHueCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move_hue(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                      void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::MoveHue::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveHueCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_step_hue(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                      void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::StepHue::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterStepHueCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move_to_saturation(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::MoveToSaturation::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveToSaturationCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move_saturation(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                             void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::MoveSaturation::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveSaturationCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_step_saturation(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                             void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::StepSaturation::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterStepSaturationCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move_to_hue_and_saturation(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                        void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::MoveToHueAndSaturation::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveToHueAndSaturationCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move_to_color(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                           void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::MoveToColor::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveToColorCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move_color(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                        void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::MoveColor::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveColorCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_step_color(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                        void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::StepColor::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterStepColorCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move_to_color_temperature(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                       void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::MoveToColorTemperature::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveToColorTemperatureCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_enhanced_move_to_hue(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                  void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::EnhancedMoveToHue::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterEnhancedMoveToHueCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_enhanced_move_hue(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                               void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::EnhancedMoveHue::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterEnhancedMoveHueCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_enhanced_step_hue(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                               void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::EnhancedStepHue::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterEnhancedStepHueCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_enhanced_move_to_hue_and_saturation(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                                 void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::EnhancedMoveToHueAndSaturation::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterEnhancedMoveToHueAndSaturationCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_color_loop_set(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                            void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::ColorLoopSet::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterColorLoopSetCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_stop_move_step(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                            void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::StopMoveStep::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterStopMoveStepCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move_color_temperature(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                    void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::MoveColorTemperature::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveColorTemperatureCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_step_color_temperature(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                    void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::StepColorTemperature::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterStepColorTemperatureCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

namespace esp_matter {
namespace cluster {
namespace color_control {

namespace feature {
namespace hue_saturation {
uint32_t get_id()
{
    return HueSaturation::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_current_hue(cluster, config->current_hue);
    attribute::create_current_saturation(cluster, config->current_saturation);
    command::create_move_to_hue(cluster);
    command::create_move_hue(cluster);
    command::create_step_hue(cluster);
    command::create_move_to_saturation(cluster);
    command::create_move_saturation(cluster);
    command::create_step_saturation(cluster);
    command::create_move_to_hue_and_saturation(cluster);
    command::create_stop_move_step(cluster);

    return ESP_OK;
}
} /* hue_saturation */

namespace enhanced_hue {
uint32_t get_id()
{
    return EnhancedHue::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_enhanced_current_hue(cluster, config->enhanced_current_hue);
    command::create_enhanced_move_to_hue(cluster);
    command::create_enhanced_move_hue(cluster);
    command::create_enhanced_step_hue(cluster);
    command::create_enhanced_move_to_hue_and_saturation(cluster);

    return ESP_OK;
}
} /* enhanced_hue */

namespace color_loop {
uint32_t get_id()
{
    return ColorLoop::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_color_loop_active(cluster, config->color_loop_active);
    attribute::create_color_loop_direction(cluster, config->color_loop_direction);
    attribute::create_color_loop_time(cluster, config->color_loop_time);
    attribute::create_color_loop_start_enhanced_hue(cluster, config->color_loop_start_enhanced_hue);
    attribute::create_color_loop_stored_enhanced_hue(cluster, config->color_loop_stored_enhanced_hue);
    command::create_color_loop_set(cluster);

    return ESP_OK;
}
} /* color_loop */

namespace xy {
uint32_t get_id()
{
    return XY::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_current_x(cluster, config->current_x);
    attribute::create_current_y(cluster, config->current_y);
    command::create_move_to_color(cluster);
    command::create_move_color(cluster);
    command::create_step_color(cluster);
    command::create_stop_move_step(cluster);

    return ESP_OK;
}
} /* xy */

namespace color_temperature {
uint32_t get_id()
{
    return ColorTemperature::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_color_temperature_mireds(cluster, config->color_temperature_mireds);
    attribute::create_color_temp_physical_min_mireds(cluster, config->color_temp_physical_min_mireds);
    attribute::create_color_temp_physical_max_mireds(cluster, config->color_temp_physical_max_mireds);
    attribute::create_couple_color_temp_to_level_min_mireds(cluster, config->couple_color_temp_to_level_min_mireds);
    attribute::create_start_up_color_temperature_mireds(cluster, config->start_up_color_temperature_mireds);
    command::create_move_to_color_temperature(cluster);
    command::create_stop_move_step(cluster);
    command::create_move_color_temperature(cluster);
    command::create_step_color_temperature(cluster);

    return ESP_OK;
}
} /* color_temperature */

} /* feature */

namespace attribute {
attribute_t *create_current_hue(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(hue_saturation), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, CurrentHue::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_current_saturation(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(hue_saturation), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, CurrentSaturation::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_remaining_time(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RemainingTime::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_current_x(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(xy), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, CurrentX::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_current_y(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(xy), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, CurrentY::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_drift_compensation(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, DriftCompensation::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(4), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_compensation_text(cluster_t *cluster, char *value, uint16_t length)
{
    VerifyOrReturnValue(length <= k_max_compensation_text_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound"));
    return esp_matter::attribute::create(cluster, CompensationText::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length), k_max_compensation_text_length + 1);
}

attribute_t *create_color_temperature_mireds(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(color_temperature), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, ColorTemperatureMireds::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_color_mode(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ColorMode::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(2), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_options(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Options::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint8_t>(1), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_number_of_primaries(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, NumberOfPrimaries::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(6)));
    return attribute;
}

attribute_t *create_primary_1_x(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Primary1X::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_primary_1_y(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Primary1Y::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_primary_1_intensity(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Primary1Intensity::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(254)));
    return attribute;
}

attribute_t *create_primary_2_x(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Primary2X::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_primary_2_y(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Primary2Y::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_primary_2_intensity(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Primary2Intensity::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(254)));
    return attribute;
}

attribute_t *create_primary_3_x(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Primary3X::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_primary_3_y(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Primary3Y::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_primary_3_intensity(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Primary3Intensity::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(254)));
    return attribute;
}

attribute_t *create_primary_4_x(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Primary4X::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_primary_4_y(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Primary4Y::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_primary_4_intensity(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Primary4Intensity::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(254)));
    return attribute;
}

attribute_t *create_primary_5_x(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Primary5X::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_primary_5_y(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Primary5Y::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_primary_5_intensity(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Primary5Intensity::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(254)));
    return attribute;
}

attribute_t *create_primary_6_x(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Primary6X::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_primary_6_y(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Primary6Y::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_primary_6_intensity(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Primary6Intensity::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(254)));
    return attribute;
}

attribute_t *create_white_point_x(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, WhitePointX::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_white_point_y(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, WhitePointY::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_color_point_rx(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ColorPointRX::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_color_point_ry(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ColorPointRY::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_color_point_r_intensity(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ColorPointRIntensity::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(254)));
    return attribute;
}

attribute_t *create_color_point_gx(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ColorPointGX::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_color_point_gy(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ColorPointGY::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_color_point_g_intensity(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ColorPointGIntensity::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(254)));
    return attribute;
}

attribute_t *create_color_point_bx(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ColorPointBX::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_color_point_by(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ColorPointBY::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_color_point_b_intensity(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ColorPointBIntensity::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(254)));
    return attribute;
}

attribute_t *create_enhanced_current_hue(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(enhanced_hue), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, EnhancedCurrentHue::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_enhanced_color_mode(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, EnhancedColorMode::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(3), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_color_loop_active(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(color_loop), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, ColorLoopActive::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(1)));
    return attribute;
}

attribute_t *create_color_loop_direction(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(color_loop), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, ColorLoopDirection::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(1), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_color_loop_time(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(color_loop), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, ColorLoopTime::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_color_loop_start_enhanced_hue(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(color_loop), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, ColorLoopStartEnhancedHue::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_color_loop_stored_enhanced_hue(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(color_loop), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, ColorLoopStoredEnhancedHue::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_color_capabilities(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ColorCapabilities::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint16_t>(65535), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_color_temp_physical_min_mireds(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(color_temperature), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, ColorTempPhysicalMinMireds::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(1)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_color_temp_physical_max_mireds(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(color_temperature), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, ColorTempPhysicalMaxMireds::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65279)));
    return attribute;
}

attribute_t *create_couple_color_temp_to_level_min_mireds(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(color_temperature), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, CoupleColorTempToLevelMinMireds::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_start_up_color_temperature_mireds(cluster_t *cluster, nullable<uint16_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(color_temperature), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, StartUpColorTemperatureMireds::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint16_t>(1)), esp_matter_attr_val(nullable<uint16_t>(65279)));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_move_to_hue(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(hue_saturation), NULL);
    return esp_matter::command::create(cluster, MoveToHue::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_move_to_hue);
}

command_t *create_move_hue(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(hue_saturation), NULL);
    return esp_matter::command::create(cluster, MoveHue::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_move_hue);
}

command_t *create_step_hue(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(hue_saturation), NULL);
    return esp_matter::command::create(cluster, StepHue::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_step_hue);
}

command_t *create_move_to_saturation(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(hue_saturation), NULL);
    return esp_matter::command::create(cluster, MoveToSaturation::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_move_to_saturation);
}

command_t *create_move_saturation(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(hue_saturation), NULL);
    return esp_matter::command::create(cluster, MoveSaturation::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_move_saturation);
}

command_t *create_step_saturation(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(hue_saturation), NULL);
    return esp_matter::command::create(cluster, StepSaturation::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_step_saturation);
}

command_t *create_move_to_hue_and_saturation(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(hue_saturation), NULL);
    return esp_matter::command::create(cluster, MoveToHueAndSaturation::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_move_to_hue_and_saturation);
}

command_t *create_move_to_color(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(xy), NULL);
    return esp_matter::command::create(cluster, MoveToColor::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_move_to_color);
}

command_t *create_move_color(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(xy), NULL);
    return esp_matter::command::create(cluster, MoveColor::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_move_color);
}

command_t *create_step_color(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(xy), NULL);
    return esp_matter::command::create(cluster, StepColor::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_step_color);
}

command_t *create_move_to_color_temperature(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(color_temperature), NULL);
    return esp_matter::command::create(cluster, MoveToColorTemperature::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_move_to_color_temperature);
}

command_t *create_enhanced_move_to_hue(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(enhanced_hue), NULL);
    return esp_matter::command::create(cluster, EnhancedMoveToHue::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_enhanced_move_to_hue);
}

command_t *create_enhanced_move_hue(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(enhanced_hue), NULL);
    return esp_matter::command::create(cluster, EnhancedMoveHue::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_enhanced_move_hue);
}

command_t *create_enhanced_step_hue(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(enhanced_hue), NULL);
    return esp_matter::command::create(cluster, EnhancedStepHue::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_enhanced_step_hue);
}

command_t *create_enhanced_move_to_hue_and_saturation(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(enhanced_hue), NULL);
    return esp_matter::command::create(cluster, EnhancedMoveToHueAndSaturation::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_enhanced_move_to_hue_and_saturation);
}

command_t *create_color_loop_set(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(color_loop), NULL);
    return esp_matter::command::create(cluster, ColorLoopSet::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_color_loop_set);
}

command_t *create_stop_move_step(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(hue_saturation)) || (has_feature(xy)) || (has_feature(color_temperature))), NULL);
    return esp_matter::command::create(cluster, StopMoveStep::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_stop_move_step);
}

command_t *create_move_color_temperature(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(color_temperature), NULL);
    return esp_matter::command::create(cluster, MoveColorTemperature::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_move_color_temperature);
}

command_t *create_step_color_temperature(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(color_temperature), NULL);
    return esp_matter::command::create(cluster, StepColorTemperature::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_step_color_temperature);
}

} /* command */

static void create_default_binding_cluster(endpoint_t *endpoint)
{
    binding::config_t config;
    binding::create(endpoint, &config, CLUSTER_FLAG_SERVER);
}

const function_generic_t function_list[] = {
    (function_generic_t)emberAfColorControlClusterServerInitCallback,
    (function_generic_t)MatterColorControlClusterServerShutdownCallback,
};

const int function_flags = CLUSTER_FLAG_INIT_FUNCTION | CLUSTER_FLAG_SHUTDOWN_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, color_control::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, color_control::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterColorControlPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_color_mode(cluster, config->color_mode);
        attribute::create_options(cluster, config->options);
        attribute::create_number_of_primaries(cluster, config->number_of_primaries);
        attribute::create_enhanced_color_mode(cluster, config->enhanced_color_mode);
        attribute::create_color_capabilities(cluster, config->color_capabilities);
        if (config->number_of_primaries.value_or(0) > 0) {
            attribute::create_primary_1_x(cluster, config->primary_1_x);
            attribute::create_primary_1_y(cluster, config->primary_1_y);
            attribute::create_primary_1_intensity(cluster, config->primary_1_intensity);
        }
        if (config->number_of_primaries.value_or(0) > 1) {
            attribute::create_primary_2_x(cluster, config->primary_2_x);
            attribute::create_primary_2_y(cluster, config->primary_2_y);
            attribute::create_primary_2_intensity(cluster, config->primary_2_intensity);
        }
        if (config->number_of_primaries.value_or(0) > 2) {
            attribute::create_primary_3_x(cluster, config->primary_3_x);
            attribute::create_primary_3_y(cluster, config->primary_3_y);
            attribute::create_primary_3_intensity(cluster, config->primary_3_intensity);
        }
        if (config->number_of_primaries.value_or(0) > 3) {
            attribute::create_primary_4_x(cluster, config->primary_4_x);
            attribute::create_primary_4_y(cluster, config->primary_4_y);
            attribute::create_primary_4_intensity(cluster, config->primary_4_intensity);
        }
        if (config->number_of_primaries.value_or(0) > 4) {
            attribute::create_primary_5_x(cluster, config->primary_5_x);
            attribute::create_primary_5_y(cluster, config->primary_5_y);
            attribute::create_primary_5_intensity(cluster, config->primary_5_intensity);
        }
        if (config->number_of_primaries.value_or(0) > 5) {
            attribute::create_primary_6_x(cluster, config->primary_6_x);
            attribute::create_primary_6_y(cluster, config->primary_6_y);
            attribute::create_primary_6_intensity(cluster, config->primary_6_intensity);
        }
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* color_control */
} /* cluster */
} /* esp_matter */
