// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_feature.h>

#include <app-common/zap-generated/cluster-enums.h>

static const char *TAG = "esp_matter_feature";

using namespace chip::app::Clusters;

namespace esp_matter {
namespace cluster {

static esp_err_t update_feature_map(cluster_t *cluster, uint32_t value)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));

    /* Get the attribute */
    attribute_t *attribute = attribute::get(cluster, Globals::Attributes::FeatureMap::Id);

    VerifyOrReturnError(attribute, ESP_ERR_INVALID_STATE, ESP_LOGE(TAG, "Feature map attribute cannot be null"));

    /* Update the value if the attribute already exists */
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    attribute::get_val(attribute, &val);
    val.val.u32 |= value;
    /* Here we can't call attribute::update() since the chip stack would not have started yet, since we are
    still creating the data model. So, we are directly using attribute::set_val(). */
    return attribute::set_val(attribute, &val);
}

static uint32_t get_feature_map_value(cluster_t *cluster)
{
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    attribute_t *attribute = attribute::get(cluster, Globals::Attributes::FeatureMap::Id);

    VerifyOrReturnValue(attribute, 0, ESP_LOGE(TAG, "Feature map attribute cannot be null"));

    attribute::get_val(attribute, &val);
    return val.val.u32;
}

namespace descriptor {
namespace feature {
namespace taglist {

uint32_t get_id()
{
    return (uint32_t)Descriptor::Feature::kTagList;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes managed internally */
    attribute::create_tag_list(cluster, NULL, 0, 0);

    return ESP_OK;
}

} /* taglist */

}
}

namespace access_control {
namespace feature {
namespace extension {

uint32_t get_id()
{
    return (uint32_t)AccessControl::Feature::kExtension;
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    /* Attributes managed internally */
    attribute::create_extension(cluster, NULL, 0, 0);

    event::create_access_control_extension_changed(cluster);

    return ESP_OK;
}
} /* extension */

namespace managed_device {

uint32_t get_id()
{
    return (uint32_t)AccessControl::Feature::kManagedDevice;
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

#if CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
    /* Attributes managed internally */
    attribute::create_commissioning_arl(cluster, NULL, 0, 0);
    attribute::create_arl(cluster, NULL, 0, 0);
#endif

    command::create_review_fabric_restrictions(cluster);
    command::create_review_fabric_restrictions_response(cluster);

    event::create_fabric_restriction_review_update(cluster);

    return ESP_OK;
}
} /* managed_device */

}
} /* access_control */

namespace bridged_device_basic_information {
namespace feature {
namespace bridged_icd_support {

uint32_t get_id()
{
    return (uint32_t)BridgedDeviceBasicInformation::Feature::kBridgedICDSupport;
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    command::create_keep_active(cluster);
    event::create_active_changed(cluster);

    return ESP_OK;
}

} /* bridged_icd_support */

} /* feature */
} /* bridged_device_basic_information */

namespace administrator_commissioning {
namespace feature {
namespace basic {

uint32_t get_id()
{
    return (uint32_t)AdministratorCommissioning::Feature::kBasic;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes managed internally */
    command::create_open_basic_commissioning_window(cluster);

    return ESP_OK;
}
} /* basic */

}
} /* administrator_commissioning */

namespace general_commissioning {
namespace feature {
namespace terms_and_conditions {

uint32_t get_id()
{
    return (uint32_t)GeneralCommissioning::Feature::kTermsAndConditions;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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

}
} /* general_commissioning */

namespace power_source {
namespace feature {
namespace wired {

uint32_t get_id()
{
    return (uint32_t)PowerSource::Feature::kWired;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    uint32_t battery_feature_map = feature::battery::get_id();
    if ((get_feature_map_value(cluster) & battery_feature_map) == battery_feature_map) {
        ESP_LOGE(TAG, "Cluster already supports Battery feature");
        return ESP_ERR_NOT_SUPPORTED;
    }
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_wired_current_type(cluster, config->wired_current_type);

    return ESP_OK;
}

} /* wired */

namespace battery {

uint32_t get_id()
{
    return (uint32_t)PowerSource::Feature::kBattery;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    uint32_t wired_feature_map = feature::wired::get_id();
    if ((get_feature_map_value(cluster) & wired_feature_map) == wired_feature_map) {
        ESP_LOGE(TAG, "Cluster already supports Wired feature");
        return ESP_ERR_NOT_SUPPORTED;
    }
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_bat_charge_level(cluster, config->bat_charge_level);
    attribute::create_bat_replacement_needed(cluster, config->bat_replacement_needed);
    attribute::create_bat_replaceability(cluster, config->bat_replaceability);

    return ESP_OK;
}

} /* battery */

namespace rechargeable {

uint32_t get_id()
{
    return (uint32_t)PowerSource::Feature::kRechargeable;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    uint32_t battery_feature_map = feature::battery::get_id();
    if ((get_feature_map_value(cluster) & battery_feature_map) == battery_feature_map) {

        update_feature_map(cluster, get_id());

        /* Attributes not managed internally */
        attribute::create_bat_charge_state(cluster, config->bat_charge_state);
        attribute::create_bat_functional_while_charging(cluster, config->bat_functional_while_charging);
    } else {
        ESP_LOGE(TAG, "Cluster shall support Battery feature");
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

} /* rechargeable */

namespace replaceable {

uint32_t get_id()
{
    return (uint32_t)PowerSource::Feature::kReplaceable;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    uint32_t battery_feature_map = feature::battery::get_id();
    if ((get_feature_map_value(cluster) & battery_feature_map) == battery_feature_map) {

        update_feature_map(cluster, get_id());

        /* Attributes not managed internally */
        attribute::create_bat_replacement_description(cluster, config->bat_replacement_description, strlen(config->bat_replacement_description));
        attribute::create_bat_quantity(cluster, config->bat_quantity, 0, 255);
    } else {
        ESP_LOGE(TAG, "Cluster shall support Battery feature");
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

} /* replaceable */
} /* feature */
} /* power_source */

namespace scenes_management {
namespace feature {
namespace scene_names {

uint32_t get_id()
{
    return (uint32_t)ScenesManagement::Feature::kSceneNames;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* scene_names */
} /* feature */
} /* scenes_management */

namespace icd_management {
namespace feature {
namespace check_in_protocol_support {

uint32_t get_id()
{
    return (uint32_t)IcdManagement::Feature::kCheckInProtocolSupport;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    uint32_t lits_feature_map = feature::long_idle_time_support::get_id();
    VerifyOrReturnError((get_feature_map_value(cluster) & lits_feature_map) == lits_feature_map, ESP_ERR_INVALID_STATE, ESP_LOGE(TAG, "Long Idle Time Support feature should be added to this cluster"));

    update_feature_map(cluster, get_id());

    /* Attributes managed internally */
    attribute::create_registered_clients(cluster, NULL, 0, 0);
    attribute::create_icd_counter(cluster, 0);
    attribute::create_clients_supported_per_fabric(cluster, 0);

    /* Commands */
    command::create_register_client(cluster);
    command::create_register_client_response(cluster);
    command::create_unregister_client(cluster);

    return ESP_OK;
}

} /* check_in_protocol_support */

namespace user_active_mode_trigger {

uint32_t get_id()
{
    return (uint32_t)IcdManagement::Feature::kUserActiveModeTrigger;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError((cluster && config), ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster and config cannot be NULL"));
    uint32_t lits_feature_map = feature::long_idle_time_support::get_id();
    VerifyOrReturnError((get_feature_map_value(cluster) & lits_feature_map) == lits_feature_map, ESP_ERR_INVALID_STATE, ESP_LOGE(TAG, "Long Idle Time Support feature should be added to this cluster"));

    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_user_active_mode_trigger_hint(cluster, config->user_active_mode_trigger_hint);
    attribute::create_user_active_mode_trigger_instruction(cluster, config->user_active_mode_trigger_instruction,
                                                           strlen(config->user_active_mode_trigger_instruction));

    return ESP_OK;
}

} /* user_active_mode_trigger */

namespace long_idle_time_support {

uint32_t get_id()
{
    return (uint32_t)IcdManagement::Feature::kLongIdleTimeSupport;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_operating_mode(cluster, 0);
    return ESP_OK;
}

} /* long_idle_time_support */
} /* feature */
} /* icd_management */

namespace on_off {
namespace feature {
namespace lighting {

uint32_t get_id()
{
    return (uint32_t)OnOff::Feature::kLighting;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_global_scene_control(cluster, config->global_scene_control);
    attribute::create_on_time(cluster, config->on_time);
    attribute::create_off_wait_time(cluster, config->off_wait_time);
    attribute::create_start_up_on_off(cluster, config->start_up_on_off);

    /* Commands */
    command::create_off_with_effect(cluster);
    command::create_on_with_recall_global_scene(cluster);
    command::create_on_with_timed_off(cluster);

    return ESP_OK;
}

} /* lighting */

namespace dead_front_behavior {

uint32_t get_id()
{
    return (uint32_t)OnOff::Feature::kDeadFrontBehavior;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* dead_front_behavior */

namespace off_only {

uint32_t get_id()
{
    return (uint32_t)OnOff::Feature::kOffOnly;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* off_only */
} /* feature */
} /* on_off */

namespace level_control {
namespace feature {
namespace on_off {

uint32_t get_id()
{
    return (uint32_t)LevelControl::Feature::kOnOff;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* on_off */

namespace lighting {

uint32_t get_id()
{
    return (uint32_t)LevelControl::Feature::kLighting;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_remaining_time(cluster, config->remaining_time);
    attribute::create_min_level(cluster, config->min_level);
    attribute::create_max_level(cluster, config->max_level);
    attribute::create_start_up_current_level(cluster, config->start_up_current_level);

    return ESP_OK;
}

} /* lighting */

namespace frequency {

uint32_t get_id()
{
    return (uint32_t)LevelControl::Feature::kFrequency;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_current_frequency(cluster, config->current_frequency);
    attribute::create_min_frequency(cluster, config->min_frequency);
    attribute::create_max_frequency(cluster, config->max_frequency);

    /* Commands */
    command::create_move_to_closest_frequency(cluster);

    return ESP_OK;
}
} /* frequency */
} /* feature */
} /* level_control */

namespace color_control {
namespace feature {

static esp_err_t update_color_capability(cluster_t *cluster, uint16_t value)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));

    /* Get the attribute */
    attribute_t *attribute = esp_matter::attribute::get(cluster, ColorControl::Attributes::ColorCapabilities::Id);

    /* Print error log if it does not exist */
    VerifyOrReturnError(attribute, ESP_FAIL, ESP_LOGE(TAG, "The color capability attribute is NULL"));

    /* Update the value if the attribute already exists */
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    esp_matter::attribute::get_val(attribute, &val);
    val.val.u16 |= value;
    /* Here we can't call attribute::update() since the chip stack would not have started yet, since we are
    still creating the data model. So, we are directly using attribute::set_val(). */
    return esp_matter::attribute::set_val(attribute, &val);
}

namespace hue_saturation {

uint32_t get_id()
{
    return (uint32_t)ColorControl::ColorCapabilitiesBitmap::kHueSaturation;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());
    update_color_capability(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_current_hue(cluster, config->current_hue);
    attribute::create_current_saturation(cluster, config->current_saturation);

    /* Commands */
    command::create_move_to_hue(cluster);
    command::create_move_hue(cluster);
    command::create_step_hue(cluster);
    command::create_move_to_saturation(cluster);
    command::create_move_saturation(cluster);
    command::create_step_saturation(cluster);
    command::create_move_to_hue_and_saturation(cluster);

    return ESP_OK;
}

} /* hue_saturation */

namespace color_temperature {

uint32_t get_id()
{
    return (uint32_t)ColorControl::ColorCapabilitiesBitmap::kColorTemperature;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());
    update_color_capability(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_color_temperature_mireds(cluster, config->color_temperature_mireds);
    attribute::create_color_temp_physical_min_mireds(cluster, config->color_temp_physical_min_mireds);
    attribute::create_color_temp_physical_max_mireds(cluster, config->color_temp_physical_max_mireds);
    attribute::create_couple_color_temp_to_level_min_mireds(cluster, config->couple_color_temp_to_level_min_mireds);
    attribute::create_startup_color_temperature_mireds(cluster, config->startup_color_temperature_mireds);

    /* Commands */
    command::create_move_to_color_temperature(cluster);
    command::create_move_color_temperature(cluster);
    command::create_step_color_temperature(cluster);

    return ESP_OK;
}

} /* color_temperature */

namespace xy {

uint32_t get_id()
{
    return (uint32_t)ColorControl::ColorCapabilitiesBitmap::kXy;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());
    update_color_capability(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_current_x(cluster, config->current_x);
    attribute::create_current_y(cluster, config->current_y);

    /* Commands */
    command::create_move_to_color(cluster);
    command::create_move_color(cluster);
    command::create_step_color(cluster);

    return ESP_OK;
}

} /* xy */

namespace enhanced_hue {

uint32_t get_id()
{
    return (uint32_t)chip::app::Clusters::ColorControl::ColorCapabilitiesBitmap::kEnhancedHue;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    uint32_t hs_feature_map = feature::hue_saturation::get_id();
    if ((get_feature_map_value(cluster) & hs_feature_map) == hs_feature_map) {
        update_feature_map(cluster, get_id());
        update_color_capability(cluster, get_id());

        /* Attributes not managed internally */
        attribute::create_enhanced_current_hue(cluster, config->enhanced_current_hue);

        /* Commands */
        command::create_enhanced_move_to_hue(cluster);
        command::create_enhanced_move_hue(cluster);
        command::create_enhanced_step_hue(cluster);
        command::create_enhanced_move_to_hue_and_saturation(cluster);

    } else {
        ESP_LOGE(TAG, "Cluster shall support hue_saturation feature");
        return ESP_ERR_NOT_SUPPORTED;
    }
    return ESP_OK;
}
} /* enhanced_hue */

namespace color_loop {
uint32_t get_id()
{
    return (uint32_t)chip::app::Clusters::ColorControl::ColorCapabilitiesBitmap::kColorLoop;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    uint32_t eh_feature_map = feature::enhanced_hue::get_id();
    if ((get_feature_map_value(cluster) & eh_feature_map) == eh_feature_map) {
        update_feature_map(cluster, get_id());
        update_color_capability(cluster, get_id());

        /* Attributes not managed internally */
        attribute::create_color_loop_active(cluster, config->color_loop_active);
        attribute::create_color_loop_direction(cluster, config->color_loop_direction);
        attribute::create_color_loop_time(cluster, config->color_loop_time);
        attribute::create_color_loop_start_enhanced_hue(cluster, config->color_loop_start_enhanced_hue);
        attribute::create_color_loop_stored_enhanced_hue(cluster, config->color_loop_stored_enhanced_hue);

        /* Commands */
        command::create_color_loop_set(cluster);
    } else {
        ESP_LOGE(TAG, "Cluster shall support enhanced_hue feature");
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}
} /* color_loop */
} /* feature */
} /* color_control */

namespace window_covering {
namespace feature {
namespace lift {

uint32_t get_id()
{
    return (uint32_t)WindowCovering::Feature::kLift;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_number_of_actuations_lift(cluster, config->number_of_actuations_lift);

    return ESP_OK;
}

} /* lift */

namespace tilt {

uint32_t get_id()
{
    return (uint32_t)WindowCovering::Feature::kTilt;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_number_of_actuations_tilt(cluster, config->number_of_actuations_tilt);

    return ESP_OK;
}

} /* tilt */

namespace position_aware_lift {

uint32_t get_id()
{
    return (uint32_t)WindowCovering::Feature::kPositionAwareLift;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    uint32_t lift_feature_map = feature::lift::get_id();
    if ((get_feature_map_value(cluster) & lift_feature_map) == lift_feature_map) {

        update_feature_map(cluster, get_id());

        attribute::create_current_position_lift_percentage(cluster, config->current_position_lift_percentage);
        attribute::create_target_position_lift_percent_100ths(cluster, config->target_position_lift_percent_100ths);
        attribute::create_current_position_lift_percent_100ths(cluster, config->current_position_lift_percent_100ths);

        command::create_go_to_lift_percentage(cluster);

        // We should update config_status attribute as position_aware_lift feature is added
        uint8_t set_third_bit = 1 << 3;
        attribute_t *attribute = esp_matter::attribute::get(cluster, WindowCovering::Attributes::ConfigStatus::Id);
        esp_matter_attr_val_t val = esp_matter_invalid(NULL);
        esp_matter::attribute::get_val(attribute, &val);
        val.val.u8 = val.val.u8 | set_third_bit;
        esp_matter::attribute::set_val(attribute, &val);
    } else {
        ESP_LOGE(TAG, "Cluster shall support Lift feature");
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}
} /* position_aware_lift */

namespace absolute_position {

uint32_t get_id()
{
    return (uint32_t)WindowCovering::Feature::kAbsolutePosition;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    uint32_t abs_and_pa_lf_and_lf_feature_map = get_id() | feature::position_aware_lift::get_id() | feature::lift::get_id();
    uint32_t abs_and_pa_tl_and_tl_feature_map = get_id() | feature::position_aware_tilt::get_id() | feature::tilt::get_id();
    uint32_t abs_and_lift_feature_map = get_id() | feature::lift::get_id();
    uint32_t abs_and_tilt_feature_map = get_id() | feature::tilt::get_id();
    if (
        (get_feature_map_value(cluster) & abs_and_pa_lf_and_lf_feature_map) != abs_and_pa_lf_and_lf_feature_map
        && (get_feature_map_value(cluster) & abs_and_pa_tl_and_tl_feature_map) != abs_and_pa_tl_and_tl_feature_map
        && (get_feature_map_value(cluster) & abs_and_lift_feature_map) != abs_and_lift_feature_map
        && (get_feature_map_value(cluster) & abs_and_tilt_feature_map) != abs_and_tilt_feature_map
    ) {
        ESP_LOGE(TAG, "Cluster shall support Lift (and optionally Position_Aware_Lift) and/or Tilt (and optionally Position_Aware_Tilt) features");
        return ESP_ERR_NOT_SUPPORTED;
    }
    if ((get_feature_map_value(cluster) & abs_and_pa_lf_and_lf_feature_map) == abs_and_pa_lf_and_lf_feature_map) {
        attribute::create_physical_closed_limit_lift(cluster, config->physical_closed_limit_lift);
        attribute::create_current_position_lift(cluster, config->current_position_lift);
        attribute::create_installed_open_limit_lift(cluster, config->installed_open_limit_lift);
        attribute::create_installed_closed_limit_lift(cluster, config->installed_closed_limit_lift);
    } else {
        ESP_LOGW(TAG, "Lift related attributes were not created because cluster does not support Position_Aware_Lift feature");
    }

    if ((get_feature_map_value(cluster) & abs_and_pa_tl_and_tl_feature_map) == abs_and_pa_tl_and_tl_feature_map) {
        attribute::create_physical_closed_limit_tilt(cluster, config->physical_closed_limit_tilt);
        attribute::create_current_position_tilt(cluster, config->current_position_tilt);
        attribute::create_installed_open_limit_tilt(cluster, config->installed_open_limit_tilt);
        attribute::create_installed_closed_limit_tilt(cluster, config->installed_closed_limit_tilt);
    } else {
        ESP_LOGW(TAG, "Tilt related attributes were not created because cluster does not support Position_Aware_Tilt feature");
    }

    if ((get_feature_map_value(cluster) & abs_and_lift_feature_map) == abs_and_lift_feature_map) {
        command::create_go_to_lift_value(cluster);
    } else {
        ESP_LOGW(TAG, "Lift commands were not created because cluster does not support Lift feature");
    }

    if ((get_feature_map_value(cluster) & abs_and_tilt_feature_map) == abs_and_tilt_feature_map) {
        command::create_go_to_tilt_value(cluster);
    } else {
        ESP_LOGW(TAG, "Tilt commands were not created because cluster does not support Tilt feature");
    }

    return ESP_OK;
}

} /* absolute_position */

namespace position_aware_tilt {

uint32_t get_id()
{
    return (uint32_t)WindowCovering::Feature::kPositionAwareTilt;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    uint32_t tilt_feature_map = feature::tilt::get_id();
    if ((get_feature_map_value(cluster) & tilt_feature_map) == tilt_feature_map) {

        update_feature_map(cluster, get_id());

        attribute::create_current_position_tilt_percentage(cluster, config->current_position_tilt_percentage);
        attribute::create_target_position_tilt_percent_100ths(cluster, config->target_position_tilt_percent_100ths);
        attribute::create_current_position_tilt_percent_100ths(cluster, config->current_position_tilt_percent_100ths);

        command::create_go_to_tilt_percentage(cluster);

        // We should update config_status attribute as position_aware_tilt feature is added
        uint8_t set_fourth_bit = 1 << 4;
        attribute_t *attribute = esp_matter::attribute::get(cluster, WindowCovering::Attributes::ConfigStatus::Id);
        esp_matter_attr_val_t val = esp_matter_invalid(NULL);
        esp_matter::attribute::get_val(attribute, &val);
        val.val.u8 = val.val.u8 | set_fourth_bit;
        esp_matter::attribute::set_val(attribute, &val);
    } else {
        ESP_LOGE(TAG, "Cluster shall support Tilt feature");
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

} /* position_aware_tilt */
} /* feature */
} /* window_covering */

namespace wifi_network_diagnotics {
namespace feature {

namespace packets_counts {

uint32_t get_id()
{
    // The WiFiNetworkDiagnosticsFeature enum class is not added in the upstream code.
    // Return the code according to the SPEC
    return 0x01;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes managed internally */
    attribute::create_beacon_rx_count(cluster, 0);
    attribute::create_packet_multicast_rx_count(cluster, 0);
    attribute::create_packet_multicast_tx_count(cluster, 0);
    attribute::create_packet_unicast_rx_count(cluster, 0);
    attribute::create_packet_unicast_tx_count(cluster, 0);

    return ESP_OK;
}

} /* packets_counts */

namespace error_counts {

uint32_t get_id()
{
    // The WiFiNetworkDiagnosticsFeature enum class is not added in the upstream code.
    // Return the code according to the SPEC
    return 0x02;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes managed internally */
    attribute::create_beacon_lost_count(cluster, 0);
    attribute::create_overrun_count(cluster, 0);

    /* Commands */
    command::create_reset_counts(cluster);

    return ESP_OK;
}

} /* error_counts */

} /* feature */
} /* wifi_network_diagnotics */

namespace ethernet_network_diagnostics {
namespace feature {

namespace packets_counts {

uint32_t get_id()
{
    return (uint32_t)EthernetNetworkDiagnostics::Feature::kPacketCounts;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes managed internally */
    attribute::create_packet_rx_count(cluster, 0);
    attribute::create_packet_tx_count(cluster, 0);

    return ESP_OK;
}

} /* packets_counts */

namespace error_counts {

uint32_t get_id()
{
    return (uint32_t)EthernetNetworkDiagnostics::Feature::kErrorCounts;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes managed internally */
    attribute::create_tx_error_count(cluster, config->tx_error_count);
    attribute::create_collision_count(cluster, config->collision_count);
    attribute::create_overrun_count(cluster, config->overrun_count);

    return ESP_OK;
}

} /* error_counts */

} /* feature */
} /* ethernet_network_diagnostics */

namespace air_quality {
namespace feature {

namespace fair {

uint32_t get_id()
{
    return (uint32_t)AirQuality::Feature::kFair;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* fair */

namespace moderate {

uint32_t get_id()
{
    return (uint32_t)AirQuality::Feature::kModerate;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* moderate */

namespace very_poor {

uint32_t get_id()
{
    return (uint32_t)AirQuality::Feature::kVeryPoor;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* very_poor */

namespace extremely_poor {

uint32_t get_id()
{
    return (uint32_t)AirQuality::Feature::kExtremelyPoor;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* extremely_poor */

} /* feature */
} /* air_quality */

namespace carbon_monoxide_concentration_measurement {
namespace feature {

namespace numeric_measurement {

uint32_t get_id()
{
    return (uint32_t)CarbonMonoxideConcentrationMeasurement::Feature::kNumericMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_measured_value(cluster, config->measured_value);
    attribute::create_min_measured_value(cluster, config->min_measured_value);
    attribute::create_max_measured_value(cluster, config->max_measured_value);
    attribute::create_measurement_unit(cluster, config->measurement_unit);

    return ESP_OK;
}

} /* numeric_measurement */

namespace level_indication {

uint32_t get_id()
{
    return (uint32_t)CarbonMonoxideConcentrationMeasurement::Feature::kLevelIndication;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_level_value(cluster, config->level_value);

    return ESP_OK;
}

} /* level_indication */

namespace medium_level {

uint32_t get_id()
{
    return (uint32_t)CarbonMonoxideConcentrationMeasurement::Feature::kMediumLevel;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* medium_level */

namespace critical_level {

uint32_t get_id()
{
    return (uint32_t)CarbonMonoxideConcentrationMeasurement::Feature::kCriticalLevel;

}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* critical_level */

namespace peak_measurement {

uint32_t get_id()
{
    return (uint32_t)CarbonMonoxideConcentrationMeasurement::Feature::kPeakMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_peak_measured_value(cluster, config->peak_measured_value);
    attribute::create_peak_measured_value_window(cluster, config->peak_measured_value_window);

    return ESP_OK;
}

} /* peak_measurement */

namespace average_measurement {

uint32_t get_id()
{
    return (uint32_t)CarbonMonoxideConcentrationMeasurement::Feature::kAverageMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_average_measured_value(cluster, config->average_measured_value);
    attribute::create_average_measured_value_window(cluster, config->average_measured_value_window);

    return ESP_OK;
}

} /* average_measurement */

} /* feature */
} /* carbon_monoxide_concentration_measurement */

namespace carbon_dioxide_concentration_measurement {
namespace feature {

namespace numeric_measurement {

uint32_t get_id()
{
    return (uint32_t)CarbonDioxideConcentrationMeasurement::Feature::kNumericMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_measured_value(cluster, config->measured_value);
    attribute::create_min_measured_value(cluster, config->min_measured_value);
    attribute::create_max_measured_value(cluster, config->max_measured_value);
    attribute::create_measurement_unit(cluster, config->measurement_unit);

    return ESP_OK;
}

} /* numeric_measurement */

namespace level_indication {

uint32_t get_id()
{
    return (uint32_t)CarbonDioxideConcentrationMeasurement::Feature::kLevelIndication;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_level_value(cluster, config->level_value);

    return ESP_OK;
}

} /* level_indication */

namespace medium_level {

uint32_t get_id()
{
    return (uint32_t)CarbonDioxideConcentrationMeasurement::Feature::kMediumLevel;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* medium_level */

namespace critical_level {

uint32_t get_id()
{
    return (uint32_t)CarbonDioxideConcentrationMeasurement::Feature::kCriticalLevel;

}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* critical_level */

namespace peak_measurement {

uint32_t get_id()
{
    return (uint32_t)CarbonDioxideConcentrationMeasurement::Feature::kPeakMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_peak_measured_value(cluster, config->peak_measured_value);
    attribute::create_peak_measured_value_window(cluster, config->peak_measured_value_window);

    return ESP_OK;
}

} /* peak_measurement */

namespace average_measurement {

uint32_t get_id()
{
    return (uint32_t)CarbonDioxideConcentrationMeasurement::Feature::kAverageMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_average_measured_value(cluster, config->average_measured_value);
    attribute::create_average_measured_value_window(cluster, config->average_measured_value_window);

    return ESP_OK;
}

} /* average_measurement */

} /* feature */
} /* carbon_dioxide_concentration_measurement */

namespace nitrogen_dioxide_concentration_measurement {
namespace feature {

namespace numeric_measurement {

uint32_t get_id()
{
    return (uint32_t)NitrogenDioxideConcentrationMeasurement::Feature::kNumericMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_measured_value(cluster, config->measured_value);
    attribute::create_min_measured_value(cluster, config->min_measured_value);
    attribute::create_max_measured_value(cluster, config->max_measured_value);
    attribute::create_measurement_unit(cluster, config->measurement_unit);

    return ESP_OK;
}

} /* numeric_measurement */

namespace level_indication {

uint32_t get_id()
{
    return (uint32_t)NitrogenDioxideConcentrationMeasurement::Feature::kLevelIndication;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_level_value(cluster, config->level_value);

    return ESP_OK;
}

} /* level_indication */

namespace medium_level {

uint32_t get_id()
{
    return (uint32_t)NitrogenDioxideConcentrationMeasurement::Feature::kMediumLevel;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* medium_level */

namespace critical_level {

uint32_t get_id()
{
    return (uint32_t)NitrogenDioxideConcentrationMeasurement::Feature::kCriticalLevel;

}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* critical_level */

namespace peak_measurement {

uint32_t get_id()
{
    return (uint32_t)NitrogenDioxideConcentrationMeasurement::Feature::kPeakMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_peak_measured_value(cluster, config->peak_measured_value);
    attribute::create_peak_measured_value_window(cluster, config->peak_measured_value_window);

    return ESP_OK;
}

} /* peak_measurement */

namespace average_measurement {

uint32_t get_id()
{
    return (uint32_t)NitrogenDioxideConcentrationMeasurement::Feature::kAverageMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_average_measured_value(cluster, config->average_measured_value);
    attribute::create_average_measured_value_window(cluster, config->average_measured_value_window);

    return ESP_OK;
}

} /* average_measurement */

} /* feature */
} /* nitrogen_dioxide_concentration_measurement */

namespace ozone_concentration_measurement {
namespace feature {

namespace numeric_measurement {

uint32_t get_id()
{
    return (uint32_t)OzoneConcentrationMeasurement::Feature::kNumericMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_measured_value(cluster, config->measured_value);
    attribute::create_min_measured_value(cluster, config->min_measured_value);
    attribute::create_max_measured_value(cluster, config->max_measured_value);
    attribute::create_measurement_unit(cluster, config->measurement_unit);

    return ESP_OK;
}

} /* numeric_measurement */

namespace level_indication {

uint32_t get_id()
{
    return (uint32_t)OzoneConcentrationMeasurement::Feature::kLevelIndication;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_level_value(cluster, config->level_value);

    return ESP_OK;
}

} /* level_indication */

namespace medium_level {

uint32_t get_id()
{
    return (uint32_t)OzoneConcentrationMeasurement::Feature::kMediumLevel;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* medium_level */

namespace critical_level {

uint32_t get_id()
{
    return (uint32_t)OzoneConcentrationMeasurement::Feature::kCriticalLevel;

}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* critical_level */

namespace peak_measurement {

uint32_t get_id()
{
    return (uint32_t)OzoneConcentrationMeasurement::Feature::kPeakMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_peak_measured_value(cluster, config->peak_measured_value);
    attribute::create_peak_measured_value_window(cluster, config->peak_measured_value_window);

    return ESP_OK;
}

} /* peak_measurement */

namespace average_measurement {

uint32_t get_id()
{
    return (uint32_t)OzoneConcentrationMeasurement::Feature::kAverageMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_average_measured_value(cluster, config->average_measured_value);
    attribute::create_average_measured_value_window(cluster, config->average_measured_value_window);

    return ESP_OK;
}

} /* average_measurement */

} /* feature */
} /* ozone_concentration_measurement */

namespace formaldehyde_concentration_measurement {
namespace feature {

namespace numeric_measurement {

uint32_t get_id()
{
    return (uint32_t)FormaldehydeConcentrationMeasurement::Feature::kNumericMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_measured_value(cluster, config->measured_value);
    attribute::create_min_measured_value(cluster, config->min_measured_value);
    attribute::create_max_measured_value(cluster, config->max_measured_value);
    attribute::create_measurement_unit(cluster, config->measurement_unit);

    return ESP_OK;
}

} /* numeric_measurement */

namespace level_indication {

uint32_t get_id()
{
    return (uint32_t)FormaldehydeConcentrationMeasurement::Feature::kLevelIndication;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_level_value(cluster, config->level_value);

    return ESP_OK;
}

} /* level_indication */

namespace medium_level {

uint32_t get_id()
{
    return (uint32_t)FormaldehydeConcentrationMeasurement::Feature::kMediumLevel;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* medium_level */

namespace critical_level {

uint32_t get_id()
{
    return (uint32_t)FormaldehydeConcentrationMeasurement::Feature::kCriticalLevel;

}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* critical_level */

namespace peak_measurement {

uint32_t get_id()
{
    return (uint32_t)FormaldehydeConcentrationMeasurement::Feature::kPeakMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_peak_measured_value(cluster, config->peak_measured_value);
    attribute::create_peak_measured_value_window(cluster, config->peak_measured_value_window);

    return ESP_OK;
}

} /* peak_measurement */

namespace average_measurement {

uint32_t get_id()
{
    return (uint32_t)FormaldehydeConcentrationMeasurement::Feature::kAverageMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_average_measured_value(cluster, config->average_measured_value);
    attribute::create_average_measured_value_window(cluster, config->average_measured_value_window);

    return ESP_OK;
}

} /* average_measurement */

} /* feature */
} /* formaldehyde_concentration_measurement */

namespace pm1_concentration_measurement {
namespace feature {

namespace numeric_measurement {

uint32_t get_id()
{
    return (uint32_t)Pm1ConcentrationMeasurement::Feature::kNumericMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_measured_value(cluster, config->measured_value);
    attribute::create_min_measured_value(cluster, config->min_measured_value);
    attribute::create_max_measured_value(cluster, config->max_measured_value);
    attribute::create_measurement_unit(cluster, config->measurement_unit);

    return ESP_OK;
}

} /* numeric_measurement */

namespace level_indication {

uint32_t get_id()
{
    return (uint32_t)Pm1ConcentrationMeasurement::Feature::kLevelIndication;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_level_value(cluster, config->level_value);

    return ESP_OK;
}

} /* level_indication */

namespace medium_level {

uint32_t get_id()
{
    return (uint32_t)Pm1ConcentrationMeasurement::Feature::kMediumLevel;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* medium_level */

namespace critical_level {

uint32_t get_id()
{
    return (uint32_t)Pm1ConcentrationMeasurement::Feature::kCriticalLevel;

}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* critical_level */

namespace peak_measurement {

uint32_t get_id()
{
    return (uint32_t)Pm1ConcentrationMeasurement::Feature::kPeakMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_peak_measured_value(cluster, config->peak_measured_value);
    attribute::create_peak_measured_value_window(cluster, config->peak_measured_value_window);

    return ESP_OK;
}

} /* peak_measurement */

namespace average_measurement {

uint32_t get_id()
{
    return (uint32_t)Pm1ConcentrationMeasurement::Feature::kAverageMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_average_measured_value(cluster, config->average_measured_value);
    attribute::create_average_measured_value_window(cluster, config->average_measured_value_window);

    return ESP_OK;
}

} /* average_measurement */

} /* feature */
} /* pm1_concentration_measurement */

namespace pm25_concentration_measurement {
namespace feature {

namespace numeric_measurement {

uint32_t get_id()
{
    return (uint32_t)Pm25ConcentrationMeasurement::Feature::kNumericMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_measured_value(cluster, config->measured_value);
    attribute::create_min_measured_value(cluster, config->min_measured_value);
    attribute::create_max_measured_value(cluster, config->max_measured_value);
    attribute::create_measurement_unit(cluster, config->measurement_unit);

    return ESP_OK;
}

} /* numeric_measurement */

namespace level_indication {

uint32_t get_id()
{
    return (uint32_t)Pm25ConcentrationMeasurement::Feature::kLevelIndication;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_level_value(cluster, config->level_value);

    return ESP_OK;
}

} /* level_indication */

namespace medium_level {

uint32_t get_id()
{
    return (uint32_t)Pm25ConcentrationMeasurement::Feature::kMediumLevel;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* medium_level */

namespace critical_level {

uint32_t get_id()
{
    return (uint32_t)Pm25ConcentrationMeasurement::Feature::kCriticalLevel;

}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* critical_level */

namespace peak_measurement {

uint32_t get_id()
{
    return (uint32_t)Pm25ConcentrationMeasurement::Feature::kPeakMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_peak_measured_value(cluster, config->peak_measured_value);
    attribute::create_peak_measured_value_window(cluster, config->peak_measured_value_window);

    return ESP_OK;
}

} /* peak_measurement */

namespace average_measurement {

uint32_t get_id()
{
    return (uint32_t)Pm25ConcentrationMeasurement::Feature::kAverageMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_average_measured_value(cluster, config->average_measured_value);
    attribute::create_average_measured_value_window(cluster, config->average_measured_value_window);

    return ESP_OK;
}

} /* average_measurement */

} /* feature */
} /* pm25_concentration_measurement */

namespace pm10_concentration_measurement {
namespace feature {

namespace numeric_measurement {

uint32_t get_id()
{
    return (uint32_t)Pm10ConcentrationMeasurement::Feature::kNumericMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_measured_value(cluster, config->measured_value);
    attribute::create_min_measured_value(cluster, config->min_measured_value);
    attribute::create_max_measured_value(cluster, config->max_measured_value);
    attribute::create_measurement_unit(cluster, config->measurement_unit);

    return ESP_OK;
}

} /* numeric_measurement */

namespace level_indication {

uint32_t get_id()
{
    return (uint32_t)Pm10ConcentrationMeasurement::Feature::kLevelIndication;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_level_value(cluster, config->level_value);

    return ESP_OK;
}

} /* level_indication */

namespace medium_level {

uint32_t get_id()
{
    return (uint32_t)Pm10ConcentrationMeasurement::Feature::kMediumLevel;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* medium_level */

namespace critical_level {

uint32_t get_id()
{
    return (uint32_t)Pm10ConcentrationMeasurement::Feature::kCriticalLevel;

}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* critical_level */

namespace peak_measurement {

uint32_t get_id()
{
    return (uint32_t)Pm10ConcentrationMeasurement::Feature::kPeakMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_peak_measured_value(cluster, config->peak_measured_value);
    attribute::create_peak_measured_value_window(cluster, config->peak_measured_value_window);

    return ESP_OK;
}

} /* peak_measurement */

namespace average_measurement {

uint32_t get_id()
{
    return (uint32_t)Pm10ConcentrationMeasurement::Feature::kAverageMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_average_measured_value(cluster, config->average_measured_value);
    attribute::create_average_measured_value_window(cluster, config->average_measured_value_window);

    return ESP_OK;
}

} /* average_measurement */

} /* feature */
} /* pm10_concentration_measurement */

namespace radon_concentration_measurement {
namespace feature {

namespace numeric_measurement {

uint32_t get_id()
{
    return (uint32_t)RadonConcentrationMeasurement::Feature::kNumericMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_measured_value(cluster, config->measured_value);
    attribute::create_min_measured_value(cluster, config->min_measured_value);
    attribute::create_max_measured_value(cluster, config->max_measured_value);
    attribute::create_measurement_unit(cluster, config->measurement_unit);

    return ESP_OK;
}

} /* numeric_measurement */

namespace level_indication {

uint32_t get_id()
{
    return (uint32_t)RadonConcentrationMeasurement::Feature::kLevelIndication;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_level_value(cluster, config->level_value);

    return ESP_OK;
}

} /* level_indication */

namespace medium_level {

uint32_t get_id()
{
    return (uint32_t)RadonConcentrationMeasurement::Feature::kMediumLevel;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* medium_level */

namespace critical_level {

uint32_t get_id()
{
    return (uint32_t)RadonConcentrationMeasurement::Feature::kCriticalLevel;

}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* critical_level */

namespace peak_measurement {

uint32_t get_id()
{
    return (uint32_t)RadonConcentrationMeasurement::Feature::kPeakMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_peak_measured_value(cluster, config->peak_measured_value);
    attribute::create_peak_measured_value_window(cluster, config->peak_measured_value_window);

    return ESP_OK;
}

} /* peak_measurement */

namespace average_measurement {

uint32_t get_id()
{
    return (uint32_t)RadonConcentrationMeasurement::Feature::kAverageMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_average_measured_value(cluster, config->average_measured_value);
    attribute::create_average_measured_value_window(cluster, config->average_measured_value_window);

    return ESP_OK;
}

} /* average_measurement */

} /* feature */
} /* radon_concentration_measurement */

namespace total_volatile_organic_compounds_concentration_measurement {
namespace feature {

namespace numeric_measurement {

uint32_t get_id()
{
    return (uint32_t)TotalVolatileOrganicCompoundsConcentrationMeasurement::Feature::kNumericMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_measured_value(cluster, config->measured_value);
    attribute::create_min_measured_value(cluster, config->min_measured_value);
    attribute::create_max_measured_value(cluster, config->max_measured_value);
    attribute::create_measurement_unit(cluster, config->measurement_unit);

    return ESP_OK;
}

} /* numeric_measurement */

namespace level_indication {

uint32_t get_id()
{
    return (uint32_t)TotalVolatileOrganicCompoundsConcentrationMeasurement::Feature::kLevelIndication;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_level_value(cluster, config->level_value);

    return ESP_OK;
}

} /* level_indication */

namespace medium_level {

uint32_t get_id()
{
    return (uint32_t)TotalVolatileOrganicCompoundsConcentrationMeasurement::Feature::kMediumLevel;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* medium_level */

namespace critical_level {

uint32_t get_id()
{
    return (uint32_t)TotalVolatileOrganicCompoundsConcentrationMeasurement::Feature::kCriticalLevel;

}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* critical_level */

namespace peak_measurement {

uint32_t get_id()
{
    return (uint32_t)TotalVolatileOrganicCompoundsConcentrationMeasurement::Feature::kPeakMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_peak_measured_value(cluster, config->peak_measured_value);
    attribute::create_peak_measured_value_window(cluster, config->peak_measured_value_window);

    return ESP_OK;
}

} /* peak_measurement */

namespace average_measurement {

uint32_t get_id()
{
    return (uint32_t)TotalVolatileOrganicCompoundsConcentrationMeasurement::Feature::kAverageMeasurement;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_average_measured_value(cluster, config->average_measured_value);
    attribute::create_average_measured_value_window(cluster, config->average_measured_value_window);

    return ESP_OK;
}

} /* average_measurement */

} /* feature */
} /* total_volatile_organic_compounds_concentration_measurement */

namespace hepa_filter_monitoring {
namespace feature {

namespace condition {

uint32_t get_id()
{
    return (uint32_t)HepaFilterMonitoring::Feature::kCondition;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_condition(cluster, config->condition);
    attribute::create_degradation_direction(cluster, config->degradation_direction);

    return ESP_OK;
}

} /* condition */

namespace warning {

uint32_t get_id()
{
    return (uint32_t)HepaFilterMonitoring::Feature::kWarning;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* warning */

namespace replacement_product_list {

uint32_t get_id()
{
    return (uint32_t)HepaFilterMonitoring::Feature::kReplacementProductList;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_replacement_product_list(cluster, NULL, 0, 0);

    return ESP_OK;
}

} /* replacement_product_list */

} /* feature */
} /* hepa_filter_monitoring */

namespace activated_carbon_filter_monitoring {
namespace feature {

namespace condition {

uint32_t get_id()
{
    return (uint32_t)ActivatedCarbonFilterMonitoring::Feature::kCondition;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_condition(cluster, config->condition);
    attribute::create_degradation_direction(cluster, config->degradation_direction);

    return ESP_OK;
}

} /* condition */

namespace warning {

uint32_t get_id()
{
    return (uint32_t)ActivatedCarbonFilterMonitoring::Feature::kWarning;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* warning */

namespace replacement_product_list {

uint32_t get_id()
{
    return (uint32_t)ActivatedCarbonFilterMonitoring::Feature::kReplacementProductList;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_replacement_product_list(cluster, NULL, 0, 0);

    return ESP_OK;
}

} /* replacement_product_list */

} /* feature */
} /* activated_carbon_filter_monitoring */

namespace laundry_washer_controls {
namespace feature {

namespace spin {

uint32_t get_id()
{
    return (uint32_t)LaundryWasherControls::Feature::kSpin;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));

    update_feature_map(cluster, get_id());

    attribute::create_spin_speed_current(cluster, config->spin_speed_current);

    return ESP_OK;
}

} /* spin */

namespace rinse {

uint32_t get_id()
{
    return (uint32_t)LaundryWasherControls::Feature::kRinse;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));

    update_feature_map(cluster, get_id());

    attribute::create_number_of_rinses(cluster, config->number_of_rinses);

    return ESP_OK;
}

} /* rinse */

} /* feature */
} /* laundry_washer_controls */

namespace smoke_co_alarm {
namespace feature {

namespace smoke_alarm {

uint32_t get_id()
{
    return (uint32_t)SmokeCoAlarm::Feature::kSmokeAlarm;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_smoke_state(cluster, 0);
    attribute::create_contamination_state(cluster, 0);
    attribute::create_smoke_sensitivity_level(cluster, 0);

    event::create_smoke_alarm(cluster);
    event::create_interconnect_smoke_alarm(cluster);

    return ESP_OK;
}

} /* smoke_alarm */

namespace co_alarm {

uint32_t get_id()
{
    return (uint32_t)SmokeCoAlarm::Feature::kCoAlarm;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_co_state(cluster, 0);

    event::create_co_alarm(cluster);
    event::create_interconnect_co_alarm(cluster);

    return ESP_OK;
}

} /* co_alarm */

} /* feature */
} /* smoke_co_alarm */

namespace thermostat {
namespace feature {

namespace heating {

uint32_t get_id()
{
    return (uint32_t)Thermostat::Feature::kHeating;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_occupied_heating_setpoint(cluster, config->occupied_heating_setpoint);

    return ESP_OK;
}

} /* heating */

namespace cooling {

uint32_t get_id()
{
    return (uint32_t)Thermostat::Feature::kCooling;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_occupied_cooling_setpoint(cluster, config->occupied_cooling_setpoint);

    return ESP_OK;
}
} /* cooling */

namespace occupancy {

uint32_t get_id()
{
    return (uint32_t)Thermostat::Feature::kOccupancy;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_occupancy(cluster, config->occupancy);

    uint32_t occ_and_cool_feature_map = get_id() | feature::cooling::get_id();
    uint32_t occ_and_heat_feature_map = get_id() | feature::heating::get_id();
    uint32_t occ_and_sb_feature_map = get_id() | feature::setback::get_id();

    if ((get_feature_map_value(cluster) & occ_and_cool_feature_map) == occ_and_cool_feature_map) {
        attribute::create_unoccupied_cooling_setpoint(cluster, config->unoccupied_cooling_setpoint);
    }

    if ((get_feature_map_value(cluster) & occ_and_heat_feature_map) == occ_and_heat_feature_map) {
        attribute::create_unoccupied_heating_setpoint(cluster, config->unoccupied_heating_setpoint);
    }

    if ((get_feature_map_value(cluster) & occ_and_sb_feature_map) == occ_and_sb_feature_map) {

        attribute::create_unoccupied_setback(cluster, config->unoccupied_setback);
        attribute::create_unoccupied_setback_min(cluster, config->unoccupied_setback_min);
        attribute::create_unoccupied_setback_max(cluster, config->unoccupied_setback_max);
    }

    return ESP_OK;
}
} /* occupancy */

namespace schedule_configuration {

uint32_t get_id()
{
    return (uint32_t)Thermostat::Feature::kScheduleConfiguration;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_start_of_week(cluster, config->start_of_week);
    attribute::create_number_of_weekly_transitions(cluster, config->number_of_weekly_transitions);
    attribute::create_number_of_daily_transitions(cluster, config->number_of_daily_transitions);

    command::create_set_weekly_schedule(cluster);
    command::create_get_weekly_schedule(cluster);
    command::create_clear_weekly_schedule(cluster);
    command::create_get_weekly_schedule_response(cluster);

    return ESP_OK;
}
} /* schedule_configuration */

namespace setback {

uint32_t get_id()
{
    return (uint32_t)Thermostat::Feature::kSetback;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_occupied_setback(cluster, config->occupied_setback);
    attribute::create_occupied_setback_min(cluster, config->occupied_setback_min);
    attribute::create_occupied_setback_max(cluster, config->occupied_setback_max);

    return ESP_OK;
}
} /* setback */

namespace auto_mode {

uint32_t get_id()
{
    return (uint32_t)Thermostat::Feature::kAutoMode;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_min_setpoint_dead_band(cluster, config->min_setpoint_dead_band);

    return ESP_OK;
}
} /* auto_mode */

namespace local_temperature_not_exposed {

uint32_t get_id()
{
    return (uint32_t)Thermostat::Feature::kLocalTemperatureNotExposed;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* local_temperature_not_exposed */

namespace matter_schedule_configuration {

uint32_t get_id()
{
    return (uint32_t)Thermostat::Feature::kMatterScheduleConfiguration;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    attribute::create_schedule_type(cluster, NULL, 0, 0);
    attribute::create_number_of_schedules(cluster, config->number_of_schedules);
    attribute::create_number_of_schedule_transitions(cluster, config->number_of_schedule_transitions);
    attribute::create_number_of_schedule_transition_per_day(cluster, config->number_of_schedule_transition_per_day);
    attribute::create_active_schedule_handle(cluster, config->active_schedule_handle, sizeof(config->active_schedule_handle));
    attribute::create_schedules(cluster, NULL, 0, 0);

    command::create_set_active_schedule_request(cluster);
    return ESP_OK;
}
} /* matter_schedule_configuration */

namespace presets {

uint32_t get_id()
{
    return (uint32_t)Thermostat::Feature::kPresets;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    /* Attributes managed internally */
    attribute::create_preset_type(cluster, NULL, 0, 0);
    attribute::create_number_of_presets(cluster, 0);
    attribute::create_active_preset_handle(cluster, NULL, 0);
    attribute::create_presets(cluster, NULL, 0, 0);

    command::create_set_active_preset_request(cluster);
    return ESP_OK;
}
} /* presets */

} /* feature */
} /* thermostat */

namespace switch_cluster {
namespace feature {
namespace latching_switch {

uint32_t get_id()
{
    return (uint32_t)Switch::Feature::kLatchingSwitch;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError((get_feature_map_value(cluster) & feature::momentary_switch::get_id()) != feature::momentary_switch::get_id(), ESP_ERR_NOT_SUPPORTED, ESP_LOGE(TAG, "Latching switch is not supported because momentary switch is present"));
    update_feature_map(cluster, get_id());

    event::create_switch_latched(cluster);
    return ESP_OK;
}

} /* latching_switch */

namespace momentary_switch {

uint32_t get_id()
{
    return (uint32_t)Switch::Feature::kMomentarySwitch;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError((get_feature_map_value(cluster) & feature::latching_switch::get_id()) != feature::latching_switch::get_id(), ESP_ERR_NOT_SUPPORTED, ESP_LOGE(TAG, "Momentary switch is not supported because latching switch is present"));
    update_feature_map(cluster, get_id());

    event::create_initial_press(cluster);
    return ESP_OK;
}

} /* momentary_switch */

namespace momentary_switch_release {

uint32_t get_id()
{
    return (uint32_t)Switch::Feature::kMomentarySwitchRelease;
}

esp_err_t add(cluster_t *cluster)
{
    uint32_t as_feature_map = feature::action_switch::get_id();
    uint32_t ms_feature_map = feature::momentary_switch::get_id();
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(((feature_map & ms_feature_map) == ms_feature_map) && ((feature_map & as_feature_map) != as_feature_map), ESP_ERR_NOT_SUPPORTED, ESP_LOGE(TAG, "Momentary switch release is not supported."));
    update_feature_map(cluster, get_id());

    event::create_short_release(cluster);
    return ESP_OK;
}

} /* momentary_switch_release */

namespace momentary_switch_long_press {

uint32_t get_id()
{
    return (uint32_t)Switch::Feature::kMomentarySwitchLongPress;
}

esp_err_t add(cluster_t *cluster)
{
    uint32_t msr_feature_map = feature::momentary_switch_release::get_id();
    uint32_t as_feature_map = feature::action_switch::get_id();
    uint32_t ms_feature_map = feature::momentary_switch::get_id();
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError((feature_map & ms_feature_map) == ms_feature_map, ESP_ERR_NOT_SUPPORTED, ESP_LOGE(TAG, "Momentary switch long press is not supported."));
    VerifyOrReturnError(((feature_map & msr_feature_map) == msr_feature_map) || ((feature_map & as_feature_map) == as_feature_map), ESP_ERR_NOT_SUPPORTED, ESP_LOGE(TAG, "Momentary switch long press is not supported."));
    update_feature_map(cluster, get_id());

    event::create_long_press(cluster);
    event::create_long_release(cluster);
    return ESP_OK;
}

} /* momentary_switch_long_press */

namespace momentary_switch_multi_press {

uint32_t get_id()
{
    return (uint32_t)Switch::Feature::kMomentarySwitchMultiPress;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    uint32_t as_feature_map = feature::action_switch::get_id();
    uint32_t ms_feature_map = feature::momentary_switch::get_id();
    uint32_t msr_feature_map = feature::momentary_switch_release::get_id();
    uint32_t ms_and_msr = ms_feature_map | msr_feature_map;
    if ((get_feature_map_value(cluster) & as_feature_map) != as_feature_map) {
        if ((get_feature_map_value(cluster) & ms_and_msr) != ms_and_msr) {
            ESP_LOGE(TAG, "Momentary switch multi press is not supported.");
            return ESP_ERR_NOT_SUPPORTED;
        }
        event::create_multi_press_ongoing(cluster);
    }
    update_feature_map(cluster, get_id());

    attribute::create_multi_press_max(cluster, config->multi_press_max);

    event::create_multi_press_complete(cluster);
    return ESP_OK;
}

} /* momentary_switch_multi_press */

namespace action_switch {

uint32_t get_id()
{
    return (uint32_t)Switch::Feature::kActionSwitch;
}

esp_err_t add(cluster_t *cluster)
{
    if ((get_feature_map_value(cluster) & feature::momentary_switch::get_id()) != feature::momentary_switch::get_id()) {
        ESP_LOGE(TAG, "Momentary switch is present");
        return ESP_ERR_NOT_SUPPORTED;
    }
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* action_switch */

} /* feature */
} /* switch_cluster */

namespace unit_localization {
namespace feature {

namespace temperature_unit {

uint32_t get_id()
{
    return (uint32_t)UnitLocalization::Feature::kTemperatureUnit;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_temperature_unit(cluster, config->temperature_unit);

    return ESP_OK;
}

} /* temperature_unit */

} /* feature */
} /* unit_localization */

namespace time_format_localization {
namespace feature {

namespace calendar_format {

uint32_t get_id()
{
    // enum class for CalendarFormat is not present in the upstream code.
    // Return the code according to the SPEC
    return 0x01;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_active_calendar_type(cluster, config->active_calendar_type);

    /* Attributes managed internally */
    attribute::create_supported_calendar_types(cluster, NULL, 0, 0);

    return ESP_OK;
}

} /* calendar_format */

} /* feature */
} /* time_format_localization */

namespace mode_select {
namespace feature {

namespace on_off {

uint32_t get_id()
{
    return (uint32_t)ModeSelect::Feature::kOnOff;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_on_mode(cluster, config->on_mode);

    return ESP_OK;
}
} /* on_off */

} /* feature */
} /* mode_select */

namespace software_diagnostics {
namespace feature {

namespace watermarks {

uint32_t get_id()
{
    // enum class for SoftwareDiagnosticsFeature is not present in the upstream code.
    // Return the code according to the SPEC
    return 0x01;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes managed internally */
    attribute::create_current_heap_high_watermark(cluster, 0);

    /* commands */
    command::create_reset_watermarks(cluster);

    return ESP_OK;
}
} /* watermarks */

} /* feature */
} /* software_diagnostics */

namespace temperature_control {
namespace feature {
namespace temperature_number {

uint32_t get_id()
{
    return (uint32_t)TemperatureControl::Feature::kTemperatureNumber;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    uint32_t temp_level_feature_map = feature::temperature_level::get_id();
    if ((get_feature_map_value(cluster) & temp_level_feature_map) != temp_level_feature_map) {
        update_feature_map(cluster, get_id());

        /* Attributes not managed internally */
        attribute::create_temperature_setpoint(cluster, config->temp_setpoint);
        attribute::create_min_temperature(cluster, config->min_temperature);
        attribute::create_max_temperature(cluster, config->max_temperature);
    } else {
        ESP_LOGE(TAG, "Cluster shall support either TemperatureNumber or TemperatureLevel feature");
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}
} /* temperature_number */

namespace temperature_level {

uint32_t get_id()
{
    return (uint32_t)TemperatureControl::Feature::kTemperatureLevel;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    uint32_t temp_number_feature_map = feature::temperature_number::get_id();
    if ((get_feature_map_value(cluster) & temp_number_feature_map) != temp_number_feature_map) {
        update_feature_map(cluster, get_id());

        /* Attributes managed internally */
        attribute::create_supported_temperature_levels(cluster, NULL, 0, 0);

        /* Attributes not managed internally */
        attribute::create_selected_temperature_level(cluster, config->selected_temp_level);
    } else {
        ESP_LOGE(TAG, "Cluster shall support either TemperatureLevel or TemperatureNumber feature");
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}
} /* temperature_level */

namespace temperature_step {

uint32_t get_id()
{
    return (uint32_t)TemperatureControl::Feature::kTemperatureStep;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    uint32_t temp_number_feature_map = feature::temperature_number::get_id();
    if ((get_feature_map_value(cluster) & temp_number_feature_map) == temp_number_feature_map) {
        update_feature_map(cluster, get_id());

        /* Attributes not managed internally */
        attribute::create_step(cluster, config->step);
    } else {
        ESP_LOGE(TAG, "Cluster shall support TemperatureNumber feature");
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}
} /* temperature_step */

} /* feature */
} /* temperature_control */

namespace fan_control {
namespace feature {

namespace multi_speed {

uint32_t get_id()
{
    return (uint32_t)FanControl::Feature::kMultiSpeed;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_speed_max(cluster, config->speed_max);
    attribute::create_speed_setting(cluster, config->speed_setting);
    attribute::create_speed_current(cluster, config->speed_current);

    return ESP_OK;
}
} /* multi_speed */

namespace fan_auto {

uint32_t get_id()
{
    return (uint32_t)FanControl::Feature::kAuto;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* fan_auto */

namespace rocking {

uint32_t get_id()
{
    return (uint32_t)FanControl::Feature::kRocking;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_rock_support(cluster, config->rock_support);
    attribute::create_rock_setting(cluster, config->rock_setting);

    return ESP_OK;
}
} /* rocking */

namespace wind {

uint32_t get_id()
{
    return (uint32_t)FanControl::Feature::kWind;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_wind_support(cluster, config->wind_support);
    attribute::create_wind_setting(cluster, config->wind_setting);

    return ESP_OK;
}
} /* wind */

namespace step {

uint32_t get_id()
{
    return (uint32_t)FanControl::Feature::kStep;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    command::create_step(cluster);

    return ESP_OK;
}
} /* step */

namespace airflow_direction {

uint32_t get_id()
{
    return (uint32_t)FanControl::Feature::kAirflowDirection;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_airflow_direction(cluster, config->airflow_direction);

    return ESP_OK;
}
} /* airflow_direction */

} /* feature */
} /* fan_control */

namespace keypad_input {
namespace feature {

namespace navigation_key_codes {

uint32_t get_id()
{
    return (uint32_t)KeypadInput::Feature::kNavigationKeyCodes;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* navigation_key_codes */

namespace location_keys {

uint32_t get_id()
{
    return (uint32_t)KeypadInput::Feature::kLocationKeys;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* location_keys */

namespace number_keys {

uint32_t get_id()
{
    return (uint32_t)KeypadInput::Feature::kNumberKeys;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* number_keys */

} /* feature */
} /* keypad_input */

namespace boolean_state_configuration {
namespace feature {

namespace visual {

uint32_t get_id()
{
    return (uint32_t)BooleanStateConfiguration::Feature::kVisual;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_alarms_active(cluster, config->alarms_active);
    attribute::create_alarms_supported(cluster, config->alarms_supported);

    command::create_enable_disable_alarm(cluster);;
    return ESP_OK;
}
} /* visual */

namespace audible {

uint32_t get_id()
{
    return (uint32_t)BooleanStateConfiguration::Feature::kAudible;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_alarms_active(cluster, config->alarms_active);
    attribute::create_alarms_supported(cluster, config->alarms_supported);

    command::create_enable_disable_alarm(cluster);;
    return ESP_OK;
}
} /* audible */

namespace alarm_suppress {

uint32_t get_id()
{
    return (uint32_t)BooleanStateConfiguration::Feature::kAlarmSuppress;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    uint32_t visual_feature_map = feature::visual::get_id();
    uint32_t audible_feature_map = feature::audible::get_id();
    if ((get_feature_map_value(cluster) & visual_feature_map) == visual_feature_map ||
            (get_feature_map_value(cluster) & audible_feature_map) == audible_feature_map) {
        update_feature_map(cluster, get_id());

        attribute::create_alarms_suppressed(cluster, config->alarms_suppressed);

        command::create_suppress_alarm(cluster);
    } else {
        ESP_LOGE(TAG, "Cluster shall support either visual or audio feature");
        return ESP_ERR_NOT_SUPPORTED;
    }
    return ESP_OK;
}
} /* alarm_suppress */

namespace sensitivity_level {

uint32_t get_id()
{
    return (uint32_t)BooleanStateConfiguration::Feature::kSensitivityLevel;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_current_sensitivity_level(cluster, 0);
    attribute::create_supported_sensitivity_levels(cluster, config->supported_sensitivity_levels);
    return ESP_OK;
}
} /* sensitivity_level */

} /* feature */
} /* boolean_state_configuration */

namespace power_topology {
namespace feature {

namespace node_topology {

uint32_t get_id()
{
    return (uint32_t)PowerTopology::Feature::kNodeTopology;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* node_topology */

namespace tree_topology {

uint32_t get_id()
{
    return (uint32_t)PowerTopology::Feature::kTreeTopology;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* tree_topology */

namespace set_topology {

uint32_t get_id()
{
    return (uint32_t)PowerTopology::Feature::kSetTopology;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_available_endpoints(cluster, NULL, 0, 0);
    return ESP_OK;
}
} /* set_topology */

namespace dynamic_power_flow {

uint32_t get_id()
{
    return (uint32_t)PowerTopology::Feature::kDynamicPowerFlow;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    uint32_t set_topology_feature_map = feature::set_topology::get_id();
    if ((get_feature_map_value(cluster) & set_topology_feature_map) == set_topology_feature_map) {

        update_feature_map(cluster, get_id());

        attribute::create_active_endpoints(cluster, NULL, 0, 0);
    } else {
        ESP_LOGE(TAG, "Cluster shall support Set Topology feature");
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}
} /* dynamic_power_flow */

} /* feature */
} /* power_topology */

namespace electrical_power_measurement {
namespace feature {

namespace direct_current {

uint32_t get_id()
{
    return (uint32_t)ElectricalPowerMeasurement::Feature::kDirectCurrent;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* direct_current */

namespace alternating_current {

uint32_t get_id()
{
    return (uint32_t)ElectricalPowerMeasurement::Feature::kAlternatingCurrent;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* alternating_current */

namespace polyphase_power {

uint32_t get_id()
{
    return (uint32_t)ElectricalPowerMeasurement::Feature::kPolyphasePower;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    uint32_t ac_feature_map = feature::alternating_current::get_id();
    if ((get_feature_map_value(cluster) & ac_feature_map) == ac_feature_map) {

        update_feature_map(cluster, get_id());

    } else {
        ESP_LOGE(TAG, "Cluster shall support Alternating Current feature");
        return ESP_ERR_NOT_SUPPORTED;
    }


    return ESP_OK;
}
} /* polyphase_power */

namespace harmonics {

uint32_t get_id()
{
    return (uint32_t)ElectricalPowerMeasurement::Feature::kHarmonics;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    uint32_t ac_feature_map = feature::alternating_current::get_id();
    if ((get_feature_map_value(cluster) & ac_feature_map) == ac_feature_map) {

        update_feature_map(cluster, get_id());

        attribute::create_harmonic_currents(cluster, NULL, 0, 0);
    } else {
        ESP_LOGE(TAG, "Cluster shall support Alternating Current feature");
        return ESP_ERR_NOT_SUPPORTED;
    }


    return ESP_OK;
}
} /* harmonics */

namespace power_quality {

uint32_t get_id()
{
    return (uint32_t)ElectricalPowerMeasurement::Feature::kPowerQuality;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    uint32_t ac_feature_map = feature::alternating_current::get_id();
    if ((get_feature_map_value(cluster) & ac_feature_map) == ac_feature_map) {

        update_feature_map(cluster, get_id());

        attribute::create_harmonic_phases(cluster, NULL, 0, 0);
    } else {
        ESP_LOGE(TAG, "Cluster shall support Alternating Current feature");
        return ESP_ERR_NOT_SUPPORTED;
    }


    return ESP_OK;
}
} /* power_quality */

} /* feature */
} /* electrical_power_measurement */

namespace electrical_energy_measurement {
namespace feature {

namespace imported_energy {

uint32_t get_id()
{
    return (uint32_t)ElectricalEnergyMeasurement::Feature::kImportedEnergy;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* imported_energy */

namespace exported_energy {

uint32_t get_id()
{
    return (uint32_t)ElectricalEnergyMeasurement::Feature::kExportedEnergy;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* exported_energy */

namespace cumulative_energy {

uint32_t get_id()
{
    return (uint32_t)ElectricalEnergyMeasurement::Feature::kCumulativeEnergy;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());
    uint32_t imported_feature_map = feature::imported_energy::get_id();
    uint32_t exported_feature_map = feature::exported_energy::get_id();
    if ((get_feature_map_value(cluster) & imported_feature_map) == imported_feature_map) {
        attribute::create_cumulative_energy_imported(cluster, NULL, 0, 0);
    }
    if ((get_feature_map_value(cluster) & exported_feature_map) == exported_feature_map) {
        attribute::create_cumulative_energy_exported(cluster, NULL, 0, 0);
    }

    event::create_cumulative_energy_measured(cluster);
    return ESP_OK;
}
} /* cumulative_energy */

namespace periodic_energy {

uint32_t get_id()
{
    return (uint32_t)ElectricalEnergyMeasurement::Feature::kPeriodicEnergy;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());
    uint32_t imported_feature_map = feature::imported_energy::get_id();
    uint32_t exported_feature_map = feature::exported_energy::get_id();
    if ((get_feature_map_value(cluster) & imported_feature_map) == imported_feature_map) {
        attribute::create_periodic_energy_imported(cluster, NULL, 0, 0);
    }
    if ((get_feature_map_value(cluster) & exported_feature_map) == exported_feature_map) {
        attribute::create_periodic_energy_exported(cluster, NULL, 0, 0);
    }

    event::create_periodic_energy_measured(cluster);
    return ESP_OK;
}
} /* periodic_energy */

} /* feature */
} /* electrical_energy_measurement */

namespace door_lock {
namespace feature {

namespace pin_credential {

uint32_t get_id()
{
    return (uint32_t)DoorLock::Feature::kPinCredential;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_number_of_pin_users_supported(cluster, config->number_pin_users_supported);
    attribute::create_max_pin_code_length(cluster, config->max_pin_code_length);
    attribute::create_min_pin_code_length(cluster, config->min_pin_code_length);
    attribute::create_wrong_code_entry_limit(cluster, config->wrong_code_entry_limit);
    attribute::create_user_code_temporary_disable_time(cluster, config->user_code_temporary_disable_time);

    uint32_t cota_feature_map = feature::credential_over_the_air_access::get_id();
    if (get_feature_map_value(cluster) & cota_feature_map) {
        attribute::create_require_pin_for_remote_operation(cluster, config->require_pin_for_remote_operation);
    }

    uint32_t usr_feature_map = feature::user::get_id();
    if (!(get_feature_map_value(cluster) & usr_feature_map)) {
        /* todo: some commands for !USR & PIN feature not define in the
        connectedhomeip/connectedhomeip/zzz_generated/app-common/app-common/zap-generated/ids/Commands.h
        will update when added*/
    }

    return ESP_OK;
}
} /* pin_credential */

namespace rfid_credential {

uint32_t get_id()
{
    return (uint32_t)DoorLock::Feature::kRfidCredential;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_number_of_rfid_users_supported(cluster, config->number_rfid_users_supported);
    attribute::create_max_rfid_code_length(cluster, config->max_rfid_code_length);
    attribute::create_min_rfid_code_length(cluster, config->min_rfid_code_length);
    attribute::create_wrong_code_entry_limit(cluster, config->wrong_code_entry_limit);
    attribute::create_user_code_temporary_disable_time(cluster, config->user_code_temporary_disable_time);

    uint32_t usr_feature_map = feature::user::get_id();
    if (!(get_feature_map_value(cluster) & usr_feature_map)) {
        /* todo: some commands for !USR & RID feature not define in the
        connectedhomeip/connectedhomeip/zzz_generated/app-common/app-common/zap-generated/ids/Commands.h
        will update when added*/
    }

    return ESP_OK;
}

} /* rfid_credential */

namespace finger_credentials {

uint32_t get_id()
{
    return (uint32_t)DoorLock::Feature::kFingerCredentials;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    uint32_t usr_feature_map = feature::user::get_id();
    if (!(get_feature_map_value(cluster) & usr_feature_map)) {
        /* todo: some commands for !USR & FGP feature not define in the
        connectedhomeip/connectedhomeip/zzz_generated/app-common/app-common/zap-generated/ids/Commands.h
        will update when added*/
    }

    return ESP_OK;
}

} /* finger_credentials */

namespace logging {

uint32_t get_id()
{
    return (uint32_t)DoorLock::Feature::kLogging;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* todo: all attributes for LOG feature not define in the
    connectedhomeip/connectedhomeip/zzz_generated/app-common/app-common/zap-generated/ids/Attributes.h
    will update when added*/

    /* todo: all commands for LOG feature not define in the
    connectedhomeip/connectedhomeip/zzz_generated/app-common/app-common/zap-generated/ids/Commands.h
    will update when added*/

    return ESP_OK;
}

} /* logging */

namespace weekday_access_schedules {

uint32_t get_id()
{
    return (uint32_t)DoorLock::Feature::kWeekDayAccessSchedules;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_number_of_weekday_schedules_supported_per_user(cluster, 1);
    /* Commands */
    command::create_set_weekday_schedule(cluster);
    command::create_get_weekday_schedule(cluster);
    command::create_get_weekday_schedule_response(cluster);
    command::create_clear_weekday_schedule(cluster);

    return ESP_OK;
}

} /* weekday_access_schedules */

namespace door_position_sensor {

uint32_t get_id()
{
    return (uint32_t)DoorLock::Feature::kDoorPositionSensor;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_door_state(cluster, 0);
    attribute::create_door_open_events(cluster, 0);
    attribute::create_door_close_events(cluster, 0);
    attribute::create_open_period(cluster, 0);

    /* events */
    event::create_door_state_change(cluster);

    return ESP_OK;
}

} /* door_position_sensor */

namespace face_credentials {

uint32_t get_id()
{
    return (uint32_t)DoorLock::Feature::kFaceCredentials;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* face_credentials */

namespace credential_over_the_air_access {

uint32_t get_id()
{
    return (uint32_t)DoorLock::Feature::kCredentialsOverTheAirAccess;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    uint32_t pin_credential_feature_map = feature::pin_credential::get_id();
    if (get_feature_map_value(cluster) & pin_credential_feature_map) {
        attribute::create_require_pin_for_remote_operation(cluster, config->require_pin_for_remote_operation);
    }

    return ESP_OK;
}

} /* credential_over_the_air_access */

namespace user {

uint32_t get_id()
{
    return (uint32_t)DoorLock::Feature::kUser;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    uint32_t pin = feature::pin_credential::get_id();
    uint32_t rid = feature::rfid_credential::get_id();
    uint32_t fgp = feature::finger_credentials::get_id();
    uint32_t face = feature::face_credentials::get_id();
    uint32_t feature = get_feature_map_value(cluster);
    VerifyOrReturnError((feature & (pin | rid | fgp | face)) != 0, ESP_FAIL, ESP_LOGE(TAG, "Should add at least one of PIN, RID, FGP and FACE feature before add USR feature"));

    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_number_of_total_users_supported(cluster, config->number_of_total_user_supported);
    attribute::create_credential_rules_support(cluster, config->credential_rules_supported);
    attribute::create_number_of_credentials_supported_per_user(cluster, config->number_of_credentials_supported_per_user);
    attribute::create_expiring_user_timeout(cluster, config->expiring_user_timeout);

    /* Commands */
    command::create_set_user(cluster);
    command::create_get_user(cluster);
    command::create_get_user_response(cluster);
    command::create_clear_user(cluster);
    command::create_set_credential(cluster);
    command::create_set_credential_response(cluster);
    command::create_get_credential_status(cluster);
    command::create_get_credential_status_response(cluster);
    command::create_clear_credential(cluster);

    /* Events */
    event::create_lock_user_change(cluster);

    return ESP_OK;
}

} /* user */

namespace notification {

uint32_t get_id()
{
    return (uint32_t)DoorLock::Feature::kNotification;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* notification */

namespace year_day_access_schedules {

uint32_t get_id()
{
    return (uint32_t)DoorLock::Feature::kYearDayAccessSchedules;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_number_of_year_day_schedules_supported_per_user(cluster, 1);

    /* Commands */
    command::create_set_year_day_schedule(cluster);
    command::create_get_year_day_schedule(cluster);
    command::create_get_year_day_schedule_response(cluster);
    command::create_clear_year_day_schedule(cluster);

    return ESP_OK;
}

} /* year_day_access_schedules */

namespace holiday_schedules {

uint32_t get_id()
{
    return (uint32_t)DoorLock::Feature::kHolidaySchedules;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_number_of_holiday_schedules_supported(cluster, 1);

    /* Commands */
    command::create_set_holiday_schedule(cluster);
    command::create_get_holiday_schedule(cluster);
    command::create_get_holiday_schedule_response(cluster);
    command::create_clear_holiday_schedule(cluster);

    return ESP_OK;
}

} /* holiday_schedules */

namespace unbolting {

uint32_t get_id()
{
    return (uint32_t)DoorLock::Feature::kUnbolt;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Commands */
    command::create_unbolt_door(cluster);

    return ESP_OK;
}

} /* unbolting */

namespace aliro_provisioning {

uint32_t get_id()
{
    return (uint32_t)DoorLock::Feature::kAliroProvisioning;
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    /* Attributes managed internally */
    attribute::create_aliro_reader_verification_key(cluster, NULL, 0);
    attribute::create_aliro_reader_group_identifier(cluster, NULL, 0);
    attribute::create_aliro_reader_group_sub_identifier(cluster, NULL, 0);
    attribute::create_aliro_expedited_transaction_supported_protocol_versions(cluster, NULL, 0, 0);
    attribute::create_number_of_aliro_credential_issuer_keys_supported(cluster, 0);
    attribute::create_number_of_aliro_endpoint_keys_supported(cluster, 0);

    /* Commands */
    command::create_set_aliro_reader_config(cluster);
    command::create_clear_aliro_reader_config(cluster);
    return ESP_OK;
}

} /* aliro_provisioning */

namespace aliro_bleuwb {

uint32_t get_id()
{
    return (uint32_t)DoorLock::Feature::kAliroBLEUWB;
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    /* Attributes managed internally */
    attribute::create_aliro_group_resolving_key(cluster, NULL, 0);
    attribute::create_aliro_supported_bleuwb_protocol_versions(cluster, NULL, 0, 0);
    attribute::create_aliro_ble_advertising_version(cluster, 0);
    return ESP_OK;
}

} /* aliro_bleuwb */

} /* feature */
} /* door_lock */

namespace energy_evse {
namespace feature {
namespace charging_preferences {

uint32_t get_id()
{
    return (uint32_t)EnergyEvse::Feature::kChargingPreferences;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes managed internally */
    attribute::create_next_charge_start_time(cluster, 0);
    attribute::create_next_charge_target_time(cluster, 0);
    attribute::create_next_charge_required_energy(cluster, 0);
    attribute::create_next_charge_target_soc(cluster, 0);

    /* Commands */
    command::create_set_targets(cluster);
    command::create_get_targets(cluster);
    command::create_clear_targets(cluster);
    command::create_get_targets_response(cluster);
    return ESP_OK;
}

} /* charging_preferences */

namespace soc_reporting {

uint32_t get_id()
{
    return (uint32_t)EnergyEvse::Feature::kSoCReporting;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes managed internally */
    attribute::create_state_of_charge(cluster, 0);
    attribute::create_battery_capacity(cluster, 0);

    return ESP_OK;
}

} /* soc_reporting */

namespace plug_and_charge {

uint32_t get_id()
{
    return (uint32_t)EnergyEvse::Feature::kPlugAndCharge;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes managed internally */
    attribute::create_vehicle_id(cluster, NULL, 0);

    return ESP_OK;
}

} /* plug_and_charge */

namespace rfid {

uint32_t get_id()
{
    return (uint32_t)EnergyEvse::Feature::kRfid;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* rfid */

namespace v2x {

uint32_t get_id()
{
    return (uint32_t)EnergyEvse::Feature::kV2x;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    /* Attributes managed internally */
    attribute::create_discharging_enabled_until(cluster, 0);
    attribute::create_maximum_discharge_current(cluster, 0);
    attribute::create_session_energy_discharged(cluster, 0);

    /* Commands */
    command::create_enable_discharging(cluster);
    return ESP_OK;
}

} /* v2x*/

} /* feature */
} /* energy_evse */

namespace microwave_oven_control {
namespace feature {

namespace power_as_number {

uint32_t get_id()
{
    return (uint32_t)MicrowaveOvenControl::Feature::kPowerAsNumber;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    uint32_t power_in_watts_feature_map = feature::power_in_watts::get_id();
    if ((get_feature_map_value(cluster) & power_in_watts_feature_map) != power_in_watts_feature_map) {
        update_feature_map(cluster, get_id());
        /* Attributes managed internally */
        attribute::create_power_setting(cluster, 0);
    } else {
        ESP_LOGE(TAG, "Cluster shall support either PowerAsNumber or PowerInWatts feature");
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}
} /* power_as_number */

namespace power_in_watts {

uint32_t get_id()
{
    return (uint32_t)MicrowaveOvenControl::Feature::kPowerInWatts;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    uint32_t power_as_number_feature_map = feature::power_as_number::get_id();
    if ((get_feature_map_value(cluster) & power_as_number_feature_map) != power_as_number_feature_map) {
        update_feature_map(cluster, get_id());
        /* Attributes managed internally */
        attribute::create_supported_watts(cluster, NULL, 0, 0);
        attribute::create_selected_watt_index(cluster, 0);
    } else {
        ESP_LOGE(TAG, "Cluster shall support either PowerInWatts or PowerAsNumber feature");
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}
} /* power_in_watts */

namespace power_number_limits {

uint32_t get_id()
{
    return (uint32_t)MicrowaveOvenControl::Feature::kPowerNumberLimits;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    uint32_t power_as_number_feature_map = feature::power_as_number::get_id();
    if ((get_feature_map_value(cluster) & power_as_number_feature_map) != power_as_number_feature_map) {
        update_feature_map(cluster, get_id());
        /* Attributes managed internally */
        attribute::create_min_power(cluster, 0);
        attribute::create_max_power(cluster, 0);
        attribute::create_power_step(cluster, 0);
    } else {
        ESP_LOGE(TAG, "Cluster shall support PowerAsNumber feature.");
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}
} /* power_number_limits */

} /* feature */
} /* microwave_oven_control */

namespace valve_configuration_and_control {
namespace feature {

namespace time_sync {

uint32_t get_id()
{
    return (uint32_t)ValveConfigurationAndControl::Feature::kTimeSync;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_auto_close_time(cluster, config->auto_close_time);

    return ESP_OK;
}
} /* time_sync */

namespace level {

uint32_t get_id()
{
    return (uint32_t)ValveConfigurationAndControl::Feature::kLevel;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());

    attribute::create_current_level(cluster, config->current_level);
    attribute::create_target_level(cluster, config->target_level);

    return ESP_OK;
}
} /* level */

} /* feature */
} /* valve_configuration_and_control */

namespace device_energy_management {
namespace feature {

namespace power_adjustment {

uint32_t get_id()
{
    return (uint32_t)DeviceEnergyManagement::Feature::kPowerAdjustment;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());
    attribute::create_power_adjustment_capability(cluster, NULL, 0, 0);
    attribute::create_opt_out_state(cluster, 0);

    /* Commands */
    command::create_power_adjust_request(cluster);
    command::create_cancel_power_adjust_request(cluster);

    /* Events */
    event::create_power_adjust_start(cluster);
    event::create_power_adjust_end(cluster);


    return ESP_OK;
}
} /* power_adjustment */

namespace power_forecast_reporting {

uint32_t get_id()
{
    return (uint32_t)DeviceEnergyManagement::Feature::kPowerForecastReporting;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());
    attribute::create_forecast(cluster, NULL, 0, 0);

    return ESP_OK;
}
} /* power_forecast_reporting */

namespace state_forecast_reporting {

uint32_t get_id()
{
    return (uint32_t)DeviceEnergyManagement::Feature::kStateForecastReporting;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    uint32_t power_adjustment_id            = feature::power_adjustment::get_id();
    uint32_t power_forecast_reporting       = feature::power_forecast_reporting::get_id();
    uint32_t start_time_adjustment_id       = feature::start_time_adjustment::get_id();
    uint32_t pausable_id                    = feature::pausable::get_id();
    uint32_t forecast_adjustment_id         = feature::forecast_adjustment::get_id();
    uint32_t constraint_based_adjustment_id = feature::constraint_based_adjustment::get_id();
    uint32_t feature_map_value              = get_feature_map_value(cluster);
    if ((((feature_map_value & power_adjustment_id)              != power_adjustment_id) ||
            ((feature_map_value & start_time_adjustment_id)         == start_time_adjustment_id) ||
            ((feature_map_value & pausable_id)                      == pausable_id) ||
            ((feature_map_value & forecast_adjustment_id)           == forecast_adjustment_id) ||
            ((feature_map_value & constraint_based_adjustment_id)   == constraint_based_adjustment_id)) &&
            ((feature_map_value & power_forecast_reporting)         != power_forecast_reporting)) {
        update_feature_map(cluster, get_id());
        attribute::create_forecast(cluster, NULL, 0, 0);
    } else {
        ESP_LOGE(TAG, "Cluster shall satisfy condition (STA|PAU|FA|CON|!PA)&!PFR of feature");
        return ESP_ERR_NOT_SUPPORTED;
    }


    return ESP_OK;
}
} /* state_forecast_reporting */

namespace start_time_adjustment {

uint32_t get_id()
{
    return (uint32_t)DeviceEnergyManagement::Feature::kStartTimeAdjustment;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());
    attribute::create_opt_out_state(cluster, 0);

    /* Commands */
    command::create_start_time_adjust_request(cluster);
    command::create_cancel_request(cluster);

    return ESP_OK;
}
} /* start_time_adjustment */

namespace pausable {

uint32_t get_id()
{
    return (uint32_t)DeviceEnergyManagement::Feature::kPausable;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());
    attribute::create_opt_out_state(cluster, 0);

    /* Commands */
    command::create_pause_request(cluster);
    command::create_resume_request(cluster);

    /* Events */
    event::create_paused(cluster);
    event::create_resumed(cluster);

    return ESP_OK;
}
} /* pausable */

namespace forecast_adjustment {

uint32_t get_id()
{
    return (uint32_t)DeviceEnergyManagement::Feature::kForecastAdjustment;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());
    attribute::create_opt_out_state(cluster, 0);

    /* Commands */
    command::create_modify_forecast_request(cluster);
    command::create_cancel_request(cluster);

    return ESP_OK;
}
} /* forecast_adjustment */

namespace constraint_based_adjustment {

uint32_t get_id()
{
    return (uint32_t)DeviceEnergyManagement::Feature::kConstraintBasedAdjustment;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster cannot be NULL"));
    update_feature_map(cluster, get_id());
    attribute::create_opt_out_state(cluster, 0);

    /* Commands */
    command::create_request_constraint_based_forecast(cluster);
    command::create_cancel_request(cluster);

    return ESP_OK;
}
} /* constraint_based_adjustment */

} /* feature */
} /* device_energy_management */

namespace thread_border_router_management {
namespace feature {

namespace pan_change {

uint32_t get_id()
{
    return static_cast<uint32_t>(ThreadBorderRouterManagement::Feature::kPANChange);
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "Cluster connot be NULL"));
    update_feature_map(cluster, get_id());
    /* attribute */
    nullable<uint64_t> timestamp;
    attribute::create_pending_dataset_timestamp(cluster, timestamp);

    /* command */
    command::create_set_pending_dataset_request(cluster);
    return ESP_OK;
}

} /* pan_change */

} /* feature */
} /* thread_border_router_management */

namespace service_area {
namespace feature {

namespace select_while_running {

uint32_t get_id()
{
    return static_cast<uint32_t>(ServiceArea::Feature::kSelectWhileRunning);
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());
    return ESP_OK;
}

} /* select_while_running */

namespace progress_reporting {

uint32_t get_id()
{
    return static_cast<uint32_t>(ServiceArea::Feature::kProgressReporting);
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    attribute::create_progress(cluster, NULL, 0, 0);
    return ESP_OK;
}

} /* progress_reporting */

namespace maps {

uint32_t get_id()
{
    return static_cast<uint32_t>(ServiceArea::Feature::kMaps);
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    attribute::create_supported_maps(cluster, NULL, 0, 0);
    return ESP_OK;
}

} /* maps */

} /* feature */
} /* service_area */

namespace water_heater_management {
namespace feature {

namespace energy_management {

uint32_t get_id()
{
    return static_cast<uint32_t>(WaterHeaterManagement::Feature::kEnergyManagement);
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());
    /* attribute */
    attribute::create_tank_volume(cluster, config->tank_volume);
    attribute::create_estimated_heat_required(cluster, config->estimated_heat_required);

    return ESP_OK;
}

} /* energy_management */

namespace tank_percent {

uint32_t get_id()
{
    return static_cast<uint32_t>(WaterHeaterManagement::Feature::kEnergyManagement);
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());
    /* attribute */
    attribute::create_tank_percentage(cluster, config->tank_percentage);

    return ESP_OK;
}


} /* tank_percent */

} /* feature */
} /* water_heater_management */

namespace energy_preference {
namespace feature {

namespace energy_balance {

uint32_t get_id()
{
    return (uint32_t)EnergyPreference::Feature::kEnergyBalance;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());
    attribute::create_energy_balances(cluster, NULL, 0, 0);
    attribute::create_current_energy_balance(cluster, config->current_energy_balance);
    attribute::create_energy_priorities(cluster, NULL, 0, 0);

    return ESP_OK;
}
} /* energy_balance */

namespace low_power_mode_sensitivity {

uint32_t get_id()
{
    return (uint32_t)EnergyPreference::Feature::kLowPowerModeSensitivity;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    attribute::create_low_power_mode_sensitivities(cluster, NULL, 0, 0);
    attribute::create_current_low_power_mode_sensitivity(cluster, config->current_low_power_mode_sensitivity);

    return ESP_OK;
}
} /* low_power_mode_sensitivity */

} /* feature */
} /* energy_preference */

namespace occupancy_sensing {
namespace feature {

namespace other {

uint32_t get_id()
{
    return static_cast<uint32_t>(OccupancySensing::Feature::kOther);
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());
    return ESP_OK;
}
} /* other */

namespace passive_infrared {

uint32_t get_id()
{
    return static_cast<uint32_t>(OccupancySensing::Feature::kPassiveInfrared);
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());
    return ESP_OK;
}
} /* passive_infrared */

namespace ultrasonic {

uint32_t get_id()
{
    return static_cast<uint32_t>(OccupancySensing::Feature::kUltrasonic);
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());
    return ESP_OK;
}
} /* ultrasonic */

namespace physical_contact {

uint32_t get_id()
{
    return static_cast<uint32_t>(OccupancySensing::Feature::kPhysicalContact);
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());
    return ESP_OK;
}
} /* physical_contact */

namespace active_infrared {

uint32_t get_id()
{
    return static_cast<uint32_t>(OccupancySensing::Feature::kActiveInfrared);
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());
    return ESP_OK;
}
} /* active_infrared */

namespace radar {

uint32_t get_id()
{
    return static_cast<uint32_t>(OccupancySensing::Feature::kRadar);
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());
    return ESP_OK;
}
} /* radar */

namespace rf_sensing {

uint32_t get_id()
{
    return static_cast<uint32_t>(OccupancySensing::Feature::kRFSensing);
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* rf_sensing */

namespace vision {

uint32_t get_id()
{
    return static_cast<uint32_t>(OccupancySensing::Feature::kVision);
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());
    return ESP_OK;
}
} /* vision */

} /* feature */
} /* occupancy_sensing */

namespace pump_configuration_and_control {
namespace feature {
namespace constant_pressure {

uint32_t get_id()
{
    return (uint32_t)PumpConfigurationAndControl::Feature::kConstantPressure;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    attribute::create_min_const_pressure(cluster, config->min_const_pressure);
    attribute::create_max_const_pressure(cluster, config->max_const_pressure);

    return ESP_OK;
}
} /* constant_pressure */

namespace compensated_pressure {

uint32_t get_id()
{
    return (uint32_t)PumpConfigurationAndControl::Feature::kCompensatedPressure;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    attribute::create_min_comp_pressure(cluster, config->min_comp_pressure);
    attribute::create_max_comp_pressure(cluster, config->max_comp_pressure);

    return ESP_OK;
}
} /* compensated_pressure */

namespace constant_flow {

uint32_t get_id()
{
    return (uint32_t)PumpConfigurationAndControl::Feature::kConstantFlow;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    attribute::create_min_const_flow(cluster, config->min_const_flow);
    attribute::create_max_const_flow(cluster, config->max_const_flow);

    return ESP_OK;
}
} /* constant_flow */

namespace constant_speed {

uint32_t get_id()
{
    return (uint32_t)PumpConfigurationAndControl::Feature::kConstantSpeed;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    attribute::create_min_const_speed(cluster, config->min_const_speed);
    attribute::create_max_const_speed(cluster, config->max_const_speed);

    return ESP_OK;
}
} /* constant_speed */

namespace constant_temperature {

uint32_t get_id()
{
    return (uint32_t)PumpConfigurationAndControl::Feature::kConstantTemperature;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    attribute::create_min_const_temp(cluster, config->min_const_temp);
    attribute::create_max_const_temp(cluster, config->max_const_temp);

    return ESP_OK;
}
} /* constant_temperature */

namespace automatic {

uint32_t get_id()
{
    return static_cast<uint32_t>(PumpConfigurationAndControl::Feature::kAutomatic);
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());
    return ESP_OK;
}
} /* automatic */

namespace local_operation {

uint32_t get_id()
{
    return static_cast<uint32_t>(PumpConfigurationAndControl::Feature::kLocalOperation);
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());
    return ESP_OK;
}
} /* local_operation */

} /* feature */
} /* pump_configuration_and_control */

namespace time_synchronization {
namespace feature {

namespace time_zone {
uint32_t get_id()
{
    return static_cast<uint32_t>(TimeSynchronization::Feature::kTimeZone);
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster || !config) {
        ESP_LOGE(TAG, "Cluster or config cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    // Attributes managed internally
    attribute::create_time_zone(cluster, nullptr, 0, 0);
    attribute::create_dst_offset(cluster, nullptr, 0, 0);
    attribute::create_local_time(cluster, nullable<uint64_t>());
    attribute::create_time_zone_list_max_size(cluster, 0);
    attribute::create_dst_offset_list_max_size(cluster, 0);

    // Attributes not managed internally
    attribute::create_time_zone_database(cluster, config->time_zone_database);

    // Commands
    command::create_set_time_zone(cluster);
    command::create_set_dst_offset(cluster);
    command::create_set_time_zone_response(cluster);

    // Events
    event::create_dst_table_empty(cluster);
    event::create_dst_status(cluster);
    event::create_time_zone_status(cluster);

    return ESP_OK;
}
} /* time_zone */

namespace ntp_client {
uint32_t get_id()
{
    return static_cast<uint32_t>(TimeSynchronization::Feature::kNTPClient);
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster || !config) {
        ESP_LOGE(TAG, "Cluster or config cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    // Attributes managed internally
    attribute::create_default_ntp(cluster, nullptr, 0);

    // Attributes not managed internally
    attribute::create_supports_dns_resolve(cluster, config->supports_dns_resolve);

    // Commands
    command::create_set_default_ntp(cluster);

    return ESP_OK;
}
} /* ntp_client */

namespace ntp_server {

uint32_t get_id()
{
    return static_cast<uint32_t>(TimeSynchronization::Feature::kNTPServer);
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster || !config) {
        ESP_LOGE(TAG, "Cluster or config cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    // Attributes not managed internally
    attribute::create_ntp_server_available(cluster, config->ntp_server_available);

    return ESP_OK;
}
} /* ntp_server */

namespace time_sync_client {
uint32_t get_id()
{
    return static_cast<uint32_t>(TimeSynchronization::Feature::kTimeSyncClient);
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    // Attributes managed internally
    attribute::create_trusted_time_source(cluster, nullptr, 0, 0);

    // Commands
    command::create_set_trusted_time_source(cluster);

    return ESP_OK;
}
} /* time_sync_client */

} /* feature */
} /* time_synchronization */

} /* cluster */
} /* esp_matter */
