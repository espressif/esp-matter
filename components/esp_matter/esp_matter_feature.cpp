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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    /* Get the attribute */
    attribute_t *attribute = attribute::get(cluster, Globals::Attributes::FeatureMap::Id);

    /* Create the attribute with the new value if it does not exist */
    if (!attribute) {
        attribute = global::attribute::create_feature_map(cluster, value);
        if (!attribute) {
            ESP_LOGE(TAG, "Could not create feature map attribute");
            return ESP_FAIL;
        }
        return ESP_OK;
    }

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
    attribute_t *attribute = attribute::get(cluster, Globals::Attributes::FeatureMap::Id);

    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    /* Attributes managed internally */
    attribute::create_tag_list(cluster, NULL, 0, 0);

    return ESP_OK;
}

} /* taglist */

}
}

namespace administrator_commissioning {

namespace feature {

namespace basic {

uint32_t get_id() {
    return (uint32_t)AdministratorCommissioning::Feature::kBasic;
}

esp_err_t add(cluster_t *cluster)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    /* Attributes managed internally */
    command::create_open_basic_commissioning_window(cluster);

    return ESP_OK;
}

} /* basic */

}
}

namespace power_source {
namespace feature {
namespace wired {

uint32_t get_id()
{
    return (uint32_t)PowerSource::Feature::kWired;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t battery_feature_map = feature::battery::get_id();
    if((get_feature_map_value(cluster) & battery_feature_map) == battery_feature_map) {

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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t battery_feature_map = feature::battery::get_id();
    if((get_feature_map_value(cluster) & battery_feature_map) == battery_feature_map) {

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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    /* Attributes managed internally */
    attribute::create_registered_clients(cluster, NULL, 0, 0);
    attribute::create_icd_counter(cluster, 0);

    /* Attribute not managed internally*/
    attribute::create_clients_supported_per_fabric(cluster, config->clients_supported_per_fabric, 1);

    /* Commands */
    command::create_register_client(cluster);
    command::create_register_client_response(cluster);
    command::create_unregister_client(cluster);

    return ESP_OK;
}

} /* check_in_protocol_support */
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    /* Get the attribute */
    attribute_t *attribute = esp_matter::attribute::get(cluster, ColorControl::Attributes::ColorCapabilities::Id);

    /* Print error log if it does not exist */
    if (!attribute) {
        ESP_LOGE(TAG, "The color capability attribute is NULL");
        return ESP_FAIL;
    }

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
    return (uint32_t)ColorControl::ColorCapabilities::kHueSaturationSupported;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());
    update_color_capability(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_current_hue(cluster, config->current_hue, 0, 254);
    attribute::create_current_saturation(cluster, config->current_saturation, 0, 254);

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
    return (uint32_t)ColorControl::ColorCapabilities::kColorTemperatureSupported;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());
    update_color_capability(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_color_temperature_mireds(cluster, config->color_temperature_mireds, 0, 0xfeff);
    attribute::create_color_temp_physical_min_mireds(cluster, config->color_temp_physical_min_mireds, 0, 0xfeff);
    attribute::create_color_temp_physical_max_mireds(cluster, config->color_temp_physical_max_mireds, 0, 0xfeff);
    attribute::create_couple_color_temp_to_level_min_mireds(cluster, config->couple_color_temp_to_level_min_mireds, config->color_temp_physical_min_mireds, config->color_temperature_mireds);
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
    return (uint32_t)ColorControl::ColorCapabilities::kXYAttributesSupported;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());
    update_color_capability(cluster, get_id());

    /* Attributes not managed internally */
    attribute::create_current_x(cluster, config->current_x, 0, 0xfeff);
    attribute::create_current_y(cluster, config->current_y, 0, 0xfeff);

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
    return (uint32_t)chip::app::Clusters::ColorControl::ColorCapabilities::kEnhancedHueSupported;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t hs_feature_map = feature::hue_saturation::get_id();
    if((get_feature_map_value(cluster) & hs_feature_map) == hs_feature_map) {
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
    return (uint32_t)chip::app::Clusters::ColorControl::ColorCapabilities::kColorLoopSupported;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    uint32_t eh_feature_map = feature::enhanced_hue::get_id();
    if((get_feature_map_value(cluster) & eh_feature_map) == eh_feature_map) {
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t lift_feature_map = feature::lift::get_id();
    if((get_feature_map_value(cluster) & lift_feature_map) == lift_feature_map) {

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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if((get_feature_map_value(cluster) & abs_and_pa_lf_and_lf_feature_map) == abs_and_pa_lf_and_lf_feature_map) {
        attribute::create_physical_closed_limit_lift(cluster, config->physical_closed_limit_lift);
        attribute::create_current_position_lift(cluster, config->current_position_lift);
        attribute::create_installed_open_limit_lift(cluster, config->installed_open_limit_lift);
        attribute::create_installed_closed_limit_lift(cluster, config->installed_closed_limit_lift);
    } else {
        ESP_LOGW(TAG, "Lift related attributes were not created because cluster does not support Position_Aware_Lift feature");
    }

    if((get_feature_map_value(cluster) & abs_and_pa_tl_and_tl_feature_map) == abs_and_pa_tl_and_tl_feature_map) {
        attribute::create_physical_closed_limit_tilt(cluster, config->physical_closed_limit_tilt);
        attribute::create_current_position_tilt(cluster, config->current_position_tilt);
        attribute::create_installed_open_limit_tilt(cluster, config->installed_open_limit_tilt);
        attribute::create_installed_closed_limit_tilt(cluster, config->installed_closed_limit_tilt);
    } else {
        ESP_LOGW(TAG, "Tilt related attributes were not created because cluster does not support Position_Aware_Tilt feature");
    }

    if((get_feature_map_value(cluster) & abs_and_lift_feature_map) == abs_and_lift_feature_map) {
        command::create_go_to_lift_value(cluster);
    } else {
        ESP_LOGW(TAG, "Lift commands were not created because cluster does not support Lift feature");
    }

    if((get_feature_map_value(cluster) & abs_and_tilt_feature_map) == abs_and_tilt_feature_map) {
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t tilt_feature_map = feature::tilt::get_id();
    if((get_feature_map_value(cluster) & tilt_feature_map) == tilt_feature_map) {

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

namespace diagnostics_network_wifi {
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
} /* diagnostics_network_wifi */

namespace diagnostics_network_ethernet {
namespace feature {

namespace packets_counts {

uint32_t get_id()
{
    return (uint32_t)EthernetNetworkDiagnostics::Feature::kPacketCounts;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    /* Attributes managed internally */
    attribute::create_tx_error_count(cluster, config->tx_error_count);
    attribute::create_collision_count(cluster, config->collision_count);
    attribute::create_overrun_count(cluster, config->overrun_count);

    return ESP_OK;
}

} /* error_counts */

} /* feature */
} /* diagnostics_network_ethernet */

namespace air_quality {
namespace feature {

namespace fair {

uint32_t get_id()
{
    return (uint32_t)AirQuality::Feature::kFair;
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

} /* fair */

namespace moderate {

uint32_t get_id()
{
    return (uint32_t)AirQuality::Feature::kModerate;
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

} /* moderate */

namespace very_poor {

uint32_t get_id()
{
    return (uint32_t)AirQuality::Feature::kVeryPoor;
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

} /* very_poor */

namespace extremely_poor {

uint32_t get_id()
{
    return (uint32_t)AirQuality::Feature::kExtremelyPoor;
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

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
    // The ThermostatFeature enum class is not added in the upstream code.
    // Return the code according to the SPEC
    return 0x01;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    attribute::create_occupied_heating_setpoint(cluster, config->occupied_heating_setpoint);

    return ESP_OK;
}

} /* heating */

namespace cooling {

uint32_t get_id()
{
    // The ThermostatFeature enum class is not added in the upstream code.
    // Return the code according to the SPEC
    return 0x02;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    update_feature_map(cluster, get_id());

    attribute::create_occupied_cooling_setpoint(cluster, config->occupied_cooling_setpoint);

    return ESP_OK;
}
} /* cooling */

namespace occupancy {

uint32_t get_id()
{
    // The ThermostatFeature enum class is not added in the upstream code.
    // Return the code according to the SPEC
    return 0x04;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    attribute::create_occupancy(cluster, config->occupancy);

    uint32_t occ_and_cool_feature_map = get_id() | feature::cooling::get_id();
    uint32_t occ_and_heat_feature_map = get_id() | feature::heating::get_id();
    uint32_t occ_and_sb_feature_map = get_id() | feature::setback::get_id();

    if((get_feature_map_value(cluster) & occ_and_cool_feature_map) == occ_and_cool_feature_map) {
        attribute::create_unoccupied_cooling_setpoint(cluster, config->unoccupied_cooling_setpoint);
    }

    if((get_feature_map_value(cluster) & occ_and_heat_feature_map) == occ_and_heat_feature_map) {
        attribute::create_unoccupied_heating_setpoint(cluster, config->unoccupied_heating_setpoint);
    }

    if((get_feature_map_value(cluster) & occ_and_sb_feature_map) == occ_and_sb_feature_map) {

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
    // The ThermostatFeature enum class is not added in the upstream code.
    // Return the code according to the SPEC
    return 0x08;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    // The ThermostatFeature enum class is not added in the upstream code.
    // Return the code according to the SPEC
    return 0x10;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    // The ThermostatFeature enum class is not added in the upstream code.
    // Return the code according to the SPEC
    return 0x20;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    attribute::create_min_setpoint_dead_band(cluster, config->min_setpoint_dead_band);

    return ESP_OK;
}
} /* auto_mode */

} /* feature */
} /* thermostat */

namespace switch_cluster {
namespace feature {
namespace latching_switch {

uint32_t get_id()
{
    // The SwitchFeature enum class is not added in the upstream code.
    // Return the code according to the SPEC
    return (uint32_t)0x01;
}

esp_err_t add(cluster_t *cluster)
{
    if((get_feature_map_value(cluster) & feature::momentary_switch::get_id()) == feature::momentary_switch::get_id())
    {
        ESP_LOGE(TAG, "Latching switch is not supported because momentary switch is present");
        return ESP_ERR_NOT_SUPPORTED;
    }
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* latching_switch */

namespace momentary_switch {

uint32_t get_id()
{
    // The SwitchFeature enum class is not added in the upstream code.
    // Return the code according to the SPEC
    return (uint32_t)0x02;
}

esp_err_t add(cluster_t *cluster)
{
    if((get_feature_map_value(cluster) & feature::latching_switch::get_id()) == feature::latching_switch::get_id())
    {
        ESP_LOGE(TAG, "Momentary switch is not supported because latching switch is present");
        return ESP_ERR_NOT_SUPPORTED;
    }
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* momentary_switch */

namespace momentary_switch_release {

uint32_t get_id()
{
    // The SwitchFeature enum class is not added in the upstream code.
    // Return the code according to the SPEC
    return (uint32_t)0x04;
}

esp_err_t add(cluster_t *cluster)
{
    if((get_feature_map_value(cluster) & feature::momentary_switch::get_id()) != feature::momentary_switch::get_id())
    {
        ESP_LOGE(TAG, "Momentary switch release is not supported because momentary is absent");
        return ESP_ERR_NOT_SUPPORTED;
    }
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* momentary_switch_release */

namespace momentary_switch_long_press {

uint32_t get_id()
{
    // The SwitchFeature enum class is not added in the upstream code.
    // Return the code according to the SPEC
    return (uint32_t)0x08;
}

esp_err_t add(cluster_t *cluster)
{
    uint32_t momentary_and_momentart_switch_release_feature_map = feature::momentary_switch::get_id() | feature::momentary_switch_release::get_id();
    if((get_feature_map_value(cluster) & momentary_and_momentart_switch_release_feature_map) != momentary_and_momentart_switch_release_feature_map)
    {
        ESP_LOGE(TAG, "Momentary switch long press is not supported because momentary switch and/or momentary switch release is absent");
        return ESP_ERR_NOT_SUPPORTED;
    }
    update_feature_map(cluster, get_id());

    return ESP_OK;
}

} /* momentary_switch_long_press */

namespace momentary_switch_multi_press {

uint32_t get_id()
{
    // The SwitchFeature enum class is not added in the upstream code.
    // Return the code according to the SPEC
    return (uint32_t)0x10;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    uint32_t momentary_and_momentart_switch_release_feature_map = feature::momentary_switch::get_id() | feature::momentary_switch_release::get_id();
    if((get_feature_map_value(cluster) & momentary_and_momentart_switch_release_feature_map) != momentary_and_momentart_switch_release_feature_map)
    {
        ESP_LOGE(TAG, "Momentary switch multi press is not supported because momentary switch and/or momentary switch releaseis absent");
        return ESP_ERR_NOT_SUPPORTED;
    }
    update_feature_map(cluster, get_id());

    attribute::create_multi_press_max(cluster, config->multi_press_max);

    return ESP_OK;
}

} /* momentary_switch_multi_press */
} /* feature */
} /* switch_cluster */

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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t temp_level_feature_map = feature::temperature_level::get_id();
    if((get_feature_map_value(cluster) & temp_level_feature_map) != temp_level_feature_map) {
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t temp_number_feature_map = feature::temperature_number::get_id();
    if((get_feature_map_value(cluster) & temp_number_feature_map) != temp_number_feature_map) {
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t temp_number_feature_map = feature::temperature_number::get_id();
    if((get_feature_map_value(cluster) & temp_number_feature_map) == temp_number_feature_map) {
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    attribute::create_speed_max(cluster, config->speed_max, 0, k_max_limit);
    attribute::create_speed_setting(cluster, config->speed_setting, 0, config->speed_max);
    attribute::create_speed_current(cluster, config->speed_current, 0, config->speed_max);

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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
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
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* number_keys */

} /* feature */
} /* keypad_input */

} /* cluster */
} /* esp_matter */
