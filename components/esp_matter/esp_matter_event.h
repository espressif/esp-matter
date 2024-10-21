// Copyright 2022 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_err.h>
#include <esp_matter.h>
#include <platform/DeviceControlServer.h>

namespace esp_matter {
namespace cluster {

/** Specific event send APIs
 *
 * If some standard event is not present here, it can be added.
 */

namespace access_control {
namespace event {
event_t *create_access_control_entry_changed(cluster_t *cluster);
event_t *create_access_control_extension_changed(cluster_t *cluster);
event_t *create_fabric_restriction_review_update(cluster_t *cluster);
} // namespace event
} // namespace access_control

namespace actions {
namespace event {
event_t *create_state_changed(cluster_t *cluster);
event_t *create_action_failed(cluster_t *cluster);
} // namespace event
} // namespace actions

namespace bridged_device_basic_information {
namespace event {
event_t *create_active_changed(cluster_t *cluster);
} // namespace event
} // namespace bridged_device_basic_information

namespace basic_information {
namespace event {
event_t *create_start_up(cluster_t *cluster);
event_t *create_shut_down(cluster_t *cluster);
event_t *create_leave(cluster_t *cluster);
event_t *create_reachable_changed(cluster_t *cluster);
} // namespace event
} // namespace basic_information

namespace ota_requestor {
namespace event {
event_t *create_state_transition(cluster_t *cluster);
event_t *create_version_applied(cluster_t *cluster);
event_t *create_download_error(cluster_t *cluster);
} // namespace event
} // namespace ota_requestor

namespace general_diagnostics {
namespace event {
event_t *create_hardware_fault_change(cluster_t *cluster);
event_t *create_radio_fault_change(cluster_t *cluster);
event_t *create_network_fault_change(cluster_t *cluster);
event_t *create_boot_reason(cluster_t *cluster);
} // namespace event
} // namespace general_diagnostics

namespace wifi_network_diagnotics {
namespace event {
event_t *create_disconnection(cluster_t *cluster);
event_t *create_association_failure(cluster_t *cluster);
event_t *create_connection_status(cluster_t *cluster);
} // namespace event
} // namespace wifi_network_diagnotics

namespace thread_network_diagnostics {
namespace event {
event_t *create_connection_status(cluster_t *cluster);
event_t *create_network_fault_change(cluster_t *cluster);
} // namespace event
} // namespace thread_network_diagnostics

namespace software_diagnostics {
namespace event {
event_t *create_software_fault(cluster_t *cluster);
} // namespace event
} // namespace software_diagnostics

namespace time_synchronization {
namespace event {
event_t *create_dst_table_empty(cluster_t *cluster);
event_t *create_dst_status(cluster_t *cluster);
event_t *create_time_zone_status(cluster_t *cluster);
event_t *create_time_failure(cluster_t *cluster);
event_t *create_missing_trusted_time_source(cluster_t *cluster);
} // namespace event
} // namespace time_synchronization

namespace bridged_device_basic_information {
namespace event {
event_t *create_start_up(cluster_t *cluster);
event_t *create_shut_down(cluster_t *cluster);
event_t *create_leave(cluster_t *cluster);
event_t *create_reachable_changed(cluster_t *cluster);
} // namespace event
} // namespace bridged_device_basic_information

namespace smoke_co_alarm {
namespace event {
event_t *create_smoke_alarm(cluster_t *cluster);
event_t *create_co_alarm(cluster_t *cluster);
event_t *create_low_battery(cluster_t *cluster);
event_t *create_hardware_fault(cluster_t *cluster);
event_t *create_end_of_service(cluster_t *cluster);
event_t *create_self_test_complete(cluster_t *cluster);
event_t *create_alarm_muted(cluster_t *cluster);
event_t *create_mute_ended(cluster_t *cluster);
event_t *create_interconnect_smoke_alarm(cluster_t *cluster);
event_t *create_interconnect_co_alarm(cluster_t *cluster);
event_t *create_all_clear(cluster_t *cluster);
} // namespace event
} // namespace smoke_co_alarm

namespace door_lock {
namespace event {
event_t *create_door_lock_alarm(cluster_t *cluster);
event_t *create_door_state_change(cluster_t *cluster);
event_t *create_lock_operation(cluster_t *cluster);
event_t *create_lock_operation_error(cluster_t *cluster);
event_t *create_lock_user_change(cluster_t *cluster);
} // namespace event
} // namespace door_lock

namespace switch_cluster {
namespace event {
event_t *create_switch_latched(cluster_t *cluster);
event_t *create_initial_press(cluster_t *cluster);
event_t *create_long_press(cluster_t *cluster);
event_t *create_short_release(cluster_t *cluster);
event_t *create_long_release(cluster_t *cluster);
event_t *create_multi_press_ongoing(cluster_t *cluster);
event_t *create_multi_press_complete(cluster_t *cluster);

esp_err_t send_switch_latched(chip::EndpointId endpoint, uint8_t new_position);
esp_err_t send_initial_press(chip::EndpointId endpoint, uint8_t new_position);
esp_err_t send_long_press(chip::EndpointId endpoint, uint8_t new_position);
esp_err_t send_short_release(chip::EndpointId endpoint, uint8_t previous_position);
esp_err_t send_long_release(chip::EndpointId endpoint, uint8_t previous_position);
esp_err_t send_multi_press_ongoing(chip::EndpointId endpoint, uint8_t new_position, uint8_t count);
esp_err_t send_multi_press_complete(chip::EndpointId endpoint, uint8_t new_position, uint8_t count);
} // namespace event
} // namespace switch_cluster

namespace boolean_state {
namespace event {
event_t *create_state_change(cluster_t *cluster);
} // namespace event
} // namespace boolean_state

namespace boolean_state_configuration {
namespace event {
event_t *create_alarms_state_changed(cluster_t *cluster);
event_t *create_sensor_fault(cluster_t *cluster);
} // namespace event
} // namespace boolean_state_configuration

namespace operational_state {
namespace event {
event_t *create_operational_error(cluster_t *cluster);
event_t *create_operation_completion(cluster_t *cluster);
} // namespace event
} // namespace operational_state

namespace pump_configuration_and_control {
namespace event {
event_t *create_supply_voltage_low(cluster_t *cluster);
event_t *create_supply_voltage_high(cluster_t *cluster);
event_t *create_power_missing_phase(cluster_t *cluster);
event_t *create_system_pressure_low(cluster_t *cluster);
event_t *create_system_pressure_high(cluster_t *cluster);
event_t *create_dry_running(cluster_t *cluster);
event_t *create_motor_temperature_high(cluster_t *cluster);
event_t *create_pump_motor_fatal_failure(cluster_t *cluster);
event_t *create_electronic_temperature_high(cluster_t *cluster);
event_t *create_pump_blocked(cluster_t *cluster);
event_t *create_sensor_failure(cluster_t *cluster);
event_t *create_electronic_non_fatal_failure(cluster_t *cluster);
event_t *create_electronic_fatal_failure(cluster_t *cluster);
event_t *create_general_fault(cluster_t *cluster);
event_t *create_leakage(cluster_t *cluster);
event_t *create_air_detection(cluster_t *cluster);
event_t *create_turbine_operation(cluster_t *cluster);
} // namespace event
} // namespace pump_configuration_and_control

namespace electrical_power_measurement {
namespace event {
event_t *create_measurement_period_ranges(cluster_t *cluster);
} // namespace event
} // namespace electrical_power_measurement

namespace electrical_energy_measurement {
namespace event {
event_t *create_cumulative_energy_measured(cluster_t *cluster);
event_t *create_periodic_energy_measured(cluster_t *cluster);
} // namespace event
} // namespace electrical_energy_measurement

namespace valve_configuration_and_control {
namespace event {
event_t *create_valve_state_changed(cluster_t *cluster);
event_t *create_valve_fault(cluster_t *cluster);
} // namespace event
} // namespace valve_configuration_and_control

namespace device_energy_management {
namespace event {
event_t *create_power_adjust_start(cluster_t *cluster);
event_t *create_power_adjust_end(cluster_t *cluster);
event_t *create_paused(cluster_t *cluster);
event_t *create_resumed(cluster_t *cluster);
} // namespace event
} // namespace device_energy_management

namespace water_heater_management {
namespace event {
event_t *create_boost_started(cluster_t *cluster);
event_t *create_boost_ended(cluster_t *cluster);
} // namespace event
} // namespace water_heater_management

namespace commissioner_control {
namespace event {
event_t *create_commissioning_request_result(cluster_t *cluster);
} // namespace event
} // namespace commissioner_control

namespace occupancy_sensing {
namespace event {
event_t *create_occupancy_changed(cluster_t *cluster);
} // namespace event
} // namespace occupancy_sensing

} // namespace cluster
} // namespace esp_matter
