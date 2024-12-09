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
#include <esp_matter_attribute.h>
#include <esp_matter_feature.h>
#include <stdint.h>

#include <app-common/zap-generated/cluster-enums.h>
#include <lib/support/TypeTraits.h>

namespace esp_matter {
namespace cluster {

/** Common cluster plugin init callback
 *
 * This is the common plugin init callback which calls the plugin init callbacks in the clusters.
 *
 * This common API has been added so that the specific APIs in zap-generated/PluginApplicationCallbacks.h can be
 * removed.
 */
void plugin_init_callback_common();

/** Common cluster delegate init callback
 *
 * This is the common delegate init callback which calls the delegate init callbacks in the clusters.
 */
void delegate_init_callback_common();

/** Common cluster add bounds callback
 *
 * This is the common add bounds callback which set the bounds to all the attributes of the clusters.
 */
void add_bounds_callback_common();

/** Specific cluster create APIs
 *
 * These APIs also create the mandatory attributes and commands for the cluster. If the mandatory attribute is not
 * managed internally, then a config is present for that attribute. The constructor for the config will set the
 * attribute to the default value from the spec.
 *
 * If some standard cluster is not present here, it can be added.
 * If a custom cluster needs to be created, the low level esp_matter::cluster::create() API can be used.
 */

namespace common {

typedef struct config {
    // Empty config for API consistency
} config_t;

} /* common */

namespace descriptor {
typedef struct config {
    uint32_t features;
    config() : features(0) {}
} config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* descriptor */

namespace actions {
using config_t = common::config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* actions */

namespace access_control {
using config_t = common::config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* access_control */

namespace basic_information {
typedef struct config {
    char node_label[k_max_node_label_length + 1];
    config() : node_label{0} {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* basic_information */

namespace binding {
using config_t = common::config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* binding */

namespace ota_provider {
using config_t = common::config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* ota_provider */

namespace ota_requestor {
typedef struct config {
    bool update_possible;
    uint8_t update_state;
    nullable<uint8_t> update_state_progress;
    config() : update_possible(true), update_state(0), update_state_progress() {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* ota_requestor */

namespace general_commissioning {
typedef struct config {
    uint64_t breadcrumb;
    config() : breadcrumb(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* general_commissioning */

namespace network_commissioning {
typedef struct config {
    uint32_t feature_map;
    config() :
#if CHIP_DEVICE_CONFIG_ENABLE_WIFI
        feature_map(chip::to_underlying(chip::app::Clusters::NetworkCommissioning::Feature::kWiFiNetworkInterface)) {}
#elif CHIP_DEVICE_CONFIG_ENABLE_THREAD
        feature_map(chip::to_underlying(chip::app::Clusters::NetworkCommissioning::Feature::kThreadNetworkInterface)) {}
#else
        feature_map(chip::to_underlying(chip::app::Clusters::NetworkCommissioning::Feature::kEthernetNetworkInterface)) {}
#endif
} config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* network_commissioning */

namespace diagnostic_logs {
using config_t = common::config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* diagnostic_logs */

namespace general_diagnostics {
using config_t = common::config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* general_diagnostics */

namespace software_diagnostics {
using config_t = common::config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* software_diagnostics */

namespace administrator_commissioning {
using config_t = common::config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* administrator_commissioning */

namespace operational_credentials {
using config_t = common::config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* operational_credentials */

namespace group_key_management {
using config_t = common::config_t;
cluster_t *create(endpoint_t *endpoint, uint8_t flags);
} /* group_key_management */

namespace wifi_network_diagnotics {
using config_t = common::config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* wifi_network_diagnotics */

namespace thread_network_diagnostics {
using config_t = common::config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* thread_network_diagnostics */

namespace ethernet_network_diagnostics {
using config_t = common::config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* ethernet_network_diagnostics */

namespace time_synchronization {
typedef struct config {
    void *delegate;
    config() : delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* time_synchronization */

namespace unit_localization {
typedef struct config {
    feature::temperature_unit::config_t temperature_unit;
    // Empty config for API consistency
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* unit_localization */

namespace bridged_device_basic_information {
typedef struct config {
    bool reachable;
    config() : reachable(true) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* bridged_device_basic_information */

namespace power_source {
typedef struct config {
    uint8_t status;
    uint8_t order;
    char description[k_max_description_length + 1];
    feature::wired::config_t wired;
    feature::battery::config_t battery;
    feature::rechargeable::config_t rechargeable;
    feature::replaceable::config_t replaceable;
	config() : status(0), order(0), description{0} {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* power_source */

namespace icd_management {
typedef struct config {
    feature::user_active_mode_trigger::config_t user_active_mode_trigger;
    // Empty config for API consistency
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* icd_management */

namespace user_label {
using config_t = common::config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* user_label */

namespace fixed_label {
using config_t = common::config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* fixed_label */

namespace identify {
typedef struct config {
    uint16_t identify_time;
    uint8_t identify_type;
    config() : identify_time(0), identify_type(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* identify */

namespace groups {
using config_t = common::config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
uint8_t get_server_cluster_count();
} /* groups */

namespace scenes_management {
typedef struct config {
    uint16_t scene_table_size;
    config() : scene_table_size(16) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* scenes_management */

namespace on_off {
typedef struct config {
    bool on_off;
    feature::lighting::config_t lighting;
    config() : on_off(false) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* on_off */

namespace level_control {
typedef struct config {
    nullable<uint8_t> current_level;
    nullable<uint8_t> on_level;
    uint8_t options;
    feature::lighting::config_t lighting;
    config() : current_level(), on_level(), options(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* level_control */

namespace color_control {
typedef struct config {
    uint8_t color_mode;
    uint8_t color_control_options;
    uint8_t enhanced_color_mode;
    uint16_t color_capabilities;
    nullable<uint8_t> number_of_primaries;
    feature::hue_saturation::config_t hue_saturation;
    feature::color_temperature::config_t color_temperature;
    feature::xy::config_t xy;
    feature::enhanced_hue::config_t enhanced_hue;
    feature::color_loop::config_t color_loop;
    config() : color_mode(1), color_control_options(0), enhanced_color_mode(1),
               color_capabilities(0), number_of_primaries(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* color_control */

namespace fan_control {
typedef struct config {
    uint8_t fan_mode;
    uint8_t fan_mode_sequence;
    nullable<uint8_t> percent_setting;
    uint8_t percent_current;
    void *delegate;
    config() : fan_mode(0), fan_mode_sequence(2), percent_setting(0), percent_current(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* fan_control */

namespace thermostat {
typedef struct config {
    nullable<int16_t> local_temperature;
    uint8_t control_sequence_of_operation;
    uint8_t system_mode;
    feature::heating::config_t heating;
    feature::cooling::config_t cooling;
    feature::occupancy::config_t occupancy;
    feature::setback::config_t setback;
    feature::schedule_configuration::config_t schedule_configuration;
    feature::auto_mode::config_t auto_mode;
    feature::local_temperature_not_exposed::config_t local_temperature_not_exposed;
    config() : local_temperature(), control_sequence_of_operation(4), system_mode(1) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* thermostat */

namespace thermostat_user_interface_configuration {
typedef struct config {
    uint8_t temperature_display_mode;
    uint8_t keypad_lockout;
    config() : temperature_display_mode(0), keypad_lockout(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* thermostat_user_interface_configuration */

namespace air_quality {
using config_t = common::config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* air_quality */

namespace hepa_filter_monitoring {
typedef struct config {
    void *delegate;
    config() : delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* hepa_filter_monitoring */

namespace activated_carbon_filter_monitoring {
typedef struct config {
    void *delegate;
    config() : delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* activated_carbon_filter_monitoring */

namespace carbon_monoxide_concentration_measurement {
typedef struct config {
    uint8_t measurement_medium;
    config() : measurement_medium(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* carbon_monoxide_concentration_measurement */

namespace carbon_dioxide_concentration_measurement {
typedef struct config {
    uint8_t measurement_medium;
    config() : measurement_medium(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* carbon_dioxide_concentration_measurement */

namespace nitrogen_dioxide_concentration_measurement {
typedef struct config {
    uint8_t measurement_medium;
    config() : measurement_medium(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* nitrogen_dioxide_concentration_measurement */

namespace ozone_concentration_measurement {
typedef struct config {
    uint8_t measurement_medium;
    config() : measurement_medium(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* ozone_concentration_measurement */

namespace formaldehyde_concentration_measurement {
typedef struct config {
    uint8_t measurement_medium;
    config() : measurement_medium(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* formaldehyde_concentration_measurement */

namespace pm1_concentration_measurement {
typedef struct config {
    uint8_t measurement_medium;
    config() : measurement_medium(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* pm1_concentration_measurement */

namespace pm25_concentration_measurement {
typedef struct config {
    uint8_t measurement_medium;
    config() : measurement_medium(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* pm25_concentration_measurement */

namespace pm10_concentration_measurement {
typedef struct config {
    uint8_t measurement_medium;
    config() : measurement_medium(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* pm10_concentration_measurement */

namespace radon_concentration_measurement {
typedef struct config {
    uint8_t measurement_medium;
    config() : measurement_medium(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* radon_concentration_measurement */

namespace total_volatile_organic_compounds_concentration_measurement {
typedef struct config {
    uint8_t measurement_medium;
    config() : measurement_medium(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* total_volatile_organic_compounds_concentration_measurement */

namespace operational_state {
typedef struct config {
    void *delegate;
    config() : delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* operational_state */

namespace laundry_washer_mode {
typedef struct config {
    uint8_t current_mode;
    void *delegate;
    config() : current_mode(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* laundry_washer_mode */

namespace laundry_washer_controls {
typedef struct config {
    feature::spin::config_t spin;
    feature::rinse::config_t rinse;
    void *delegate;
    config() : delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* laundry_washer_controls */

namespace laundry_dryer_controls {
typedef struct config {
    nullable<uint8_t> selected_dryness_level;
    void *delegate;
    config() : selected_dryness_level(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* laundry_dryer_controls */

namespace dish_washer_mode {
typedef struct config {
    uint8_t current_mode;
    void *delegate;
    config() : current_mode(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* dish_washer_mode */

namespace dish_washer_alarm {
typedef struct config {
    void *delegate;
    config() : delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* dish_washer_alarm */

namespace smoke_co_alarm {
using config_t = common::config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* smoke_co_alarm */

namespace door_lock {
typedef struct config {
    nullable<uint8_t> lock_state;
    uint8_t lock_type;
    bool actuator_enabled;
    uint8_t operating_mode;
    uint16_t supported_operating_modes;
    void *delegate;
    config() : lock_state(0), lock_type(0), actuator_enabled(0), operating_mode(0), supported_operating_modes(0xFFF6), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* door_lock */

namespace window_covering {
typedef struct config {
    uint8_t type;
    uint8_t config_status;
    uint8_t operational_status;
    const uint8_t end_product_type;
    uint8_t mode;
    feature::lift::config_t lift;
    void *delegate;
    config(uint8_t end_product_type = 0) : type(0), config_status(0), operational_status(0), end_product_type(end_product_type), mode(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* window_covering */

namespace switch_cluster {
typedef struct config {
    uint8_t number_of_positions;
    uint8_t current_position;
    config() : number_of_positions(2), current_position(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* switch_cluster */

namespace temperature_measurement {
typedef struct config {
    nullable<int16_t> measured_value;
    nullable<int16_t> min_measured_value;
    nullable<int16_t> max_measured_value;
    config() : measured_value(), min_measured_value(), max_measured_value() {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* temperature_measurement */

namespace relative_humidity_measurement {
typedef struct config {
    nullable<uint16_t> measured_value;
    nullable<uint16_t> min_measured_value;
    nullable<uint16_t> max_measured_value;
    config() : measured_value(), min_measured_value(), max_measured_value() {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* relative_humidity_measurement */

namespace occupancy_sensing {
typedef struct config {
    uint8_t occupancy;
    uint8_t occupancy_sensor_type;
    uint8_t occupancy_sensor_type_bitmap;
    uint32_t features;
    config() : occupancy(0), occupancy_sensor_type(0),
               occupancy_sensor_type_bitmap(0), features(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* occupancy_sensing */

namespace boolean_state {
typedef struct config {
    bool state_value;
    config() : state_value(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* boolean_state */

namespace boolean_state_configuration {
typedef struct config {
    feature::visual::config_t visual;
    feature::audible::config_t audible;
    feature::alarm_suppress::config_t alarm_suppress;
    feature::sensitivity_level::config_t sensitivity_level;
    void *delegate;
    config() : delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* boolean_state */

namespace localization_configuration {
typedef struct config {
    char active_locale[k_max_active_locale_length + 1];
    config() : active_locale{0} {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* localization_configuration_cluster */

namespace time_format_localization {
typedef struct config {
    uint8_t hour_format;
    feature::calendar_format::config_t calendar_format;
    config() : hour_format(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* time_format_localization */

namespace illuminance_measurement {
typedef struct config {
    nullable<uint16_t> illuminance_measured_value;
    nullable<uint16_t> illuminance_min_measured_value;
    nullable<uint16_t> illuminance_max_measured_value;
    config() : illuminance_measured_value(0), illuminance_min_measured_value(), illuminance_max_measured_value() {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* illuminance_measurement */

namespace pressure_measurement {
typedef struct config {
    nullable<int16_t> pressure_measured_value;
    nullable<int16_t> pressure_min_measured_value;
    nullable<int16_t> pressure_max_measured_value;
    config() : pressure_measured_value(), pressure_min_measured_value(), pressure_max_measured_value() {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* pressure_measurement */

namespace flow_measurement {
typedef struct config {
    nullable<uint16_t> flow_measured_value;
    nullable<uint16_t> flow_min_measured_value;
    nullable<uint16_t> flow_max_measured_value;
    config() : flow_measured_value(), flow_min_measured_value(), flow_max_measured_value() {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* flow_measurement */

namespace pump_configuration_and_control {
typedef struct config {
    // Pump Information Attributes
    const nullable<int16_t> max_pressure;
    const nullable<uint16_t> max_speed;
    const nullable<uint16_t> max_flow;
    // Pump Dynamic Information Attributes
    uint8_t effective_operation_mode;
    uint8_t effective_control_mode;
    nullable<int16_t> capacity;
    // Pump Settings Attributes
    uint8_t operation_mode;
    feature::constant_pressure::config_t constant_pressure;
    feature::compensated_pressure::config_t compensated_pressure;
    feature::constant_flow::config_t constant_flow;
    feature::constant_speed::config_t constant_speed;
    feature::constant_temperature::config_t constant_temperature;
    uint32_t features;
    config(
        nullable<int16_t> max_pressure = nullable<int16_t>(),
        nullable<uint16_t> max_speed = nullable<uint16_t>(),
        nullable<uint16_t> max_flow = nullable<uint16_t>()
    ) : max_pressure(max_pressure), max_speed(max_speed), max_flow(max_flow),
        effective_operation_mode(0), effective_control_mode(0), capacity(), operation_mode(0), features(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* pump_configuration_and_control */

namespace mode_select {
typedef struct config {
    char mode_select_description[k_max_mode_select_description_length + 1];
    const nullable<uint16_t> standard_namespace;
    uint8_t current_mode;
    feature::on_off::config_t on_off;
    void *delegate;
    config() : mode_select_description{0}, standard_namespace(), current_mode(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* mode_select */

namespace temperature_control {
typedef struct config {
    feature::temperature_number::config_t temperature_number;
    feature::temperature_level::config_t temperature_level;
    feature::temperature_step::config_t temperature_step;
    uint32_t features;
    config() : features(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* temperature_control */

namespace refrigerator_alarm {
typedef struct config {
    uint32_t mask;
    uint32_t state;
    uint32_t supported;
    config() : mask(1), state(0), supported(1) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* refrigerator_alarm */

namespace refrigerator_and_tcc_mode {
typedef struct config {
    uint8_t current_mode;
    void *delegate;
    config() : current_mode(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* refrigerator_and_tcc_mode */

namespace rvc_run_mode {
typedef struct config {
    uint8_t current_mode;
    void *delegate;
    config() : current_mode(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* rvc_run_mode */

namespace rvc_clean_mode {
typedef struct config {
    uint8_t current_mode;
    void *delegate;
    config() : current_mode(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* rvc_clean_mode */

namespace microwave_oven_mode {
typedef struct config {
    uint8_t current_mode;
    void *delegate;
    config() : current_mode(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* microwave_oven_mode */

namespace microwave_oven_control {
typedef struct config {
    void *delegate;
    config() : delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* microwave_oven_control */

namespace rvc_operational_state {
using config_t = common::config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* rvc_operational_state */

namespace keypad_input {
typedef struct config {
    void *delegate;
    config() : delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* keypad_input */

namespace power_topology {
typedef struct config {
    void *delegate;
    config() : delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* power_topology */

namespace electrical_power_measurement {
typedef struct config {
    void *delegate;
    config() : delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* electrical_power_measurement */

namespace electrical_energy_measurement {
typedef struct config {
    void *delegate;
    config() : delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* electrical_energy_measurement */

namespace energy_evse_mode {
typedef struct config {
    uint8_t current_mode;
    void *delegate;
    config() : current_mode(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* energy_evse_mode */

namespace energy_evse {
typedef struct config {
    void *delegate;
    config() : delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* energy_evse */

namespace valve_configuration_and_control {
typedef struct config {
    nullable<uint32_t> open_duration;
    nullable<uint32_t> default_open_duration;
    nullable<uint8_t> current_state;
    nullable<uint8_t> target_state;
    feature::time_sync::config_t time_sync;
    feature::level::config_t level;
    void *delegate;
    config() : open_duration(), default_open_duration(), current_state(), target_state(), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* valve_configuration_and_control */

namespace device_energy_management {
typedef struct config {
    void *delegate;
    config() : delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* device_energy_management */

namespace device_energy_management_mode {
typedef struct config {
    uint8_t current_mode;
    void *delegate;
    config() : current_mode(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* device_energy_management_mode */

namespace application_basic {
typedef struct config {
    void *delegate;
    config() : delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* application_basic */

namespace thread_border_router_management {
typedef struct config {
    void *delegate;
    config() : delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* thread_border_router_management */

namespace wifi_network_management {
using config_t = common::config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* wifi_network_management */

namespace thread_network_directory {
using config_t = common::config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* thread_network_directory */

namespace service_area {
typedef struct config {
    void *delegate;
    config() : delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* service_area */

namespace water_heater_management {
typedef struct config {
    uint8_t heater_types;
    uint8_t heat_demand;
    uint8_t boost_state;
    void *delegate;
    feature::energy_management::config_t energy_management;
    feature::tank_percent::config_t tank_percent;
    config() : heater_types(0), heat_demand(0), boost_state(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* water_heater_management */

namespace water_heater_mode {
typedef struct config {
    uint8_t current_mode;
    void *delegate;
    config() : current_mode(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* water_heater_mode */

namespace energy_preference {
typedef struct config {
    feature::energy_balance::config_t energy_balance;
    feature::low_power_mode_sensitivity::config_t low_power_mode_sensitivity;
    void *delegate;
    config() : delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags, uint32_t features);
} /* energy_preference */

namespace commissioner_control {
typedef struct config {
    uint32_t supported_device_categories;
    void *delegate;
    config() : supported_device_categories(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* commissioner_control */

namespace ecosystem_information {
using config_t = common::config_t;
cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);
} /* ecosystem_information */

} /* cluster */
} /* esp_matter */
