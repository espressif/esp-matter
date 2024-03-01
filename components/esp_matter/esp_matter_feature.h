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

#include <esp_matter_core.h>
#include <stdint.h>

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

namespace administrator_commissioning {

namespace feature {

namespace basic {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* basic */

} /* feature */
} /* administrator_commissioning */

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
    char bat_replacement_description[61];
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
typedef struct config {
    uint16_t clients_supported_per_fabric;
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* check_in_protocol_support */
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
               color_temp_physical_max_mireds(0xfeff), couple_color_temp_to_level_min_mireds(0),
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

namespace diagnostics_network_wifi {
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
} /* diagnostics_network_wifi */

namespace diagnostics_network_ethernet {
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
} /* diagnostics_network_ethernet */

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
    nullable<uint16_t> measured_value;
    nullable<uint16_t> min_measured_value;
    nullable<uint16_t> max_measured_value;
    uint16_t uncertainty;
    uint8_t measurement_unit;
    config() : measured_value(), min_measured_value(), max_measured_value(), uncertainty(0), measurement_unit(0) {}
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
    nullable<uint16_t> peak_measured_value;
    uint32_t peak_measured_value_window;
    config() : peak_measured_value(), peak_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* peak_measurement */

namespace average_measurement {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* average_measurement */

} /* feature */
} /* carbon_monoxide_concentration_measurement */

namespace carbon_dioxide_concentration_measurement {
namespace feature {

namespace numeric_measurement {
typedef struct config {
    nullable<uint16_t> measured_value;
    nullable<uint16_t> min_measured_value;
    nullable<uint16_t> max_measured_value;
    uint16_t uncertainty;
    uint8_t measurement_unit;
    config() : measured_value(), min_measured_value(), max_measured_value(), uncertainty(0), measurement_unit(0) {}
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
    nullable<uint16_t> peak_measured_value;
    uint32_t peak_measured_value_window;
    config() : peak_measured_value(), peak_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* peak_measurement */

namespace average_measurement {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* average_measurement */

} /* feature */
} /* carbon_dioxide_concentration_measurement */

namespace nitrogen_dioxide_concentration_measurement {
namespace feature {

namespace numeric_measurement {
typedef struct config {
    nullable<uint16_t> measured_value;
    nullable<uint16_t> min_measured_value;
    nullable<uint16_t> max_measured_value;
    uint16_t uncertainty;
    uint8_t measurement_unit;
    config() : measured_value(), min_measured_value(), max_measured_value(), uncertainty(0), measurement_unit(0) {}
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
    nullable<uint16_t> peak_measured_value;
    uint32_t peak_measured_value_window;
    config() : peak_measured_value(), peak_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* peak_measurement */

namespace average_measurement {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* average_measurement */

} /* feature */
} /* nitrogen_dioxide_concentration_measurement */

namespace ozone_concentration_measurement {
namespace feature {

namespace numeric_measurement {
typedef struct config {
    nullable<uint16_t> measured_value;
    nullable<uint16_t> min_measured_value;
    nullable<uint16_t> max_measured_value;
    uint16_t uncertainty;
    uint8_t measurement_unit;
    config() : measured_value(), min_measured_value(), max_measured_value(), uncertainty(0), measurement_unit(0) {}
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
    nullable<uint16_t> peak_measured_value;
    uint32_t peak_measured_value_window;
    config() : peak_measured_value(), peak_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* peak_measurement */

namespace average_measurement {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* average_measurement */

} /* feature */
} /* ozone_concentration_measurement */

namespace formaldehyde_concentration_measurement {
namespace feature {

namespace numeric_measurement {
typedef struct config {
    nullable<uint16_t> measured_value;
    nullable<uint16_t> min_measured_value;
    nullable<uint16_t> max_measured_value;
    uint16_t uncertainty;
    uint8_t measurement_unit;
    config() : measured_value(), min_measured_value(), max_measured_value(), uncertainty(0), measurement_unit(0) {}
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
    nullable<uint16_t> peak_measured_value;
    uint32_t peak_measured_value_window;
    config() : peak_measured_value(), peak_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* peak_measurement */

namespace average_measurement {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* average_measurement */

} /* feature */
} /* formaldehyde_concentration_measurement */

namespace pm1_concentration_measurement {
namespace feature {

namespace numeric_measurement {
typedef struct config {
    nullable<uint16_t> measured_value;
    nullable<uint16_t> min_measured_value;
    nullable<uint16_t> max_measured_value;
    uint16_t uncertainty;
    uint8_t measurement_unit;
    config() : measured_value(), min_measured_value(), max_measured_value(), uncertainty(0), measurement_unit(0) {}
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
    nullable<uint16_t> peak_measured_value;
    uint32_t peak_measured_value_window;
    config() : peak_measured_value(), peak_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* peak_measurement */

namespace average_measurement {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* average_measurement */

} /* feature */
} /* pm1_concentration_measurement */

namespace pm25_concentration_measurement {
namespace feature {

namespace numeric_measurement {
typedef struct config {
    nullable<uint16_t> measured_value;
    nullable<uint16_t> min_measured_value;
    nullable<uint16_t> max_measured_value;
    uint16_t uncertainty;
    uint8_t measurement_unit;
    config() : measured_value(), min_measured_value(), max_measured_value(), uncertainty(0), measurement_unit(0) {}
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
    nullable<uint16_t> peak_measured_value;
    uint32_t peak_measured_value_window;
    config() : peak_measured_value(), peak_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* peak_measurement */

namespace average_measurement {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* average_measurement */

} /* feature */
} /* pm25_concentration_measurement */

namespace pm10_concentration_measurement {
namespace feature {

namespace numeric_measurement {
typedef struct config {
    nullable<uint16_t> measured_value;
    nullable<uint16_t> min_measured_value;
    nullable<uint16_t> max_measured_value;
    uint16_t uncertainty;
    uint8_t measurement_unit;
    config() : measured_value(), min_measured_value(), max_measured_value(), uncertainty(0), measurement_unit(0) {}
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
    nullable<uint16_t> peak_measured_value;
    uint32_t peak_measured_value_window;
    config() : peak_measured_value(), peak_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* peak_measurement */

namespace average_measurement {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* average_measurement */

} /* feature */
} /* pm10_concentration_measurement */

namespace radon_concentration_measurement {
namespace feature {

namespace numeric_measurement {
typedef struct config {
    nullable<uint16_t> measured_value;
    nullable<uint16_t> min_measured_value;
    nullable<uint16_t> max_measured_value;
    uint16_t uncertainty;
    uint8_t measurement_unit;
    config() : measured_value(), min_measured_value(), max_measured_value(), uncertainty(0), measurement_unit(0) {}
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
    nullable<uint16_t> peak_measured_value;
    uint32_t peak_measured_value_window;
    config() : peak_measured_value(), peak_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* peak_measurement */

namespace average_measurement {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* average_measurement */

} /* feature */
} /* radon_concentration_measurement */

namespace total_volatile_organic_compounds_concentration_measurement {
namespace feature {

namespace numeric_measurement {
typedef struct config {
    nullable<uint16_t> measured_value;
    nullable<uint16_t> min_measured_value;
    nullable<uint16_t> max_measured_value;
    uint16_t uncertainty;
    uint8_t measurement_unit;
    config() : measured_value(), min_measured_value(), max_measured_value(), uncertainty(0), measurement_unit(0) {}
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
    nullable<uint16_t> peak_measured_value;
    uint32_t peak_measured_value_window;
    config() : peak_measured_value(), peak_measured_value_window(1) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* peak_measurement */

namespace average_measurement {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

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

// MomentarySwitchRelease feature is dependent on MomentarySwitch feature, in order to add
// MomentarySwitchRelease feature one must add MomentarySwitch feature first.

namespace momentary_switch_release {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* momentary_switch_release */

// MomentarySwitchRelease feature is dependent on MomentarySwitch and MomentarySwitchRelease feature, in order to add
// MomentarySwitchRelease feature one must add MomentarySwitch and MomentarySwitchRelease feature first.

namespace momentary_switch_long_press {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* momentary_switch_long_press */

// MomentarySwitchRelease feature is dependent on MomentarySwitch and MomentarySwitchRelease feature, in order to add
// MomentarySwitchRelease feature one must add MomentarySwitch and MomentarySwitchRelease feature first.

namespace momentary_switch_multi_press {

typedef struct config {
    uint8_t multi_press_max;
    config() : multi_press_max(2) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* momentary_switch_multi_pressy */
} /* feature */
} /* switch_cluster */

namespace time_format_localization {
namespace feature {

namespace calendar_format {

typedef struct config {
    nullable<uint8_t> active_calendar_type;
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
constexpr uint32_t k_max_limit = 100;
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

} /* cluster */
} /* esp_matter */
