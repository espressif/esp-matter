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

namespace on_off {
namespace feature {
namespace lighting {

uint32_t get_id()
{
    return (uint32_t)OnOff::OnOffFeature::kLighting;
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
} /* feature */
} /* on_off */

namespace level_control {
namespace feature {
namespace on_off {

uint32_t get_id()
{
    return (uint32_t)LevelControl::LevelControlFeature::kOnOff;
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
    return (uint32_t)LevelControl::LevelControlFeature::kLighting;
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
    return (uint32_t)LevelControl::LevelControlFeature::kFrequency;
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
    command::create_stop_move_step(cluster);

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
    attribute::create_color_temperature_mireds(cluster, config->color_temperature_mireds);
    attribute::create_color_temp_physical_min_mireds(cluster, config->color_temp_physical_min_mireds);
    attribute::create_color_temp_physical_max_mireds(cluster, config->color_temp_physical_max_mireds);
    attribute::create_couple_color_temp_to_level_min_mireds(cluster, config->couple_color_temp_to_level_min_mireds);
    attribute::create_startup_color_temperature_mireds(cluster, config->startup_color_temperature_mireds);

    /* Commands */
    command::create_move_to_color_temperature(cluster);
    command::create_stop_move_step(cluster);
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
    attribute::create_current_x(cluster, config->current_x);
    attribute::create_current_y(cluster, config->current_y);

    /* Commands */
    command::create_move_to_color(cluster);
    command::create_stop_move_step(cluster);
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
    return (uint32_t)WindowCovering::WindowCoveringFeature::kLift;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    if (!cluster) {
        ESP_LOGE(TAG, "Cluster cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    update_feature_map(cluster, get_id());

    attribute::create_number_of_actuations_lift(cluster, config->number_of_actuations_lift);

	uint8_t set_second_bit = 1 << 2;
	esp_matter::attribute_t *attribute = esp_matter::attribute::get(cluster, WindowCovering::Attributes::ConfigStatus::Id);
	esp_matter_attr_val_t val = esp_matter_invalid(NULL);
	esp_matter::attribute::get_val(attribute, &val);
	val.val.u8 = val.val.u8 | set_second_bit;
	esp_matter::attribute::set_val(attribute, &val);
    return ESP_OK;
}

} /* lift */

namespace tilt {

uint32_t get_id()
{
    return (uint32_t)WindowCovering::WindowCoveringFeature::kTilt;
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
    return (uint32_t)WindowCovering::WindowCoveringFeature::kPositionAwareLift;
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
    return (uint32_t)WindowCovering::WindowCoveringFeature::kAbsolutePosition;
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
    return (uint32_t)WindowCovering::WindowCoveringFeature::kPositionAwareTilt;
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

    attribute::create_abs_min_heat_setpoint_limit(cluster, config->abs_min_heat_setpoint_limit);
    attribute::create_abs_max_heat_setpoint_limit(cluster, config->abs_max_heat_setpoint_limit);
    attribute::create_pi_heating_demand(cluster, config->pi_heating_demand);
    attribute::create_occupied_heating_setpoint(cluster, config->occupied_heating_setpoint);
    attribute::create_min_heat_setpoint_limit(cluster, config->min_heat_setpoint_limit);
    attribute::create_max_heat_setpoint_limit(cluster, config->max_heat_setpoint_limit);

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

    attribute::create_abs_min_cool_setpoint_limit(cluster, config->abs_min_cool_setpoint_limit);
    attribute::create_abs_max_cool_setpoint_limit(cluster, config->abs_max_cool_setpoint_limit);
    attribute::create_pi_cooling_demand(cluster, config->pi_cooling_demand);
    attribute::create_occupied_cooling_setpoint(cluster, config->occupied_cooling_setpoint);
    attribute::create_min_cool_setpoint_limit(cluster, config->min_cool_setpoint_limit);
    attribute::create_max_cool_setpoint_limit(cluster, config->max_cool_setpoint_limit);

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

    if((get_feature_map_value(cluster) & occ_and_cool_feature_map) == occ_and_cool_feature_map){
        attribute::create_unoccupied_cooling_setpoint(cluster, config->unoccupied_cooling_setpoint);
    }else{
	ESP_LOGE(TAG, "Cluster shall support Cool feature");
	return ESP_ERR_NOT_SUPPORTED;
    }
    if((get_feature_map_value(cluster) & occ_and_heat_feature_map) == occ_and_heat_feature_map){
        attribute::create_unoccupied_heating_setpoint(cluster, config->unoccupied_heating_setpoint);
    }else{
	ESP_LOGE(TAG, "Cluster shall support Heat feature");
	return ESP_ERR_NOT_SUPPORTED;
    }
    if((get_feature_map_value(cluster) & occ_and_sb_feature_map) == occ_and_sb_feature_map){
        attribute::create_unoccupied_setback(cluster, config->unoccupied_setback);
        attribute::create_unoccupied_setback_min(cluster, config->unoccupied_setback_min);
        attribute::create_unoccupied_setback_max(cluster, config->unoccupied_setback_max);
    }else{
	ESP_LOGE(TAG, "Cluster shall support Setback feature");
	return ESP_ERR_NOT_SUPPORTED;
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
    attribute::create_thermostat_running_mode(cluster, config->thermostat_running_mode);

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
    return (uint32_t)ModeSelect::ModeSelectFeature::kDeponoff;
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

} /* cluster */
} /* esp_matter */
