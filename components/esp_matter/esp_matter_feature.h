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

#pragma once

#include <stdint.h>
#include <esp_matter_attribute.h>

#define ESP_MATTER_NONE_FEATURE_ID 0x0000

/** Specific feature APIs
 *
 * These APIs also create the mandatory attributes and commands for the cluster for that particular feature. If the
 * mandatory attribute is not managed internally, then a config is present for that attribute. The constructor for the
 * config will set the attribute to the default value from the spec.
 *
 * If some standard feature is not present here, it can be added.
 */

namespace esp_matter {
namespace cluster {

namespace descriptor {
namespace feature {
namespace taglist {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* taglist */

} /* feature */
} /* descriptor */

namespace access_control {
namespace feature {
namespace extension {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* extension */

namespace managed_device {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* managed_device */

} /* feature */
} /* access_control */

namespace bridged_device_basic_information {
namespace feature {
namespace bridged_icd_support {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* bridged_icd_support */

} /* feature */
} /* bridged_device_basic_information */

namespace administrator_commissioning {

namespace feature {

namespace basic {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* basic */

} /* feature */
} /* administrator_commissioning */

namespace general_commissioning {
namespace feature {
namespace terms_and_conditions {
typedef struct config {
    uint16_t tc_accepted_version;
    uint16_t tc_min_required_version;
    uint16_t tc_acknowledgements;
    bool tc_acknowledgements_required;
    nullable<uint32_t> tc_update_deadline;
    config(): tc_accepted_version(0), tc_min_required_version(0), tc_acknowledgements(0), tc_acknowledgements_required(true), tc_update_deadline(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* terms_and_conditions */
} /* feature */
} /* general_commissioning */

namespace power_source {
namespace feature {
namespace wired {
typedef struct config {
    uint8_t wired_current_type;
    config(): wired_current_type(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* wired */

namespace battery {
typedef struct config {
    uint8_t bat_charge_level;
    bool bat_replacement_needed;
    uint8_t bat_replaceability;
    config(): bat_charge_level(0), bat_replacement_needed(false), bat_replaceability(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* battery */

// Rechargeable feature is dependent on  Battery feature, in order to add
// Rechargeable feature one must add Battery feature first.
namespace rechargeable {
typedef struct config {
    uint8_t bat_charge_state;
    bool bat_functional_while_charging;
    config(): bat_charge_state(0), bat_functional_while_charging(false) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* rechargeable */

// Replaceable feature is dependent on  Battery feature, in order to add
// Replaceable feature one must add Battery feature first.
namespace replaceable {
typedef struct config {
    char bat_replacement_description[k_max_bat_replacement_description_length + 1];
    uint8_t bat_quantity;
    config(): bat_replacement_description{0}, bat_quantity(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* replaceable */

} /* feature */
} /* power_source */

namespace scenes_management {
namespace feature {
namespace scene_names {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* scene_names */

namespace explicit_feature {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* explicit_feature */

namespace table_size {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* table_size */

namespace fabric_scenes {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* fabric_scenes*/

} /* feature */
} /* scenes_management */

namespace icd_management {
namespace feature {
namespace check_in_protocol_support {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* check_in_protocol_support */

namespace user_active_mode_trigger {
typedef struct config {
    uint32_t user_active_mode_trigger_hint;
    char user_active_mode_trigger_instruction[attribute::k_user_active_mode_trigger_instruction_length + 1];
    config() : user_active_mode_trigger_hint(0), user_active_mode_trigger_instruction{0} {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* user_active_mode_trigger */

namespace long_idle_time_support {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* long_idle_time_support */

} /* feature */
} /* icd_management */

namespace on_off {
namespace feature {
namespace lighting {

typedef struct config {
    bool global_scene_control;
    nullable<uint16_t> on_time;
    nullable<uint16_t> off_wait_time;
    nullable<uint8_t> start_up_on_off;
    config() : global_scene_control(1), on_time(0), off_wait_time(0), start_up_on_off(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* lighting */

namespace dead_front_behavior {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* dead_front_behavior */

namespace off_only {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* off_only */
} /* feature */
} /* on_off */

namespace level_control {
namespace feature {
namespace on_off {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* on_off */

namespace lighting {

typedef struct config {
    uint16_t remaining_time;
    uint8_t min_level;
    uint8_t max_level;
    nullable<uint8_t> start_up_current_level;
    config() : remaining_time(0), min_level(1), max_level(254), start_up_current_level(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* lighting */

namespace frequency {

typedef struct config {
    uint16_t current_frequency;
    uint16_t min_frequency;
    uint16_t max_frequency;
    config() : current_frequency(0), min_frequency(0), max_frequency(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* frequency */
} /* feature */
} /* level_control */

namespace color_control {
namespace feature {
namespace hue_saturation {

typedef struct config {
    uint8_t current_hue;
    uint8_t current_saturation;
    config() : current_hue(0), current_saturation(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* hue_saturation */

namespace color_temperature {

typedef struct config {
    uint16_t color_temperature_mireds;
    uint16_t color_temp_physical_min_mireds;
    uint16_t color_temp_physical_max_mireds;
    uint16_t couple_color_temp_to_level_min_mireds;
    nullable<uint16_t> startup_color_temperature_mireds;
    config() : color_temperature_mireds(0x00fa), color_temp_physical_min_mireds(0),
               color_temp_physical_max_mireds(0xfeff), couple_color_temp_to_level_min_mireds(1),
               startup_color_temperature_mireds(0x00fa) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* color_temperature */

namespace xy {

typedef struct config {
    uint16_t current_x;
    uint16_t current_y;
    config() : current_x(0x616b), current_y(0x607d) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* xy */

// EnhancedHue feature is dependent on HueSaturation feature, in order to add
// EnhancedHue feature one must add HueSaturation feature first.

namespace enhanced_hue {

typedef struct config {
    uint16_t enhanced_current_hue;
    config() : enhanced_current_hue(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* enhanced_hue */

// ColorLoop feature is dependent on EnhancedHue feature, in order to add
// ColorLoop feature one must add EnhancedHue feature first.

namespace color_loop {

typedef struct config {
    uint8_t color_loop_active;
    uint8_t color_loop_direction;
    uint16_t color_loop_time;
    uint16_t color_loop_start_enhanced_hue;
    uint16_t color_loop_stored_enhanced_hue;
    config() : color_loop_active(0), color_loop_direction(0), color_loop_time(0x19),
               color_loop_start_enhanced_hue(0x2300), color_loop_stored_enhanced_hue(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* color_loop */
} /* feature */
} /* color_control */

namespace window_covering {
namespace feature {

namespace lift {

typedef struct config {
    uint16_t number_of_actuations_lift;
    config() : number_of_actuations_lift(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* lift */

namespace tilt {

typedef struct config {
    uint16_t number_of_actuations_tilt;
    config() : number_of_actuations_tilt(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* tilt */

// PositionAwareLift feature is dependent on Lift feature, in order to add
// PositionAwareLift feature one must add Lift feature first.

namespace position_aware_lift {

typedef struct config {
    nullable<uint8_t> current_position_lift_percentage;
    nullable<uint16_t> target_position_lift_percent_100ths;
    nullable<uint16_t> current_position_lift_percent_100ths;
    config() : current_position_lift_percentage(), target_position_lift_percent_100ths(), current_position_lift_percent_100ths() {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* position_aware_lift */

// Attributes of AbsolutePosition feature may have dependency on LF, TL, PA_LF, PA_TL
// feature, one must add features according to the usecase first.

namespace absolute_position {

typedef struct config {
    uint16_t physical_closed_limit_lift;
    nullable<uint16_t> current_position_lift;
    uint16_t installed_open_limit_lift;
    uint16_t installed_closed_limit_lift;
    uint16_t physical_closed_limit_tilt;
    nullable<uint16_t> current_position_tilt;
    uint16_t installed_open_limit_tilt;
    uint16_t installed_closed_limit_tilt;
    config() : physical_closed_limit_lift(0), current_position_lift(), installed_open_limit_lift(0), installed_closed_limit_lift(65534), physical_closed_limit_tilt(0), current_position_tilt(), installed_open_limit_tilt(0), installed_closed_limit_tilt(65534) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* absolute_position */

// PositionAwareTilt feature is dependent on Tilt feature, in order to add
// PositionAwareTilt feature one must add Tilt feature first.

namespace position_aware_tilt {

typedef struct config {
    nullable<uint8_t> current_position_tilt_percentage;
    nullable<uint16_t> target_position_tilt_percent_100ths;
    nullable<uint16_t> current_position_tilt_percent_100ths;
    config() : current_position_tilt_percentage(), target_position_tilt_percent_100ths(), current_position_tilt_percent_100ths() {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* position_aware_tilt */
} /* feature */
} /* window_covering */

namespace wifi_network_diagnotics {
namespace feature {

namespace packets_counts {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* packets_counts */

namespace error_counts {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* error_counts */

} /* feature */
} /* wifi_network_diagnotics */

namespace ethernet_network_diagnostics {
namespace feature {

namespace packets_counts {

typedef struct config {
    uint64_t packet_rx_count;
    uint64_t packet_tx_count;
    config() : packet_rx_count(0), packet_tx_count(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* packets_counts */

namespace error_counts {

typedef struct config {
    uint64_t tx_error_count;
    uint64_t collision_count;
    uint64_t overrun_count;
    config() : tx_error_count(0), collision_count(0), overrun_count(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* error_counts */

} /* feature */
} /* ethernet_network_diagnostics */

namespace thermostat {
namespace feature {

namespace heating {

typedef struct config {
   int16_t occupied_heating_setpoint;

   config (): occupied_heating_setpoint(2000) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* heating */

namespace cooling {

typedef struct config {
   int16_t occupied_cooling_setpoint;

   config (): occupied_cooling_setpoint(2600) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* cooling */

// Attributes of Occupancy feature may have dependency on Heating, Cooling and Setback
// feature, one must add features according to the usecase first.
namespace occupancy {

typedef struct config {
   uint8_t occupancy;
   int16_t unoccupied_cooling_setpoint;
   int16_t unoccupied_heating_setpoint;
   nullable<uint8_t> unoccupied_setback;
   nullable<uint8_t> unoccupied_setback_min;
   nullable<uint8_t> unoccupied_setback_max;

   config (): occupancy(1), unoccupied_cooling_setpoint(2600), unoccupied_heating_setpoint(2000), unoccupied_setback(), unoccupied_setback_min(), unoccupied_setback_max() {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* occupancy */

namespace schedule_configuration {

typedef struct config {
   uint8_t start_of_week;
   uint8_t number_of_weekly_transitions;
   uint8_t number_of_daily_transitions;

   config (): start_of_week(0), number_of_weekly_transitions(0), number_of_daily_transitions(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* schedule_configuration */

namespace setback {

typedef struct config {
   nullable<uint8_t> occupied_setback;
   nullable<uint8_t> occupied_setback_min;
   nullable<uint8_t> occupied_setback_max;

   config (): occupied_setback(), occupied_setback_min(), occupied_setback_max() {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* setback */

// Auto feature mandates the Heating and Cooling feature, while adding
// Auto feature one must add Heating and Colling features.

namespace auto_mode {

typedef struct config {
   int8_t min_setpoint_dead_band;

   config (): min_setpoint_dead_band(25) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* auto_mode */

namespace local_temperature_not_exposed {

typedef struct config {
    int16_t local_temperature_calibration;

    config (): local_temperature_calibration(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* local_temperature_not_exposed */

namespace matter_schedule_configuration {

typedef struct config {
    uint8_t number_of_schedules;
    uint8_t number_of_schedule_transitions;
    nullable<uint8_t> number_of_schedule_transition_per_day;
    uint8_t active_schedule_handle[k_max_active_schedule_handle];

    config (): number_of_schedules(0), number_of_schedule_transitions(0), number_of_schedule_transition_per_day() {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* matter_schedule_configuration */

namespace presets {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* presets */

} /* feature */
} /* thermostat */

namespace air_quality {
namespace feature {

namespace fair {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* fair */

namespace mod {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* mod */

namespace vpoor {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* vpoor */

namespace xpoor {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* xpoor */

} /* feature */
} /* air_quality */

namespace carbon_monoxide_concentration_measurement {
namespace feature {

namespace numeric_measurement {
typedef struct config {
    nullable<float> measured_value;
    nullable<float> min_measured_value;
    nullable<float> max_measured_value;
    uint8_t measurement_unit;
    config() : measured_value(), min_measured_value(), max_measured_value(), measurement_unit(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* numeric_measurement */

namespace level_indication {
typedef struct config {
    uint8_t level_value;
    config() : level_value(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* level_indication */

namespace medium_level {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* medium_level */

namespace critical_level {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* critical_level */

namespace peak_measurement {
typedef struct config {
    nullable<float> peak_measured_value;
    uint32_t peak_measured_value_window;
    config() : peak_measured_value(), peak_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* peak_measurement */

namespace average_measurement {
typedef struct config {
    nullable<float> average_measured_value;
    uint32_t average_measured_value_window;
    config() : average_measured_value(), average_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* average_measurement */

} /* feature */
} /* carbon_monoxide_concentration_measurement */

namespace carbon_dioxide_concentration_measurement {
namespace feature {

namespace numeric_measurement {
typedef struct config {
    nullable<float> measured_value;
    nullable<float> min_measured_value;
    nullable<float> max_measured_value;
    uint8_t measurement_unit;
    config() : measured_value(), min_measured_value(), max_measured_value(), measurement_unit(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* numeric_measurement */

namespace level_indication {
typedef struct config {
    uint8_t level_value;
    config() : level_value(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* level_indication */

namespace medium_level {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* medium_level */

namespace critical_level {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* critical_level */

namespace peak_measurement {
typedef struct config {
    nullable<float> peak_measured_value;
    uint32_t peak_measured_value_window;
    config() : peak_measured_value(), peak_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* peak_measurement */

namespace average_measurement {
typedef struct config {
    nullable<float> average_measured_value;
    uint32_t average_measured_value_window;
    config() : average_measured_value(), average_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* average_measurement */

} /* feature */
} /* carbon_dioxide_concentration_measurement */

namespace nitrogen_dioxide_concentration_measurement {
namespace feature {

namespace numeric_measurement {
typedef struct config {
    nullable<float> measured_value;
    nullable<float> min_measured_value;
    nullable<float> max_measured_value;
    uint8_t measurement_unit;
    config() : measured_value(), min_measured_value(), max_measured_value(), measurement_unit(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* numeric_measurement */

namespace level_indication {
typedef struct config {
    uint8_t level_value;
   config() : level_value(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* level_indication */

namespace medium_level {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* medium_level */

namespace critical_level {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* critical_level */

namespace peak_measurement {
typedef struct config {
    nullable<float> peak_measured_value;
    uint32_t peak_measured_value_window;
    config() : peak_measured_value(), peak_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* peak_measurement */

namespace average_measurement {
typedef struct config {
    nullable<float> average_measured_value;
    uint32_t average_measured_value_window;
    config() : average_measured_value(), average_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* average_measurement */

} /* feature */
} /* nitrogen_dioxide_concentration_measurement */

namespace ozone_concentration_measurement {
namespace feature {

namespace numeric_measurement {
typedef struct config {
    nullable<float> measured_value;
    nullable<float> min_measured_value;
    nullable<float> max_measured_value;
    uint8_t measurement_unit;
    config() : measured_value(), min_measured_value(), max_measured_value(), measurement_unit(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* numeric_measurement */

namespace level_indication {
typedef struct config {
    uint8_t level_value;
   config() : level_value(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* level_indication */

namespace medium_level {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* medium_level */

namespace critical_level {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* critical_level */

namespace peak_measurement {
typedef struct config {
    nullable<float> peak_measured_value;
    uint32_t peak_measured_value_window;
    config() : peak_measured_value(), peak_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* peak_measurement */

namespace average_measurement {
typedef struct config {
    nullable<float> average_measured_value;
    uint32_t average_measured_value_window;
    config() : average_measured_value(), average_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* average_measurement */

} /* feature */
} /* ozone_concentration_measurement */

namespace formaldehyde_concentration_measurement {
namespace feature {

namespace numeric_measurement {
typedef struct config {
    nullable<float> measured_value;
    nullable<float> min_measured_value;
    nullable<float> max_measured_value;
    uint8_t measurement_unit;
    config() : measured_value(), min_measured_value(), max_measured_value(), measurement_unit(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* numeric_measurement */

namespace level_indication {
typedef struct config {
    uint8_t level_value;
   config() : level_value(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* level_indication */

namespace medium_level {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* medium_level */

namespace critical_level {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* critical_level */

namespace peak_measurement {
typedef struct config {
    nullable<float> peak_measured_value;
    uint32_t peak_measured_value_window;
    config() : peak_measured_value(), peak_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* peak_measurement */

namespace average_measurement {
typedef struct config {
    nullable<float> average_measured_value;
    uint32_t average_measured_value_window;
    config() : average_measured_value(), average_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* average_measurement */

} /* feature */
} /* formaldehyde_concentration_measurement */

namespace pm1_concentration_measurement {
namespace feature {

namespace numeric_measurement {
typedef struct config {
    nullable<float> measured_value;
    nullable<float> min_measured_value;
    nullable<float> max_measured_value;
    uint8_t measurement_unit;
    config() : measured_value(), min_measured_value(), max_measured_value(), measurement_unit(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* numeric_measurement */

namespace level_indication {
typedef struct config {
    uint8_t level_value;
   config() : level_value(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* level_indication */

namespace medium_level {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* medium_level */

namespace critical_level {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* critical_level */

namespace peak_measurement {
typedef struct config {
    nullable<float> peak_measured_value;
    uint32_t peak_measured_value_window;
    config() : peak_measured_value(), peak_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* peak_measurement */

namespace average_measurement {
typedef struct config {
    nullable<float> average_measured_value;
    uint32_t average_measured_value_window;
    config() : average_measured_value(), average_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* average_measurement */

} /* feature */
} /* pm1_concentration_measurement */

namespace pm25_concentration_measurement {
namespace feature {

namespace numeric_measurement {
typedef struct config {
    nullable<float> measured_value;
    nullable<float> min_measured_value;
    nullable<float> max_measured_value;
    uint8_t measurement_unit;
    config() : measured_value(), min_measured_value(), max_measured_value(), measurement_unit(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* numeric_measurement */

namespace level_indication {
typedef struct config {
    uint8_t level_value;
   config() : level_value(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* level_indication */

namespace medium_level {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* medium_level */

namespace critical_level {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* critical_level */

namespace peak_measurement {
typedef struct config {
    nullable<float> peak_measured_value;
    uint32_t peak_measured_value_window;
    config() : peak_measured_value(), peak_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* peak_measurement */

namespace average_measurement {
typedef struct config {
    nullable<float> average_measured_value;
    uint32_t average_measured_value_window;
    config() : average_measured_value(), average_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* average_measurement */

} /* feature */
} /* pm25_concentration_measurement */

namespace pm10_concentration_measurement {
namespace feature {

namespace numeric_measurement {
typedef struct config {
    nullable<float> measured_value;
    nullable<float> min_measured_value;
    nullable<float> max_measured_value;
    uint8_t measurement_unit;
    config() : measured_value(), min_measured_value(), max_measured_value(), measurement_unit(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* numeric_measurement */

namespace level_indication {
typedef struct config {
    uint8_t level_value;
   config() : level_value(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* level_indication */

namespace medium_level {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* medium_level */

namespace critical_level {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* critical_level */

namespace peak_measurement {
typedef struct config {
    nullable<float> peak_measured_value;
    uint32_t peak_measured_value_window;
    config() : peak_measured_value(), peak_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* peak_measurement */

namespace average_measurement {
typedef struct config {
    nullable<float> average_measured_value;
    uint32_t average_measured_value_window;
    config() : average_measured_value(), average_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* average_measurement */

} /* feature */
} /* pm10_concentration_measurement */

namespace radon_concentration_measurement {
namespace feature {

namespace numeric_measurement {
typedef struct config {
    nullable<float> measured_value;
    nullable<float> min_measured_value;
    nullable<float> max_measured_value;
    uint8_t measurement_unit;
    config() : measured_value(), min_measured_value(), max_measured_value(), measurement_unit(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* numeric_measurement */

namespace level_indication {
typedef struct config {
    uint8_t level_value;
   config() : level_value(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* level_indication */

namespace medium_level {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* medium_level */

namespace critical_level {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* critical_level */

namespace peak_measurement {
typedef struct config {
    nullable<float> peak_measured_value;
    uint32_t peak_measured_value_window;
    config() : peak_measured_value(), peak_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* peak_measurement */

namespace average_measurement {
typedef struct config {
    nullable<float> average_measured_value;
    uint32_t average_measured_value_window;
    config() : average_measured_value(), average_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* average_measurement */

} /* feature */
} /* radon_concentration_measurement */

namespace total_volatile_organic_compounds_concentration_measurement {
namespace feature {

namespace numeric_measurement {
typedef struct config {
    nullable<float> measured_value;
    nullable<float> min_measured_value;
    nullable<float> max_measured_value;
    uint8_t measurement_unit;
    config() : measured_value(), min_measured_value(), max_measured_value(), measurement_unit(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* numeric_measurement */

namespace level_indication {
typedef struct config {
    uint8_t level_value;
   config() : level_value(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* level_indication */

namespace medium_level {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* medium_level */

namespace critical_level {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* critical_level */

namespace peak_measurement {
typedef struct config {
    nullable<float> peak_measured_value;
    uint32_t peak_measured_value_window;
    config() : peak_measured_value(), peak_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* peak_measurement */

namespace average_measurement {
typedef struct config {
    nullable<float> average_measured_value;
    uint32_t average_measured_value_window;
    config() : average_measured_value(), average_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* average_measurement */

} /* feature */
} /* total_volatile_organic_compounds_concentration_measurement */

namespace hepa_filter_monitoring {
namespace feature {

namespace condition {
typedef struct config {
    uint8_t condition;
    uint8_t degradation_direction;
    config() : condition(0), degradation_direction(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* condition */

namespace warning {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* warning */

namespace replacement_product_list {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* replacement_product_list */

} /* feature */
} /* hepa_filter_monitoring */

namespace activated_carbon_filter_monitoring {
namespace feature {

namespace condition {
typedef struct config {
    uint8_t condition;
    uint8_t degradation_direction;
    config() : condition(0), degradation_direction(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* condition */

namespace warning {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* warning */

namespace replacement_product_list {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* replacement_product_list */

} /* feature */
} /* activated_carbon_filter_monitoring */

namespace laundry_washer_controls {
namespace feature {

namespace spin {

typedef struct config {
    uint8_t spin_speed_current;
    config() : spin_speed_current(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* spin */

namespace rinse {

typedef struct config {
    uint8_t number_of_rinses;
    config() : number_of_rinses(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* rinse */

} /* feature */
} /* laundry_washer_controls */

namespace smoke_co_alarm {
namespace feature {

namespace smoke_alarm {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* smoke_alarm */

namespace co_alarm {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* co_alarm */

} /* feature */
} /* smoke_co_alarm */

namespace switch_cluster {
namespace feature {

// Note: Latching and Momentary switch features are mutually exclusive, only one of them shall be supported.

namespace latching_switch {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* latching_switch */

namespace momentary_switch {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* momentary_switch */

// MomentarySwitchRelease feature has dependency on MomentarySwitch and !ActionSwitch features, in order to add
// MomentarySwitchRelease feature one must add MomentarySwitch feature first.

namespace momentary_switch_release {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* momentary_switch_release */

// MomentarySwitchLongPress feature has dependency on MomentarySwitch and (MomentarySwitchRelease or ActionSwitch) features, in order to add
// MomentarySwitchLongPress feature one must add MomentarySwitch and (MomentarySwitchRelease or ActionSwitch) features first.

namespace momentary_switch_long_press {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* momentary_switch_long_press */

// MomentarySwitchMultiPress feature has dependency on ActionSwitch or (MomentarySwitch and MomentarySwitchRelease) features, in order to add
// MomentarySwitchMultiPress feature one must add ActionSwitch or (MomentarySwitch and MomentarySwitchRelease) features first.

namespace momentary_switch_multi_press {

typedef struct config {
    uint8_t multi_press_max;
    config() : multi_press_max(2) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* momentary_switch_multi_press */

// ActionSwitch feature has dependency on MomentarySwitch feature, in order to add
// ActionSwitch feature one must add MomentarySwitch feature first.

namespace action_switch {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* action_switch */
} /* feature */
} /* switch_cluster */

namespace unit_localization {
namespace feature {

namespace temperature_unit {

typedef struct config {
    uint8_t temperature_unit;
    config() : temperature_unit(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* temperature_unit */

} /* feature */
} /* unit_localization */

namespace time_format_localization {
namespace feature {

namespace calendar_format {

typedef struct config {
    uint8_t active_calendar_type;
    config() : active_calendar_type(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* calendar_format */

} /* feature */
} /* time_format_localization */

namespace mode_select {
namespace feature {

namespace on_off {

typedef struct config {
    nullable<uint8_t> on_mode;
    config() : on_mode() {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* on_off */

} /* feature */
} /* mode_select */

namespace software_diagnostics {
namespace feature {

namespace watermarks {

typedef struct config {
    uint64_t current_heap_high_watermark;
    config() : current_heap_high_watermark(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* watermarks */

} /* feature */
} /* software_diagnostics */

namespace temperature_control {
namespace feature {

// TemperatureNumber and TemperatureLevel features are mutually exclusive,
// only one of them shall present.
namespace temperature_number {
typedef struct config {
    int16_t temp_setpoint;
    int16_t min_temperature;
    int16_t max_temperature;
    config() : temp_setpoint(1), min_temperature(0), max_temperature(10) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* temperature_number */

// TemperatureNumber and TemperatureLevel features are mutually exclusive,
// only one of them shall present.
namespace temperature_level {
typedef struct config {
    uint8_t selected_temp_level;
    config() : selected_temp_level(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* temperature_level */

// TemperatureStep feature have conformance of TemperatureNumber feature,
// in order to support TemperatureStep cluster shall support TemperatureNumber.
namespace temperature_step {
typedef struct config {
    int16_t step;
    config() : step(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* temperature_step */

} /* feature */
} /* temperature_control */

namespace fan_control {
namespace feature {

namespace multi_speed {
typedef struct config {
    uint8_t speed_max;
    nullable<uint8_t> speed_setting;
    uint8_t speed_current;
    config() : speed_max(10), speed_setting(0), speed_current(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* multi_speed */

namespace fan_auto {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* fan_auto */

namespace rocking {
typedef struct config {
    uint8_t rock_support;
    uint8_t rock_setting;
    config() : rock_support(0), rock_setting(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* rocking */

namespace wind {
typedef struct config {
    uint8_t wind_support;
    uint8_t wind_setting;
    config() : wind_support(0), wind_setting(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* wind */

namespace step {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* step */

namespace airflow_direction {
typedef struct config {
    uint8_t airflow_direction;
    config() : airflow_direction(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* airflow_direction */

} /* feature */
} /* fan_control */

namespace keypad_input {
namespace feature {

namespace navigation_key_codes {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* navigation_key_codes */

namespace location_keys {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* location_keys */

namespace number_keys {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* number_keys */

} /* feature */
} /* keypad_input */

namespace boolean_state_configuration {
namespace feature {

namespace visual {

typedef struct config {
    uint8_t alarms_active;
    uint8_t alarms_supported;
    config() : alarms_active(0), alarms_supported(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* visual */

namespace audible {

typedef struct config {
    uint8_t alarms_active;
    uint8_t alarms_supported;
    config() : alarms_active(0), alarms_supported(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* audible */

namespace alarm_suppress {

typedef struct config {
    uint8_t alarms_suppressed;
    config() : alarms_suppressed(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* alarm_suppress */

namespace sensitivity_level {

typedef struct config {
    uint8_t supported_sensitivity_levels;
    config() : supported_sensitivity_levels(10) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* sensitivity_level */

} /* feature */
} /* boolean_state_configuration */

namespace power_topology {
namespace feature {

namespace node_topology {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* node_topology */

namespace tree_topology {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* tree_topology */

namespace set_topology {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* set_topology */

namespace dynamic_power_flow {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* dynamic_power_flow */

} /* feature */
} /* power_topology */

namespace electrical_power_measurement {
namespace feature {

namespace direct_current {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* direct_current */

namespace alternating_current {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* alternating_current */

namespace polyphase_power {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* polyphase_power */

namespace harmonics {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* harmonics */

namespace power_quality {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* power_quality */

} /* feature */
} /* electrical_power_measurement */

namespace electrical_energy_measurement {
namespace feature {

namespace imported_energy {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* imported_energy */

namespace exported_energy {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* exported_energy */

namespace cumulative_energy {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* cumulative_energy */

namespace periodic_energy {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* periodic_energy */

} /* feature */
} /* electrical_energy_measurement */

namespace door_lock {
namespace feature {

namespace pin_credential {
typedef struct config {
    uint16_t number_pin_users_supported;
    uint8_t max_pin_code_length;
    uint8_t min_pin_code_length;
    uint8_t wrong_code_entry_limit;
    uint8_t user_code_temporary_disable_time;
    bool require_pin_for_remote_operation;
    config() : number_pin_users_supported(5), max_pin_code_length(16), min_pin_code_length(4),
               wrong_code_entry_limit(5), user_code_temporary_disable_time(5),
               require_pin_for_remote_operation(true) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* pin_credential */

namespace rfid_credential {
typedef struct config {
    uint16_t number_rfid_users_supported;
    uint8_t max_rfid_code_length;
    uint8_t min_rfid_code_length;
    uint8_t wrong_code_entry_limit;
    uint8_t user_code_temporary_disable_time;
    config() : number_rfid_users_supported(5), max_rfid_code_length(16), min_rfid_code_length(4),
               wrong_code_entry_limit(5), user_code_temporary_disable_time(5) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* rfid_credential */

namespace finger_credentials {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* finger_credentials */

namespace weekday_access_schedules {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* weekday_access_schedules */

namespace door_position_sensor {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* door_position_sensor */

namespace face_credentials {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* face_credentials */

namespace credential_over_the_air_access {
typedef struct config {
    bool require_pin_for_remote_operation;
    config() : require_pin_for_remote_operation(false) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* credential_over_the_air_access */

namespace user {
typedef struct config {
    uint16_t number_of_total_user_supported;
    uint16_t expiring_user_timeout;
    uint8_t credential_rules_supported;
    uint8_t number_of_credentials_supported_per_user;
    config() : number_of_total_user_supported(5), expiring_user_timeout(5), credential_rules_supported(0), number_of_credentials_supported_per_user(3) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* user */

namespace year_day_access_schedules {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* year_day_access_schedules */

namespace holiday_schedules {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* holiday_schedules */

namespace holiday_schedules {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* holiday_schedules */

namespace unbolting {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* unbolting */

namespace aliro_provisioning {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* aliro_provisioning */

namespace aliro_bleuwb{

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* aliro_bleuwb */

} /* feature */
}/* door_lock */

namespace energy_evse {
namespace feature {
namespace charging_preferences {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* charging_preferences */

namespace soc_reporting {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* soc_reporting */

namespace plug_and_charge {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* plug_and_charge */

namespace rfid {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* rfid */

namespace v2x {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* v2x */

} /* feature */
} /* energy_evse */

namespace microwave_oven_control {
namespace feature {

namespace power_as_number {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* power_as_number */

namespace power_in_watts {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* power_in_watts */

namespace power_number_limits {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* power_number_limits */

} /* feature */
} /* microwave_oven_control */

namespace valve_configuration_and_control {
namespace feature {

namespace time_sync {
typedef struct config {
    nullable<uint64_t> auto_close_time;
    config() : auto_close_time() {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* time_sync */

namespace level {
typedef struct config {
    nullable<uint8_t> current_level;
    nullable<uint8_t> target_level;
    config() : current_level(), target_level() {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* level */

} /* feature */
} /* valve_configuration_and_control */

namespace device_energy_management {
namespace feature {

namespace power_adjustment {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* power_adjustment */

namespace power_forecast_reporting {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* power_forecast_reporting */

namespace state_forecast_reporting {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* state_forecast_reporting */

namespace start_time_adjustment {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* start_time_adjustment */

namespace pausable {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* pausable */

namespace forecast_adjustment {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* forecast_adjustment */

namespace constraint_based_adjustment {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* constraint_based_adjustment */

} /* feature */
} /* device_energy_management */

namespace thread_border_router_management {
namespace feature {

namespace pan_change {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* pan_change */

} /* feature */
} /* thread_border_router_management */

namespace service_area {
namespace feature {

namespace select_while_running {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* select_while_running */

namespace progress_reporting {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* progress_reporting */

namespace maps {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* maps */

} /* feature */
} /* service_area */

namespace water_heater_management {
namespace feature {

namespace energy_management {

typedef struct config {
    uint16_t tank_volume;
    int64_t estimated_heat_required;
    config(): tank_volume(0), estimated_heat_required(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* energy_management */

namespace tank_percent {

typedef struct config {
    uint8_t tank_percentage;
    config(): tank_percentage(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* tank_percent */
} /* feature */
} /* water_heater_management */

namespace energy_preference {
namespace feature {

namespace energy_balance {
typedef struct config {
    uint8_t current_energy_balance;
    config() : current_energy_balance(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* energy_balance */

namespace low_power_mode_sensitivity {
typedef struct config {
    uint8_t current_low_power_mode_sensitivity;
    config() : current_low_power_mode_sensitivity(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* low_power_mode_sensitivity */

} /* feature */
} /* energy_preference */

namespace occupancy_sensing {
namespace feature {

namespace other {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* other */

namespace passive_infrared {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* passive_infrared */

namespace ultrasonic {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* ultrasonic */

namespace physical_contact {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* physical_contact */

namespace active_infrared {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* active_infrared */

namespace radar {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* radar */

namespace rf_sensing {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* rf_sensing */

namespace vision {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* vision */
} /* feature */
} /* occupancy_sensing */

namespace pump_configuration_and_control {
namespace feature {

namespace constant_pressure {

typedef struct config {
    nullable<int16_t> min_const_pressure;
    nullable<int16_t> max_const_pressure;
    config() : min_const_pressure(), max_const_pressure() {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* constant_pressure */

namespace compensated_pressure {

typedef struct config {
    nullable<int16_t> min_comp_pressure;
    nullable<int16_t> max_comp_pressure;
    config() : min_comp_pressure(), max_comp_pressure() {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* compensated_pressure */

namespace constant_flow {

typedef struct config {
    nullable<uint16_t> min_const_flow;
    nullable<uint16_t> max_const_flow;
    config() : min_const_flow(), max_const_flow() {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* constant_flow */

namespace constant_speed {

typedef struct config {
    nullable<uint16_t> min_const_speed;
    nullable<uint16_t> max_const_speed;
    config() : min_const_speed(), max_const_speed() {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* constant_speed */

namespace constant_temperature {

typedef struct config {
    nullable<int16_t> min_const_temp;
    nullable<int16_t> max_const_temp;
    config() : min_const_temp(), max_const_temp() {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* constant_temperature */

namespace automatic {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* automatic */

namespace local_operation {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* automatic */
} /* feature */
} /* pump_configuration_and_control */

namespace time_synchronization {
namespace feature {

namespace time_zone {
typedef struct config {
    uint8_t time_zone_database;
    config() : time_zone_database(2/* None */) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* time_zone */

namespace ntp_client {
typedef struct config {
    bool supports_dns_resolve;
    config() : supports_dns_resolve(false) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* ntp_client */

namespace ntp_server {
typedef struct config {
    bool ntp_server_available;
    config() : ntp_server_available(false) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* ntp_server */

namespace time_sync_client {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* time_sync_client */

} /* feature */
} /* time_synchronization */

} /* cluster */
} /* esp_matter */
