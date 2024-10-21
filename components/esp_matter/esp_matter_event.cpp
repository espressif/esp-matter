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

#include <esp_log.h>
#include <esp_matter_event.h>

#include <app/clusters/switch-server/switch-server.h>
#include <platform/DeviceControlServer.h>

using chip::EndpointId;
using chip::DeviceLayer::DeviceControlServer;
using namespace chip::app::Clusters;

namespace esp_matter {
namespace cluster {
namespace access_control {
namespace event {
event_t *create_access_control_entry_changed(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, AccessControl::Events::AccessControlEntryChanged::Id);
}

event_t *create_access_control_extension_changed(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, AccessControl::Events::AccessControlExtensionChanged::Id);
}

event_t *create_fabric_restriction_review_update(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, AccessControl::Events::FabricRestrictionReviewUpdate::Id);
}
} // namespace event
} // namespace access_control

namespace bridged_device_basic_information {
namespace event {
event_t *create_active_changed(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, BridgedDeviceBasicInformation::Events::ActiveChanged::Id);
}
} // namespace event
} // namespace bridged_device_basic_information

namespace actions {
namespace event {
event_t *create_state_changed(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, Actions::Events::StateChanged::Id);
}

event_t *create_action_failed(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, Actions::Events::ActionFailed::Id);
}

esp_err_t send_state_changed(EndpointId endpoint, uint16_t action_id, uint32_t invoke_id, uint8_t action_state)
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_action_failed(EndpointId endpoint, uint16_t action_id, uint32_t invoke_id, uint8_t action_state,
                             uint8_t error)
{
    /* Not implemented */
    return ESP_OK;
}

} // namespace event
} // namespace actions

namespace basic_information {
namespace event {
event_t *create_start_up(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, BasicInformation::Events::StartUp::Id);
}

event_t *create_shut_down(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, BasicInformation::Events::ShutDown::Id);
}

event_t *create_leave(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, BasicInformation::Events::Leave::Id);
}

event_t *create_reachable_changed(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, BasicInformation::Events::ReachableChanged::Id);
}

} // namespace event
} // namespace basic_information

namespace ota_requestor {
namespace event {
event_t *create_state_transition(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, OtaSoftwareUpdateRequestor::Events::StateTransition::Id);
}

event_t *create_version_applied(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, OtaSoftwareUpdateRequestor::Events::VersionApplied::Id);
}

event_t *create_download_error(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, OtaSoftwareUpdateRequestor::Events::DownloadError::Id);
}

} // namespace event
} // namespace ota_requestor

namespace general_diagnostics {
namespace event {
event_t *create_hardware_fault_change(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, GeneralDiagnostics::Events::HardwareFaultChange::Id);
}

event_t *create_radio_fault_change(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, GeneralDiagnostics::Events::RadioFaultChange::Id);
}

event_t *create_network_fault_change(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, GeneralDiagnostics::Events::NetworkFaultChange::Id);
}

event_t *create_boot_reason(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, GeneralDiagnostics::Events::BootReason::Id);
}

} // namespace event
} // namespace general_diagnostics

namespace wifi_network_diagnotics {
namespace event {
event_t *create_disconnection(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, WiFiNetworkDiagnostics::Events::Disconnection::Id);
}

event_t *create_association_failure(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, WiFiNetworkDiagnostics::Events::AssociationFailure::Id);
}

event_t *create_connection_status(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, WiFiNetworkDiagnostics::Events::ConnectionStatus::Id);
}

} // namespace event
} // namespace wifi_network_diagnotics

namespace thread_network_diagnostics {
namespace event {
event_t *create_connection_status(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, ThreadNetworkDiagnostics::Events::ConnectionStatus::Id);
}

event_t *create_network_fault_change(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, ThreadNetworkDiagnostics::Events::NetworkFaultChange::Id);
}

} // namespace event
} // namespace thread_network_diagnostics

namespace software_diagnostics {
namespace event {
event_t *create_software_fault(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, SoftwareDiagnostics::Events::SoftwareFault::Id);
}
} // namespace event
} // namespace software_diagnostics

namespace time_synchronization {
namespace event {
event_t *create_dst_table_empty(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, TimeSynchronization::Events::DSTTableEmpty::Id);
}

event_t *create_dst_status(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, TimeSynchronization::Events::DSTStatus::Id);
}

event_t *create_time_zone_status(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, TimeSynchronization::Events::TimeZoneStatus::Id);
}

event_t *create_time_failure(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, TimeSynchronization::Events::TimeFailure::Id);
}

event_t *create_missing_trusted_time_source(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, TimeSynchronization::Events::MissingTrustedTimeSource::Id);
}

} // namespace event
} // namespace time_synchronization

namespace bridged_device_basic_information {
namespace event {

event_t *create_start_up(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, BridgedDeviceBasicInformation::Events::StartUp::Id);
}

event_t *create_shut_down(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, BridgedDeviceBasicInformation::Events::ShutDown::Id);
}

event_t *create_leave(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, BridgedDeviceBasicInformation::Events::Leave::Id);
}

event_t *create_reachable_changed(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, BridgedDeviceBasicInformation::Events::ReachableChanged::Id);
}

} // namespace event
} // namespace bridged_device_basic_information

namespace smoke_co_alarm {
namespace event {

event_t *create_smoke_alarm(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, SmokeCoAlarm::Events::SmokeAlarm::Id);
}

event_t *create_co_alarm(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, SmokeCoAlarm::Events::COAlarm::Id);
}

event_t *create_low_battery(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, SmokeCoAlarm::Events::LowBattery::Id);
}

event_t *create_hardware_fault(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, SmokeCoAlarm::Events::HardwareFault::Id);
}

event_t *create_end_of_service(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, SmokeCoAlarm::Events::EndOfService::Id);
}

event_t *create_self_test_complete(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, SmokeCoAlarm::Events::SelfTestComplete::Id);
}

event_t *create_alarm_muted(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, SmokeCoAlarm::Events::AlarmMuted::Id);
}

event_t *create_mute_ended(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, SmokeCoAlarm::Events::MuteEnded::Id);
}

event_t *create_interconnect_smoke_alarm(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, SmokeCoAlarm::Events::InterconnectSmokeAlarm::Id);
}

event_t *create_interconnect_co_alarm(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, SmokeCoAlarm::Events::InterconnectCOAlarm::Id);
}

event_t *create_all_clear(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, SmokeCoAlarm::Events::AllClear::Id);
}

} // namespace event
} // namespace smoke_co_alarm

namespace door_lock {
namespace event {

event_t *create_door_lock_alarm(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, DoorLock::Events::DoorLockAlarm::Id);
}

event_t *create_door_state_change(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, DoorLock::Events::DoorStateChange::Id);
}

event_t *create_lock_operation(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, DoorLock::Events::LockOperation::Id);
}

event_t *create_lock_operation_error(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, DoorLock::Events::LockOperationError::Id);
}

event_t *create_lock_user_change(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, DoorLock::Events::LockUserChange::Id);
}

esp_err_t send_door_lock_alarm()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_door_state_change()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_lock_operation()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_lock_operation_error()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_lock_user_change()
{
    /* Not implemented */
    return ESP_OK;
}

} // namespace event
} // namespace door_lock

namespace switch_cluster {
namespace event {

event_t *create_switch_latched(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, Switch::Events::SwitchLatched::Id);
}

event_t *create_initial_press(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, Switch::Events::InitialPress::Id);
}

event_t *create_long_press(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, Switch::Events::LongPress::Id);
}

event_t *create_short_release(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, Switch::Events::ShortRelease::Id);
}

event_t *create_long_release(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, Switch::Events::LongRelease::Id);
}

event_t *create_multi_press_ongoing(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, Switch::Events::MultiPressOngoing::Id);
}

event_t *create_multi_press_complete(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, Switch::Events::MultiPressComplete::Id);
}

esp_err_t send_switch_latched(EndpointId endpoint, uint8_t new_position)
{
    SwitchServer::Instance().OnSwitchLatch(endpoint, new_position);
    return ESP_OK;
}

esp_err_t send_initial_press(EndpointId endpoint, uint8_t new_position)
{
    SwitchServer::Instance().OnInitialPress(endpoint, new_position);
    return ESP_OK;
}

esp_err_t send_long_press(EndpointId endpoint, uint8_t new_position)
{
    SwitchServer::Instance().OnLongPress(endpoint, new_position);
    return ESP_OK;
}

esp_err_t send_short_release(EndpointId endpoint, uint8_t previous_position)
{
    SwitchServer::Instance().OnShortRelease(endpoint, previous_position);
    return ESP_OK;
}

esp_err_t send_long_release(EndpointId endpoint, uint8_t previous_position)
{
    SwitchServer::Instance().OnLongRelease(endpoint, previous_position);
    return ESP_OK;
}

esp_err_t send_multi_press_ongoing(EndpointId endpoint, uint8_t new_position, uint8_t count)
{
    SwitchServer::Instance().OnMultiPressOngoing(endpoint, new_position, count);
    return ESP_OK;
}

esp_err_t send_multi_press_complete(EndpointId endpoint, uint8_t new_position, uint8_t count)
{
    SwitchServer::Instance().OnMultiPressComplete(endpoint, new_position, count);
    return ESP_OK;
}

} // namespace event
} // namespace switch_cluster

namespace boolean_state {
namespace event {

event_t *create_state_change(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, BooleanState::Events::StateChange::Id);
}

} // namespace event
} // namespace boolean_state

namespace boolean_state_configuration {
namespace event {

event_t *create_alarms_state_changed(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, BooleanStateConfiguration::Events::AlarmsStateChanged::Id);
}

event_t *create_sensor_fault(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, BooleanStateConfiguration::Events::SensorFault::Id);
}

} // namespace event
} // namespace boolean_state_configuration

namespace operational_state {
namespace event {

event_t *create_operational_error(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, OperationalState::Events::OperationalError::Id);
}

event_t *create_operation_completion(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, OperationalState::Events::OperationCompletion::Id);
}

} // namespace event
} // namespace operational_state
namespace pump_configuration_and_control {
namespace event {

event_t *create_supply_voltage_low(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, PumpConfigurationAndControl::Events::SupplyVoltageLow::Id);
}

event_t *create_supply_voltage_high(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, PumpConfigurationAndControl::Events::SupplyVoltageHigh::Id);
}

event_t *create_power_missing_phase(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, PumpConfigurationAndControl::Events::PowerMissingPhase::Id);
}

event_t *create_system_pressure_low(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, PumpConfigurationAndControl::Events::SystemPressureLow::Id);
}

event_t *create_system_pressure_high(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, PumpConfigurationAndControl::Events::SystemPressureHigh::Id);
}

event_t *create_dry_running(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, PumpConfigurationAndControl::Events::DryRunning::Id);
}

event_t *create_motor_temperature_high(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, PumpConfigurationAndControl::Events::MotorTemperatureHigh::Id);
}

event_t *create_pump_motor_fatal_failure(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, PumpConfigurationAndControl::Events::PumpMotorFatalFailure::Id);
}

event_t *create_electronic_temperature_high(cluster_t *cluster)
{
    return
        esp_matter::event::create(cluster, PumpConfigurationAndControl::Events::ElectronicTemperatureHigh::Id);
}

event_t *create_pump_blocked(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, PumpConfigurationAndControl::Events::PumpBlocked::Id);
}

event_t *create_sensor_failure(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, PumpConfigurationAndControl::Events::SensorFailure::Id);
}

event_t *create_electronic_non_fatal_failure(cluster_t *cluster)
{
    return
        esp_matter::event::create(cluster, PumpConfigurationAndControl::Events::ElectronicNonFatalFailure::Id);
}

event_t *create_electronic_fatal_failure(cluster_t *cluster)
{
    return
        esp_matter::event::create(cluster, PumpConfigurationAndControl::Events::ElectronicFatalFailure::Id);
}

event_t *create_general_fault(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, PumpConfigurationAndControl::Events::GeneralFault::Id);
}

event_t *create_leakage(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, PumpConfigurationAndControl::Events::Leakage::Id);
}

event_t *create_air_detection(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, PumpConfigurationAndControl::Events::AirDetection::Id);
}

event_t *create_turbine_operation(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, PumpConfigurationAndControl::Events::TurbineOperation::Id);
}

esp_err_t *send_supply_voltage_low()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_supply_voltage_high()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_power_missing_phase()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_system_pressure_low()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_system_pressure_high()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_dry_running()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_motor_temperature_high()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_pump_motor_fatal_failure()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_electronic_temperature_high()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_pump_blocked()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_sensor_failure()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_electronic_non_fatal_failure()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_electronic_fatal_failure()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_general_fault()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_leakage()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_air_detection()
{
    /* Not implemented */
    return ESP_OK;
}

esp_err_t send_turbine_operation()
{
    /* Not implemented */
    return ESP_OK;
}

} // namespace event
} // namespace pump_configuration_and_control

namespace electrical_power_measurement {
namespace event {

event_t *create_measurement_period_ranges(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, ElectricalPowerMeasurement::Events::MeasurementPeriodRanges::Id);
}
} // namespace event
} // namespace electrical_power_measurement

namespace electrical_energy_measurement {
namespace event {

event_t *create_cumulative_energy_measured(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, ElectricalEnergyMeasurement::Events::CumulativeEnergyMeasured::Id);
}

event_t *create_periodic_energy_measured(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, ElectricalEnergyMeasurement::Events::PeriodicEnergyMeasured::Id);
}
} // namespace event
} // namespace electrical_energy_measurement

namespace valve_configuration_and_control {
namespace event {
event_t *create_valve_state_changed(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, ValveConfigurationAndControl::Events::ValveStateChanged::Id);
}

event_t *create_valve_fault(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, ValveConfigurationAndControl::Events::ValveFault::Id);
}
} // namespace event
} // namespace valve_configuration_and_control

namespace device_energy_management {
namespace event {
event_t *create_power_adjust_start(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, DeviceEnergyManagement::Events::PowerAdjustStart::Id);
}

event_t *create_power_adjust_end(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, DeviceEnergyManagement::Events::PowerAdjustEnd::Id);
}

event_t *create_paused(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, DeviceEnergyManagement::Events::Paused::Id);
}

event_t *create_resumed(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, DeviceEnergyManagement::Events::Resumed::Id);
}

} // namespace event
} // namespace device_energy_management

namespace water_heater_management {
namespace event {
event_t *create_boost_started(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, WaterHeaterManagement::Events::BoostStarted::Id);
}

event_t *create_boost_ended(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, WaterHeaterManagement::Events::BoostEnded::Id);
}

} // namespace event
} // namespace water_heater_management

namespace commissioner_control {
namespace event {
event_t *create_commissioning_request_result(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, CommissionerControl::Events::CommissioningRequestResult::Id);
}

} // namespace event
} // namespace commissioner_control

namespace occupancy_sensing {
namespace event {
event_t *create_occupancy_changed(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, OccupancySensing::Events::OccupancyChanged::Id);
}

} // namespace event
} // namespace occupancy_sensing

} // namespace cluster
} // namespace esp_matter
