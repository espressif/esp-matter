// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
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

namespace esp_matter {
namespace cluster {

namespace descriptor {
constexpr uint16_t cluster_revision = 2;
} // namespace descriptor

namespace actions {
constexpr uint16_t cluster_revision = 1;
} // namespace actions

namespace access_control {
constexpr uint16_t cluster_revision = 2;
} // namespace access_control

namespace basic_information {
constexpr uint16_t cluster_revision = 4;
} // namespace basic_information

namespace binding {
constexpr uint16_t cluster_revision = 1;
} // namespace binding

namespace ota_provider {
constexpr uint16_t cluster_revision = 1;
} // namespace ota_provider

namespace ota_requestor {
constexpr uint16_t cluster_revision = 1;
} // namespace ota_requestor

namespace general_commissioning {
constexpr uint16_t cluster_revision = 2;
} // namespace general_commissioning

namespace network_commissioning {
constexpr uint16_t cluster_revision = 2;
} // namespace network_commissioning

namespace diagnostic_logs {
constexpr uint16_t cluster_revision = 1;
} // namespace diagnostic_logs

namespace general_diagnostics {
constexpr uint16_t cluster_revision = 2;
} // namespace general_diagnostics

namespace software_diagnostics {
constexpr uint16_t cluster_revision = 1;
} // namespace software_diagnostics

namespace administrator_commissioning {
constexpr uint16_t cluster_revision = 1;
} // namespace administrator_commissioning

namespace operational_credentials {
constexpr uint16_t cluster_revision = 1;
} // namespace operational_credentials

namespace group_key_management {
constexpr uint16_t cluster_revision = 2;
} // namespace group_key_management

namespace wifi_network_diagnotics {
constexpr uint16_t cluster_revision = 1;
} // namespace wifi_network_diagnotics

namespace thread_network_diagnostics {
constexpr uint16_t cluster_revision = 3;
} // namespace thread_network_diagnostics

namespace ethernet_network_diagnostics {
constexpr uint16_t cluster_revision = 1;
} // namespace ethernet_network_diagnostics

namespace time_synchronization {
constexpr uint16_t cluster_revision = 2;
} // namespace time_synchronization

namespace unit_localization {
constexpr uint16_t cluster_revision = 1;
} // namespace unit_localization

namespace bridged_device_basic_information {
constexpr uint16_t cluster_revision = 4;
} // namespace bridged_device_basic_information

namespace power_source {
constexpr uint16_t cluster_revision = 3;
} // namespace power_source

namespace icd_management {
constexpr uint16_t cluster_revision = 2;
} // namespace icd_management

namespace user_label {
constexpr uint16_t cluster_revision = 1;
} // namespace user_label

namespace fixed_label {
constexpr uint16_t cluster_revision = 1;
} // namespace fixed_label

namespace identify {
constexpr uint16_t cluster_revision = 5;
} // namespace identify

namespace groups {
constexpr uint16_t cluster_revision = 4;
} // namespace groups

namespace scenes_management {
constexpr uint16_t cluster_revision = 1;
} // namespace scenes_management

namespace on_off {
constexpr uint16_t cluster_revision = 6;
} // namespace on_off

namespace level_control {
constexpr uint16_t cluster_revision = 6;
} // namespace level_control

namespace color_control {
constexpr uint16_t cluster_revision = 7;
} // namespace color_control

namespace fan_control {
constexpr uint16_t cluster_revision = 4;
} // namespace fan_control

namespace thermostat {
constexpr uint16_t cluster_revision = 8;
} // namespace thermostat

namespace thermostat_user_interface_configuration {
constexpr uint16_t cluster_revision = 2;
} // namespace thermostat_user_interface_configuration

namespace air_quality {
constexpr uint16_t cluster_revision = 1;
} // namespace air_quality

namespace hepa_filter_monitoring {
constexpr uint16_t cluster_revision = 1;
} // namespace hepa_filter_monitoring

namespace activated_carbon_filter_monitoring {
constexpr uint16_t cluster_revision = 1;
} // namespace activated_carbon_filter_monitoring

namespace carbon_monoxide_concentration_measurement {
constexpr uint16_t cluster_revision = 3;
} // namespace carbon_monoxide_concentration_measurement

namespace carbon_dioxide_concentration_measurement {
constexpr uint16_t cluster_revision = 3;
} // namespace carbon_dioxide_concentration_measurement

namespace nitrogen_dioxide_concentration_measurement {
constexpr uint16_t cluster_revision = 3;
} // namespace nitrogen_dioxide_concentration_measurement

namespace ozone_concentration_measurement {
constexpr uint16_t cluster_revision = 3;
} // namespace ozone_concentration_measurement

namespace formaldehyde_concentration_measurement {
constexpr uint16_t cluster_revision = 3;
} // namespace formaldehyde_concentration_measurement

namespace pm1_concentration_measurement {
constexpr uint16_t cluster_revision = 3;
} // namespace pm1_concentration_measurement

namespace pm25_concentration_measurement {
constexpr uint16_t cluster_revision = 3;
} // namespace pm25_concentration_measurement

namespace pm10_concentration_measurement {
constexpr uint16_t cluster_revision = 3;
} // namespace pm10_concentration_measurement

namespace radon_concentration_measurement {
constexpr uint16_t cluster_revision = 3;
} // namespace radon_concentration_measurement

namespace total_volatile_organic_compounds_concentration_measurement {
constexpr uint16_t cluster_revision = 3;
} // namespace total_volatile_organic_compounds_concentration_measurement

namespace operational_state {
constexpr uint16_t cluster_revision = 2;
} // namespace operational_state

namespace laundry_washer_mode {
constexpr uint16_t cluster_revision = 3;
} // namespace laundry_washer_mode

namespace laundry_washer_controls {
constexpr uint16_t cluster_revision = 2;
} // namespace laundry_washer_controls

namespace laundry_dryer_controls {
constexpr uint16_t cluster_revision = 1;
} // namespace laundry_dryer_controls

namespace dish_washer_mode {
constexpr uint16_t cluster_revision = 3;
} // namespace dish_washer_mode

namespace dish_washer_alarm {
constexpr uint16_t cluster_revision = 1;
} // namespace dish_washer_alarm

namespace smoke_co_alarm {
constexpr uint16_t cluster_revision = 1;
} // namespace smoke_co_alarm

namespace door_lock {
constexpr uint16_t cluster_revision = 8;
} // namespace door_lock

namespace window_covering {
constexpr uint16_t cluster_revision = 5;
} // namespace window_covering

namespace switch_cluster {
constexpr uint16_t cluster_revision = 2;
} // namespace switch_cluster

namespace temperature_measurement {
constexpr uint16_t cluster_revision = 4;
} // namespace temperature_measurement

namespace relative_humidity_measurement {
constexpr uint16_t cluster_revision = 3;
} // namespace relative_humidity_measurement

namespace occupancy_sensing {
constexpr uint16_t cluster_revision = 5;
} // namespace occupancy_sensing

namespace boolean_state {
constexpr uint16_t cluster_revision = 1;
} // namespace boolean_state

namespace boolean_state_configuration {
constexpr uint16_t cluster_revision = 1;
} // namespace boolean_state_configuration

namespace localization_configuration {
constexpr uint16_t cluster_revision = 4;
} // namespace localization_configuration

namespace time_format_localization {
constexpr uint16_t cluster_revision = 4;
} // namespace time_format_localization

namespace illuminance_measurement {
constexpr uint16_t cluster_revision = 3;
} // namespace illuminance_measurement

namespace pressure_measurement {
constexpr uint16_t cluster_revision = 3;
} // namespace pressure_measurement

namespace flow_measurement {
constexpr uint16_t cluster_revision = 3;
} // namespace flow_measurement

namespace pump_configuration_and_control {
constexpr uint16_t cluster_revision = 3;
} // namespace pump_configuration_and_control

namespace mode_select {
constexpr uint16_t cluster_revision = 2;
} // namespace mode_select

namespace temperature_control {
constexpr uint16_t cluster_revision = 1;
} // namespace temperature_control

namespace refrigerator_alarm {
constexpr uint16_t cluster_revision = 1;
} // namespace refrigerator_alarm

namespace refrigerator_and_tcc_mode {
constexpr uint16_t cluster_revision = 3;
} // namespace refrigerator_and_tcc_mode

namespace rvc_run_mode {
constexpr uint16_t cluster_revision = 3;
} // namespace rvc_run_mode

namespace rvc_clean_mode {
constexpr uint16_t cluster_revision = 3;
} // namespace rvc_clean_mode

namespace water_heater_mode {
constexpr uint16_t cluster_revision = 1;
} // namespace water_heater_mode

namespace microwave_oven_mode {
constexpr uint16_t cluster_revision = 1;
} // namespace microwave_oven_mode

namespace microwave_oven_control {
constexpr uint16_t cluster_revision = 1;
} // namespace microwave_oven_control

namespace rvc_operational_state {
constexpr uint16_t cluster_revision = 2;
} // namespace rvc_operational_state

namespace keypad_input {
constexpr uint16_t cluster_revision = 1;
} // namespace keypad_input

namespace power_topology {
constexpr uint16_t cluster_revision = 1;
} // namespace power_topology

namespace electrical_power_measurement {
constexpr uint16_t cluster_revision = 1;
} // namespace electrical_power_measurement

namespace electrical_energy_measurement {
constexpr uint16_t cluster_revision = 1;
} // namespace electrical_energy_measurement

namespace energy_evse_mode {
constexpr uint16_t cluster_revision = 2;
} // namespace energy_evse_mode

namespace energy_evse {
constexpr uint16_t cluster_revision = 3;
} // namespace energy_evse

namespace valve_configuration_and_control {
constexpr uint16_t cluster_revision = 1;
} // namespace valve_configuration_and_control

namespace device_energy_management {
constexpr uint16_t cluster_revision = 4;
} // namespace device_energy_management

namespace device_energy_management_mode {
constexpr uint16_t cluster_revision = 2;
} // namespace device_energy_management_mode

namespace application_basic {
constexpr uint16_t cluster_revision = 1;
} // namespace application_basic

namespace thread_border_router_management {
constexpr uint16_t cluster_revision = 1;
} // namespace thread_border_router_management

namespace wifi_network_management {
constexpr uint16_t cluster_revision = 1;
} // namespace wifi_network_management

namespace thread_network_directory {
constexpr uint16_t cluster_revision = 1;
} // namespace thread_network_directory

namespace service_area {
constexpr uint16_t cluster_revision = 1;
} // namespace service_area

namespace water_heater_management {
constexpr uint16_t cluster_revision = 2;
} // namespace water_heater_management

namespace energy_preference {
constexpr uint16_t cluster_revision = 1;
} // namespace energy_preference

namespace commissioner_control {
constexpr uint16_t cluster_revision = 1;
} // namespace commissioner_control

namespace ecosystem_information {
constexpr uint16_t cluster_revision = 1;
} // namespace ecosystem_information

} // namespace cluster
} // namespace esp_matter

