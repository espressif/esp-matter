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

#include <inttypes.h>
#include <esp_matter_delegate_callbacks.h>
#include <esp_matter_core.h>
#include <esp_matter_feature.h>
#include <esp_matter_data_model_priv.h>
#include <app/clusters/mode-base-server/mode-base-server.h>
#include <clusters/EnergyEvse/ClusterId.h>
#include <clusters/energy_evse/integration.h>
#include <app/clusters/microwave-oven-control-server/microwave-oven-control-server.h>
#include <app/clusters/operational-state-server/operational-state-server.h>
#include <app/clusters/resource-monitoring-server/resource-monitoring-server.h>
#include <app/clusters/fan-control-server/fan-control-server.h>
#include <app/clusters/laundry-dryer-controls-server/laundry-dryer-controls-server.h>
#include <app/clusters/valve-configuration-and-control-server/valve-configuration-and-control-server.h>
#include <app/clusters/door-lock-server/door-lock-server.h>
#include <app/clusters/boolean-state-configuration-server/boolean-state-configuration-server.h>
#include <app/clusters/application-basic-server/application-basic-server.h>
#include <app/clusters/laundry-washer-controls-server/laundry-washer-controls-server.h>
#include <app/clusters/window-covering-server/window-covering-server.h>
#include <app/clusters/dishwasher-alarm-server/dishwasher-alarm-server.h>
#include <app/clusters/keypad-input-server/keypad-input-server.h>
#include <app/clusters/mode-select-server/supported-modes-manager.h>
#include <app/clusters/thread-border-router-management-server/thread-border-router-management-server.h>
#include <app/clusters/water-heater-management-server/water-heater-management-server.h>
#include <app/clusters/energy-preference-server/energy-preference-server.h>
#include <app/clusters/commissioner-control-server/commissioner-control-server.h>
#include <app/clusters/actions-server/actions-server.h>
#include <app/clusters/thermostat-server/thermostat-server.h>
#include <app/clusters/diagnostic-logs-server/diagnostic-logs-server.h>
#include <app/clusters/closure-dimension-server/closure-dimension-server.h>
#include <app/clusters/application-launcher-server/application-launcher-server.h>
#include <app/clusters/account-login-server/account-login-server.h>
#include <app/clusters/audio-output-server/audio-output-server.h>
#include <app/clusters/channel-server/channel-server.h>
#include <app/clusters/content-app-observer/content-app-observer.h>
#include <app/clusters/content-control-server/content-control-server.h>
#include <app/clusters/low-power-server/low-power-server.h>
#include <app/clusters/messages-server/messages-server.h>
#include <app/clusters/media-input-server/media-input-server.h>
#include <app/clusters/media-playback-server/media-playback-server.h>
#include <app/clusters/target-navigator-server/target-navigator-server.h>
#include <app/clusters/wake-on-lan-server/wake-on-lan-server.h>
#include <app/clusters/commodity-tariff-server/commodity-tariff-server.h>
#include <app/clusters/commodity-price-server/commodity-price-server.h>
#include <app/clusters/electrical-grid-conditions-server/electrical-grid-conditions-server.h>
#include <app/clusters/meter-identification-server/meter-identification-server.h>
#include <app/clusters/tls-client-management-server/TLSClientManagementCluster.h>

#include <clusters/ota_software_update_provider/integration.h>
#include <clusters/push_av_stream_transport/integration.h>
#include <clusters/closure_control/integration.h>
#include <clusters/power_topology/integration.h>
#include <clusters/device_energy_management/integration.h>
#include <clusters/diagnostic_logs/integration.h>
#include <clusters/electrical_power_measurement/integration.h>
#include <clusters/time_synchronization/integration.h>
#include <clusters/temperature_control/integration.h>
#include <clusters/resource_monitor/integration.h>
#include <clusters/chime/integration.h>
#include <clusters/tls_client_management/integration.h>
#include <clusters/tls_certificate_management/integration.h>

using namespace chip::app::Clusters;
namespace esp_matter {
namespace cluster {

static uint32_t get_feature_map_value(uint16_t endpoint_id, uint32_t cluster_id)
{
    uint32_t attribute_id = Globals::Attributes::FeatureMap::Id;
    attribute_t *attribute = attribute::get(endpoint_id, cluster_id, attribute_id);
    VerifyOrReturnError(attribute, 0);

    esp_matter_attr_val_t val = esp_matter_invalid(nullptr);
    VerifyOrReturnError(attribute::get_val_internal(attribute, &val) == ESP_OK, 0);
    return val.val.u32;
}

// Cluster-specific optional attributes handlers
chip::BitMask<EnergyEvse::OptionalAttributes> get_energy_evse_enabled_optional_attributes(uint16_t endpoint_id)
{
    chip::BitMask<EnergyEvse::OptionalAttributes> optional_attrs = 0;

    if (endpoint::is_attribute_enabled(endpoint_id, EnergyEvse::Id, EnergyEvse::Attributes::UserMaximumChargeCurrent::Id)) {
        optional_attrs.Set(EnergyEvse::OptionalAttributes::kSupportsUserMaximumChargingCurrent);
    }

    if (endpoint::is_attribute_enabled(endpoint_id, EnergyEvse::Id, EnergyEvse::Attributes::RandomizationDelayWindow::Id)) {
        optional_attrs.Set(EnergyEvse::OptionalAttributes::kSupportsRandomizationWindow);
    }

    if (endpoint::is_attribute_enabled(endpoint_id, EnergyEvse::Id, EnergyEvse::Attributes::ApproximateEVEfficiency::Id)) {
        optional_attrs.Set(EnergyEvse::OptionalAttributes::kSupportsApproximateEvEfficiency);
    }

    return optional_attrs;
}

chip::BitMask<ElectricalPowerMeasurement::OptionalAttributes> get_electrical_power_measurement_enabled_optional_attributes(uint16_t endpoint_id)
{
    chip::BitMask<ElectricalPowerMeasurement::OptionalAttributes> optional_attrs = 0;

    // Check for various optional attributes
    if (endpoint::is_attribute_enabled(endpoint_id, ElectricalPowerMeasurement::Id, ElectricalPowerMeasurement::Attributes::Ranges::Id)) {
        optional_attrs.Set(ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeRanges);
    }

    if (endpoint::is_attribute_enabled(endpoint_id, ElectricalPowerMeasurement::Id, ElectricalPowerMeasurement::Attributes::Voltage::Id)) {
        optional_attrs.Set(ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeVoltage);
    }

    if (endpoint::is_attribute_enabled(endpoint_id, ElectricalPowerMeasurement::Id, ElectricalPowerMeasurement::Attributes::ActiveCurrent::Id)) {
        optional_attrs.Set(ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeActiveCurrent);
    }

    if (endpoint::is_attribute_enabled(endpoint_id, ElectricalPowerMeasurement::Id, ElectricalPowerMeasurement::Attributes::ReactiveCurrent::Id)) {
        optional_attrs.Set(ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeReactiveCurrent);
    }

    if (endpoint::is_attribute_enabled(endpoint_id, ElectricalPowerMeasurement::Id, ElectricalPowerMeasurement::Attributes::ApparentCurrent::Id)) {
        optional_attrs.Set(ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeApparentCurrent);
    }

    if (endpoint::is_attribute_enabled(endpoint_id, ElectricalPowerMeasurement::Id, ElectricalPowerMeasurement::Attributes::ReactivePower::Id)) {
        optional_attrs.Set(ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeReactivePower);
    }

    if (endpoint::is_attribute_enabled(endpoint_id, ElectricalPowerMeasurement::Id, ElectricalPowerMeasurement::Attributes::ApparentPower::Id)) {
        optional_attrs.Set(ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeApparentPower);
    }

    if (endpoint::is_attribute_enabled(endpoint_id, ElectricalPowerMeasurement::Id, ElectricalPowerMeasurement::Attributes::RMSVoltage::Id)) {
        optional_attrs.Set(ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeRMSVoltage);
    }

    if (endpoint::is_attribute_enabled(endpoint_id, ElectricalPowerMeasurement::Id, ElectricalPowerMeasurement::Attributes::RMSCurrent::Id)) {
        optional_attrs.Set(ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeRMSCurrent);
    }

    if (endpoint::is_attribute_enabled(endpoint_id, ElectricalPowerMeasurement::Id, ElectricalPowerMeasurement::Attributes::RMSPower::Id)) {
        optional_attrs.Set(ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeRMSPower);
    }

    if (endpoint::is_attribute_enabled(endpoint_id, ElectricalPowerMeasurement::Id, ElectricalPowerMeasurement::Attributes::Frequency::Id)) {
        optional_attrs.Set(ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeFrequency);
    }

    if (endpoint::is_attribute_enabled(endpoint_id, ElectricalPowerMeasurement::Id, ElectricalPowerMeasurement::Attributes::PowerFactor::Id)) {
        optional_attrs.Set(ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributePowerFactor);
    }

    if (endpoint::is_attribute_enabled(endpoint_id, ElectricalPowerMeasurement::Id, ElectricalPowerMeasurement::Attributes::NeutralCurrent::Id)) {
        optional_attrs.Set(ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeNeutralCurrent);
    }

    return optional_attrs;
}

// Cluster-specific optional commands handlers
chip::BitMask<EnergyEvse::OptionalCommands> get_energy_evse_enabled_optional_commands(uint16_t endpoint_id)
{
    chip::BitMask<EnergyEvse::OptionalCommands> optional_cmds = 0;

    if (endpoint::is_command_enabled(endpoint_id, EnergyEvse::Id, EnergyEvse::Commands::StartDiagnostics::Id)) {
        optional_cmds.Set(EnergyEvse::OptionalCommands::kSupportsStartDiagnostics);
    }

    return optional_cmds;
}

namespace delegate_cb {

static void ModeBaseShutdownCB(void *managed_instance, uint16_t /*endpoint_id*/)
{
    delete static_cast<ModeBase::Instance*>(managed_instance);
}

void InitModeDelegate(void *delegate, uint16_t endpoint_id, uint32_t cluster_id)
{
    VerifyOrReturn(delegate != nullptr);
    cluster_t *cl = cluster::get(endpoint_id, cluster_id);
    VerifyOrReturn(cl != nullptr);
    ModeBase::Instance *modeInstance = static_cast<ModeBase::Instance*>(get_delegate_managed_instance(cl));
    if (!modeInstance) {
        ModeBase::Delegate *mode_delegate = static_cast<ModeBase::Delegate*>(delegate);
        uint32_t feature_map = get_feature_map_value(endpoint_id, cluster_id);
        modeInstance = new ModeBase::Instance(mode_delegate, endpoint_id, cluster_id, feature_map);
        set_delegate_shutdown_callback_and_managed_instance(cl, ModeBaseShutdownCB, modeInstance);
    }
    (void)modeInstance->Init();
}

void LaundryWasherModeDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    InitModeDelegate(delegate, endpoint_id, LaundryWasherMode::Id);
}

void DishWasherModeDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    InitModeDelegate(delegate, endpoint_id, DishwasherMode::Id);
}

void RefrigeratorAndTCCModeDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    InitModeDelegate(delegate, endpoint_id, RefrigeratorAndTemperatureControlledCabinetMode::Id);
}

void RvcRunModeDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    InitModeDelegate(delegate, endpoint_id, RvcRunMode::Id);
}

void RvcCleanModeDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    InitModeDelegate(delegate, endpoint_id, RvcCleanMode::Id);
}

void WaterHeaterModeDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    InitModeDelegate(delegate, endpoint_id, WaterHeaterMode::Id);
}

void EnergyEvseModeDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    InitModeDelegate(delegate, endpoint_id, EnergyEvseMode::Id);
}

void MicrowaveOvenModeDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    InitModeDelegate(delegate, endpoint_id, MicrowaveOvenMode::Id);
}

void DeviceEnergyManagementModeDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    InitModeDelegate(delegate, endpoint_id, DeviceEnergyManagementMode::Id);
}

static void EnergyEvseShutdownCB(void *managed_instance, uint16_t /*endpoint_id*/)
{
    delete static_cast<EnergyEvse::Instance*>(managed_instance);
}

void EnergyEvseDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    cluster_t *cl = cluster::get(endpoint_id, EnergyEvse::Id);
    VerifyOrReturn(cl != nullptr);
    EnergyEvse::Instance *energyEvseInstance = static_cast<EnergyEvse::Instance*>(get_delegate_managed_instance(cl));
    if (!energyEvseInstance) {
        EnergyEvse::Delegate *energy_evse_delegate = static_cast<EnergyEvse::Delegate*>(delegate);
        uint32_t feature_map = get_feature_map_value(endpoint_id, EnergyEvse::Id);
        chip::BitMask<EnergyEvse::OptionalAttributes> optional_attrs = get_energy_evse_enabled_optional_attributes(endpoint_id);
        chip::BitMask<EnergyEvse::OptionalCommands> optional_cmds = get_energy_evse_enabled_optional_commands(endpoint_id);
        energyEvseInstance = new EnergyEvse::Instance(endpoint_id, *energy_evse_delegate, chip::BitMask<EnergyEvse::Feature, uint32_t>(feature_map),
                                                      optional_attrs, optional_cmds);
        set_delegate_shutdown_callback_and_managed_instance(cl, EnergyEvseShutdownCB, energyEvseInstance);
    }
    (void)energyEvseInstance->Init();
}

static void MicrowaveOvenControlShutdownCB(void *managed_instance, uint16_t /*endpoint_id*/)
{
    delete static_cast<MicrowaveOvenControl::Instance*>(managed_instance);
}

static void OperationalStateShutdownCB(void *managed_instance, uint16_t /*endpoint_id*/)
{
    delete static_cast<OperationalState::Instance*>(managed_instance);
}

void MicrowaveOvenControlDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    cluster_t *mom_cl = cluster::get(endpoint_id, MicrowaveOvenMode::Id);
    cluster_t *os_cl = cluster::get(endpoint_id, OperationalState::Id);
    cluster_t *moc_cl = cluster::get(endpoint_id, MicrowaveOvenControl::Id);
    VerifyOrReturn(mom_cl != nullptr && os_cl != nullptr && moc_cl != nullptr);

    ModeBase::Delegate *microwave_oven_mode_delegate = static_cast<ModeBase::Delegate*>(get_delegate_impl(mom_cl));
    OperationalState::Delegate *operational_state_delegate = static_cast<OperationalState::Delegate*>(get_delegate_impl(os_cl));
    VerifyOrReturn(microwave_oven_mode_delegate != nullptr && operational_state_delegate != nullptr);

    // Ensure MicrowaveOvenMode instance exists on its cluster slot.
    ModeBase::Instance *microwaveOvenModeInstance = static_cast<ModeBase::Instance*>(get_delegate_managed_instance(mom_cl));
    if (!microwaveOvenModeInstance) {
        uint32_t fm = get_feature_map_value(endpoint_id, MicrowaveOvenMode::Id);
        microwaveOvenModeInstance = new ModeBase::Instance(microwave_oven_mode_delegate, endpoint_id, MicrowaveOvenMode::Id, fm);
        set_delegate_shutdown_callback_and_managed_instance(mom_cl, ModeBaseShutdownCB, microwaveOvenModeInstance);
    }

    // Ensure OperationalState instance exists on its cluster slot.
    OperationalState::Instance *operationalStateInstance = static_cast<OperationalState::Instance*>(get_delegate_managed_instance(os_cl));
    if (!operationalStateInstance) {
        operationalStateInstance = new OperationalState::Instance(operational_state_delegate, endpoint_id);
        set_delegate_shutdown_callback_and_managed_instance(os_cl, OperationalStateShutdownCB, operationalStateInstance);
    }

    // MicrowaveOvenControl instance.
    MicrowaveOvenControl::Instance *microwaveOvenControlInstance = static_cast<MicrowaveOvenControl::Instance*>(get_delegate_managed_instance(moc_cl));
    if (!microwaveOvenControlInstance) {
        MicrowaveOvenControl::Delegate *microwave_oven_control_delegate = static_cast<MicrowaveOvenControl::Delegate*>(delegate);
        uint32_t feature_map = get_feature_map_value(endpoint_id, MicrowaveOvenControl::Id);
        microwaveOvenControlInstance = new MicrowaveOvenControl::Instance(microwave_oven_control_delegate, endpoint_id, MicrowaveOvenControl::Id, feature_map,
                                                                          *operationalStateInstance, *microwaveOvenModeInstance);
        set_delegate_shutdown_callback_and_managed_instance(moc_cl, MicrowaveOvenControlShutdownCB, microwaveOvenControlInstance);
    }
    (void)microwaveOvenControlInstance->Init();
}

void OperationalStateDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    cluster_t *cl = cluster::get(endpoint_id, OperationalState::Id);
    VerifyOrReturn(cl != nullptr);
    OperationalState::Instance *operationalStateInstance = static_cast<OperationalState::Instance*>(get_delegate_managed_instance(cl));
    if (!operationalStateInstance) {
        OperationalState::Delegate *operational_state_delegate = static_cast<OperationalState::Delegate*>(delegate);
        operationalStateInstance = new OperationalState::Instance(operational_state_delegate, endpoint_id);
        set_delegate_shutdown_callback_and_managed_instance(cl, OperationalStateShutdownCB, operationalStateInstance);
    }
    (void)operationalStateInstance->Init();
}

void FanControlDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    FanControl::Delegate *fan_control_delegate = static_cast<FanControl::Delegate*>(delegate);
    FanControl::SetDefaultDelegate(endpoint_id, fan_control_delegate);
}

void HepaFilterMonitoringDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    ResourceMonitoring::Delegate *resource_monitoring_delegate = static_cast<ResourceMonitoring::Delegate*>(delegate);
    (void) chip::app::Clusters::ResourceMonitoring::SetDefaultDelegate(endpoint_id, HepaFilterMonitoring::Id,
                                                                       resource_monitoring_delegate);
}

void ActivatedCarbonFilterMonitoringDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    ResourceMonitoring::Delegate *resource_monitoring_delegate = static_cast<ResourceMonitoring::Delegate*>(delegate);
    (void) chip::app::Clusters::ResourceMonitoring::SetDefaultDelegate(endpoint_id, ActivatedCarbonFilterMonitoring::Id,
                                                                       resource_monitoring_delegate);
}

void LaundryDryerControlsDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    LaundryDryerControls::Delegate *laundry_dryer_controls_delegate = static_cast<LaundryDryerControls::Delegate*>(delegate);
    LaundryDryerControls::LaundryDryerControlsServer::SetDefaultDelegate(endpoint_id, laundry_dryer_controls_delegate);
}

void ValveConfigurationAndControlDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    ValveConfigurationAndControl::Delegate *valve_configuration_and_control_delegate = static_cast<ValveConfigurationAndControl::Delegate*>(delegate);
    ValveConfigurationAndControl::SetDefaultDelegate(endpoint_id, valve_configuration_and_control_delegate);
}

static void DeviceEnergyManagementShutdownCB(void *managed_instance, uint16_t /*endpoint_id*/)
{
    delete static_cast<DeviceEnergyManagement::Instance*>(managed_instance);
}

void DeviceEnergyManagementDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    cluster_t *cl = cluster::get(endpoint_id, DeviceEnergyManagement::Id);
    VerifyOrReturn(cl != nullptr);
    DeviceEnergyManagement::Instance *deviceEnergyManagementInstance = static_cast<DeviceEnergyManagement::Instance*>(get_delegate_managed_instance(cl));
    if (!deviceEnergyManagementInstance) {
        DeviceEnergyManagement::Delegate *device_energy_management_delegate = static_cast<DeviceEnergyManagement::Delegate*>(delegate);
        uint32_t feature_map = get_feature_map_value(endpoint_id, DeviceEnergyManagement::Id);
        deviceEnergyManagementInstance = new DeviceEnergyManagement::Instance(endpoint_id, *device_energy_management_delegate, chip::BitMask<DeviceEnergyManagement::Feature, uint32_t>(feature_map));
        set_delegate_shutdown_callback_and_managed_instance(cl, DeviceEnergyManagementShutdownCB, deviceEnergyManagementInstance);
    }
    (void)deviceEnergyManagementInstance->Init();
}

void DoorLockDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    DoorLock::Delegate *door_lock_delegate = static_cast<DoorLock::Delegate*>(delegate);
    (void)DoorLockServer::Instance().SetDelegate(endpoint_id, door_lock_delegate);
}

void BooleanStateConfigurationDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    BooleanStateConfiguration::Delegate *boolean_state_configuration_delegate = static_cast<BooleanStateConfiguration::Delegate*>(delegate);
    BooleanStateConfiguration::SetDefaultDelegate(endpoint_id, boolean_state_configuration_delegate);
}

void TimeSynchronizationDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    TimeSynchronization::Delegate *time_synchronization_delegate = static_cast<TimeSynchronization::Delegate*>(delegate);
    TimeSynchronization::SetDefaultDelegate(time_synchronization_delegate);
}

void ApplicationBasicDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    ApplicationBasic::Delegate *application_basic_delegate = static_cast<ApplicationBasic::Delegate*>(delegate);
    ApplicationBasic::SetDefaultDelegate(endpoint_id, application_basic_delegate);
}

static void PowerTopologyShutdownCB(void *managed_instance, uint16_t /*endpoint_id*/)
{
    delete static_cast<PowerTopology::Instance*>(managed_instance);
}

void PowerTopologyDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    cluster_t *cl = cluster::get(endpoint_id, PowerTopology::Id);
    VerifyOrReturn(cl != nullptr);
    PowerTopology::Instance *powerTopologyInstance = static_cast<PowerTopology::Instance*>(get_delegate_managed_instance(cl));
    if (!powerTopologyInstance) {
        PowerTopology::Delegate *power_topology_delegate = static_cast<PowerTopology::Delegate*>(delegate);
        chip::BitMask<PowerTopology::Feature> feature_map(get_feature_map_value(endpoint_id, PowerTopology::Id));
        powerTopologyInstance = new PowerTopology::Instance(endpoint_id, *power_topology_delegate, feature_map);
        set_delegate_shutdown_callback_and_managed_instance(cl, PowerTopologyShutdownCB, powerTopologyInstance);
    }
    (void)powerTopologyInstance->Init();
}

static void ElectricalPowerMeasurementShutdownCB(void *managed_instance, uint16_t /*endpoint_id*/)
{
    delete static_cast<ElectricalPowerMeasurement::Instance*>(managed_instance);
}

void ElectricalPowerMeasurementDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    cluster_t *cl = cluster::get(endpoint_id, ElectricalPowerMeasurement::Id);
    VerifyOrReturn(cl != nullptr);
    ElectricalPowerMeasurement::Instance *electricalPowerMeasurementInstance = static_cast<ElectricalPowerMeasurement::Instance*>(get_delegate_managed_instance(cl));
    if (!electricalPowerMeasurementInstance) {
        ElectricalPowerMeasurement::Delegate *electrical_power_measurement_delegate = static_cast<ElectricalPowerMeasurement::Delegate*>(delegate);
        uint32_t feature_map = get_feature_map_value(endpoint_id, ElectricalPowerMeasurement::Id);
        chip::BitMask<ElectricalPowerMeasurement::OptionalAttributes> optional_attrs = get_electrical_power_measurement_enabled_optional_attributes(endpoint_id);
        electricalPowerMeasurementInstance = new ElectricalPowerMeasurement::Instance(endpoint_id, *electrical_power_measurement_delegate,
                                                                                      chip::BitMask<ElectricalPowerMeasurement::Feature, uint32_t>(feature_map), optional_attrs);
        set_delegate_shutdown_callback_and_managed_instance(cl, ElectricalPowerMeasurementShutdownCB, electricalPowerMeasurementInstance);
    }
    (void)electricalPowerMeasurementInstance->Init();
}

void LaundryWasherControlsDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    LaundryWasherControls::Delegate *laundry_washer_controls_delegate = static_cast<LaundryWasherControls::Delegate*>(delegate);
    LaundryWasherControls::LaundryWasherControlsServer::SetDefaultDelegate(endpoint_id, laundry_washer_controls_delegate);
}

void WindowCoveringDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    WindowCovering::Delegate *window_covering_delegate = static_cast<WindowCovering::Delegate*>(delegate);
    WindowCovering::SetDefaultDelegate(endpoint_id, window_covering_delegate);
}

void DishwasherAlarmDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    DishwasherAlarm::Delegate *dishwasher_alarm_delegate = static_cast<DishwasherAlarm::Delegate*>(delegate);
    DishwasherAlarm::SetDefaultDelegate(endpoint_id, dishwasher_alarm_delegate);
}

void KeypadInputDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    KeypadInput::Delegate *keypad_input_delegate = static_cast<KeypadInput::Delegate*>(delegate);
    KeypadInput::SetDefaultDelegate(endpoint_id, keypad_input_delegate);
}

void ModeSelectDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    ModeSelect::SupportedModesManager *supported_modes_manager = static_cast<ModeSelect::SupportedModesManager*>(delegate);
    ModeSelect::setSupportedModesManager(supported_modes_manager);
}

static void ThreadBorderRouterManagementShutdownCB(void *managed_instance, uint16_t /*endpoint_id*/)
{
    chip::Platform::Delete(static_cast<ThreadBorderRouterManagement::ServerInstance*>(managed_instance));
}

void ThreadBorderRouterManagementDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    assert(delegate != nullptr);
    cluster_t *cl = cluster::get(endpoint_id, ThreadBorderRouterManagement::Id);
    VerifyOrReturn(cl != nullptr);
    ThreadBorderRouterManagement::ServerInstance *server_instance =
        static_cast<ThreadBorderRouterManagement::ServerInstance*>(get_delegate_managed_instance(cl));
    if (!server_instance) {
        /* Get the attribute */
        attribute_t *attribute = attribute::get(endpoint_id, ThreadBorderRouterManagement::Id, Globals::Attributes::FeatureMap::Id);
        assert(attribute != nullptr);
        /* Update the value if the attribute already exists */
        esp_matter_attr_val_t val = esp_matter_invalid(nullptr);
        attribute::get_val(attribute, &val);
        bool pan_change_supported = (val.val.u32 & thread_border_router_management::feature::pan_change::get_id()) ? true : false;
        ThreadBorderRouterManagement::Delegate *thread_br_delegate = static_cast<ThreadBorderRouterManagement::Delegate *>(delegate);
        assert(thread_br_delegate->GetPanChangeSupported() == pan_change_supported);
        server_instance = chip::Platform::New<ThreadBorderRouterManagement::ServerInstance>(endpoint_id, thread_br_delegate, chip::Server::GetInstance().GetFailSafeContext());
        set_delegate_shutdown_callback_and_managed_instance(cl, ThreadBorderRouterManagementShutdownCB, server_instance);
    }
    (void)server_instance->Init();
}

void ServiceAreaDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    // TODO: This cluster has two delegates. We need to update existing delegate logic to accommodate multiple delegates.
}

static void WaterHeaterManagementShutdownCB(void *managed_instance, uint16_t /*endpoint_id*/)
{
    delete static_cast<WaterHeaterManagement::Instance*>(managed_instance);
}

void WaterHeaterManagementDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    if (delegate == nullptr) {
        return;
    }
    cluster_t *cl = cluster::get(endpoint_id, WaterHeaterManagement::Id);
    VerifyOrReturn(cl != nullptr);
    WaterHeaterManagement::Instance *wHtrInstance = static_cast<WaterHeaterManagement::Instance*>(get_delegate_managed_instance(cl));
    if (!wHtrInstance) {
        WaterHeaterManagement::Delegate *whtr_delegate = static_cast<WaterHeaterManagement::Delegate*>(delegate);
        uint32_t feature_map = get_feature_map_value(endpoint_id, WaterHeaterManagement::Id);
        wHtrInstance = new WaterHeaterManagement::Instance(endpoint_id, *whtr_delegate, chip::BitMask<WaterHeaterManagement::Feature, uint32_t>(feature_map));
        set_delegate_shutdown_callback_and_managed_instance(cl, WaterHeaterManagementShutdownCB, wHtrInstance);
    }
    (void)wHtrInstance->Init();
}

void EnergyPreferenceDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    if (delegate == nullptr) {
        return;
    }
    EnergyPreference::Delegate *energy_preference_delegate = static_cast<EnergyPreference::Delegate*>(delegate);
    EnergyPreference::SetDelegate(energy_preference_delegate);
}

static void CommissionerControlShutdownCB(void *managed_instance, uint16_t /*endpoint_id*/)
{
    delete static_cast<CommissionerControl::CommissionerControlServer*>(managed_instance);
}

void CommissionerControlDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    if (delegate == nullptr) {
        return;
    }
    cluster_t *cl = cluster::get(endpoint_id, CommissionerControl::Id);
    VerifyOrReturn(cl != nullptr);
    CommissionerControl::CommissionerControlServer *commissioner_control_instance =
        static_cast<CommissionerControl::CommissionerControlServer*>(get_delegate_managed_instance(cl));
    if (!commissioner_control_instance) {
        CommissionerControl::Delegate *commissioner_control_delegate = static_cast<CommissionerControl::Delegate*>(delegate);
        commissioner_control_instance =
            new CommissionerControl::CommissionerControlServer(commissioner_control_delegate, endpoint_id);
        set_delegate_shutdown_callback_and_managed_instance(cl, CommissionerControlShutdownCB, commissioner_control_instance);
    }
    (void)commissioner_control_instance->Init();
}

static void ActionsShutdownCB(void *managed_instance, uint16_t /*endpoint_id*/)
{
    delete static_cast<Actions::ActionsServer*>(managed_instance);
}

void ActionsDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    cluster_t *cl = cluster::get(endpoint_id, Actions::Id);
    VerifyOrReturn(cl != nullptr);
    Actions::ActionsServer *actionsServer = static_cast<Actions::ActionsServer*>(get_delegate_managed_instance(cl));
    if (!actionsServer) {
        Actions::Delegate *actions_delegate = static_cast<Actions::Delegate*>(delegate);
        actionsServer = new Actions::ActionsServer(endpoint_id, *actions_delegate);
        set_delegate_shutdown_callback_and_managed_instance(cl, ActionsShutdownCB, actionsServer);
    }
    (void)actionsServer->Init();
}

void ThermostatDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    Thermostat::Delegate *thermostat_delegate = static_cast<Thermostat::Delegate*>(delegate);
    Thermostat::SetDefaultDelegate(endpoint_id, thermostat_delegate);
}

void TemperatureControlDelegateInitCB(void *delegate, [[maybe_unused]] uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    chip::app::Clusters::TemperatureControl::SetDelegate(
        static_cast<chip::app::Clusters::TemperatureControl::SupportedTemperatureLevelsIteratorDelegate *>(delegate));
}

void OtaSoftwareUpdateProviderDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    chip::app::Clusters::OTAProvider::SetDelegate(endpoint_id, static_cast<chip::app::Clusters::OTAProviderDelegate *>(delegate));
}

void DiagnosticLogsDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr && endpoint_id == chip::kRootEndpointId);
    DiagnosticLogs::DiagnosticLogsProviderDelegate *diagnostic_logs_delegate = static_cast<DiagnosticLogs::DiagnosticLogsProviderDelegate*>(delegate);
    DiagnosticLogs::SetDiagnosticLogsProviderDelegate(diagnostic_logs_delegate);
}

static void ChimeShutdownCB(void *managed_instance, uint16_t /*endpoint_id*/)
{
    delete static_cast<Chime::ChimeServer*>(managed_instance);
}

void ChimeDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    cluster_t *cl = cluster::get(endpoint_id, Chime::Id);
    VerifyOrReturn(cl != nullptr);
    Chime::ChimeServer *chime_server = static_cast<Chime::ChimeServer*>(get_delegate_managed_instance(cl));
    if (!chime_server) {
        ChimeDelegate *chime_delegate = static_cast<ChimeDelegate*>(delegate);
        chime_server = new Chime::ChimeServer(endpoint_id, *chime_delegate);
        set_delegate_shutdown_callback_and_managed_instance(cl, ChimeShutdownCB, chime_server);
    }
    (void)chime_server->Init();
}

void ClosureControlDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    auto *cc_delegate = static_cast<ClosureControl::ClosureControlClusterDelegate*>(delegate);
    ClosureControl::MatterClosureControlSetDelegate(static_cast<chip::EndpointId>(endpoint_id), *cc_delegate);
    // Cluster instance lifecycle is owned by ESPMatterClosureControlClusterServer{Init,Shutdown}Callback
    // (see data_model_provider/clusters/closure_control/integration.cpp), wired via
    // set_init_and_shutdown_callbacks() on the generated cluster.
}

struct ClosureDimensionBundle {
    ClosureDimension::MatterContext *context;
    ClosureDimension::Interface *interface;
};

static void ClosureDimensionShutdownCB(void *managed_instance, uint16_t /*endpoint_id*/)
{
    ClosureDimensionBundle *bundle = static_cast<ClosureDimensionBundle*>(managed_instance);
    (void)bundle->interface->Shutdown();
    delete bundle->interface;
    delete bundle->context;
    delete bundle;
}

void ClosureDimensionDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    cluster_t *cl = cluster::get(endpoint_id, ClosureDimension::Id);
    VerifyOrReturn(cl != nullptr);
    ClosureDimensionBundle *bundle = static_cast<ClosureDimensionBundle*>(get_delegate_managed_instance(cl));
    if (!bundle) {
        auto *cd_delegate = static_cast<ClosureDimension::DelegateBase*>(delegate);
        bundle = new ClosureDimensionBundle();
        bundle->context = new ClosureDimension::MatterContext(endpoint_id);
        bundle->interface = new ClosureDimension::Interface(endpoint_id, *cd_delegate, *bundle->context);
        set_delegate_shutdown_callback_and_managed_instance(cl, ClosureDimensionShutdownCB, bundle);
    }
    (void)bundle->interface->Init();
}

void PushAvStreamTransportDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    PushAvStreamTransportDelegate *push_av_stream_transport_delegate = static_cast<PushAvStreamTransportDelegate*>(delegate);
    chip::app::Clusters::PushAvStreamTransport::SetDelegate(endpoint_id, push_av_stream_transport_delegate);
}

static void CommodityTariffShutdownCB(void *managed_instance, uint16_t /*endpoint_id*/)
{
    delete static_cast<CommodityTariff::Instance*>(managed_instance);
}

void CommodityTariffDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    cluster_t *cl = cluster::get(endpoint_id, CommodityTariff::Id);
    VerifyOrReturn(cl != nullptr);
    CommodityTariff::Instance *commodity_tariff_instance = static_cast<CommodityTariff::Instance*>(get_delegate_managed_instance(cl));
    if (!commodity_tariff_instance) {
        CommodityTariff::Delegate *commodity_tariff_delegate = static_cast<CommodityTariff::Delegate*>(delegate);
        uint32_t feature_map = get_feature_map_value(endpoint_id, CommodityTariff::Id);
        commodity_tariff_instance = new CommodityTariff::Instance(endpoint_id, *commodity_tariff_delegate, chip::BitMask<CommodityTariff::Feature, uint32_t>(feature_map));
        set_delegate_shutdown_callback_and_managed_instance(cl, CommodityTariffShutdownCB, commodity_tariff_instance);
    }
    (void)commodity_tariff_instance->Init();
}

static void CommodityPriceShutdownCB(void *managed_instance, uint16_t /*endpoint_id*/)
{
    delete static_cast<CommodityPrice::Instance*>(managed_instance);
}

void CommodityPriceDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    cluster_t *cl = cluster::get(endpoint_id, CommodityPrice::Id);
    VerifyOrReturn(cl != nullptr);
    CommodityPrice::Instance *commodity_price_instance = static_cast<CommodityPrice::Instance*>(get_delegate_managed_instance(cl));
    if (!commodity_price_instance) {
        CommodityPrice::Delegate *commodity_price_delegate = static_cast<CommodityPrice::Delegate*>(delegate);
        uint32_t feature_map = get_feature_map_value(endpoint_id, CommodityPrice::Id);
        commodity_price_instance = new CommodityPrice::Instance(endpoint_id, *commodity_price_delegate, chip::BitMask<CommodityPrice::Feature, uint32_t>(feature_map));
        set_delegate_shutdown_callback_and_managed_instance(cl, CommodityPriceShutdownCB, commodity_price_instance);
    }
    (void)commodity_price_instance->Init();
}

static void ElectricalGridConditionsShutdownCB(void *managed_instance, uint16_t /*endpoint_id*/)
{
    delete static_cast<ElectricalGridConditions::Instance*>(managed_instance);
}

void ElectricalGridConditionsDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    cluster_t *cl = cluster::get(endpoint_id, ElectricalGridConditions::Id);
    VerifyOrReturn(cl != nullptr);
    ElectricalGridConditions::Instance *electrical_grid_conditions_instance = static_cast<ElectricalGridConditions::Instance*>(get_delegate_managed_instance(cl));
    if (!electrical_grid_conditions_instance) {
        ElectricalGridConditions::Delegate *electrical_grid_conditions_delegate = static_cast<ElectricalGridConditions::Delegate*>(delegate);
        uint32_t feature_map = get_feature_map_value(endpoint_id, ElectricalGridConditions::Id);
        electrical_grid_conditions_instance = new ElectricalGridConditions::Instance(endpoint_id, *electrical_grid_conditions_delegate, chip::BitMask<ElectricalGridConditions::Feature, uint32_t>(feature_map));
        set_delegate_shutdown_callback_and_managed_instance(cl, ElectricalGridConditionsShutdownCB, electrical_grid_conditions_instance);
    }
    (void)electrical_grid_conditions_instance->Init();
}

static void MeterIdentificationShutdownCB(void *managed_instance, uint16_t /*endpoint_id*/)
{
    delete static_cast<MeterIdentification::Instance*>(managed_instance);
}

/* Not a delegate but an Initialization callback */
void MeterIdentificationDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    cluster_t *cl = cluster::get(endpoint_id, MeterIdentification::Id);
    VerifyOrReturn(cl != nullptr);
    MeterIdentification::Instance *meter_identification_instance = static_cast<MeterIdentification::Instance*>(get_delegate_managed_instance(cl));
    if (!meter_identification_instance) {
        uint32_t feature_map = get_feature_map_value(endpoint_id, MeterIdentification::Id);
        meter_identification_instance = new MeterIdentification::Instance(endpoint_id, chip::BitMask<MeterIdentification::Feature, uint32_t>(feature_map));
        set_delegate_shutdown_callback_and_managed_instance(cl, MeterIdentificationShutdownCB, meter_identification_instance);
    }
    LogErrorOnFailure(meter_identification_instance->Init());
}

void ApplicationLauncherDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    ApplicationLauncher::Delegate *application_launcher_delegate = static_cast<ApplicationLauncher::Delegate *>(delegate);
    ApplicationLauncher::SetDefaultDelegate(endpoint_id, application_launcher_delegate);
}
void AccountLoginDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    AccountLogin::Delegate *account_login_delegate = static_cast<AccountLogin::Delegate *>(delegate);
    AccountLogin::SetDefaultDelegate(endpoint_id, account_login_delegate);
}
void AudioOutputDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    AudioOutput::Delegate *audio_output_delegate = static_cast<AudioOutput::Delegate *>(delegate);
    AudioOutput::SetDefaultDelegate(endpoint_id, audio_output_delegate);
}
void ChannelDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    Channel::Delegate *channel_delegate = static_cast<Channel::Delegate *>(delegate);
    Channel::SetDefaultDelegate(endpoint_id, channel_delegate);
}
void ContentAppObserverDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    ContentAppObserver::Delegate *content_app_observer_delegate = static_cast<ContentAppObserver::Delegate *>(delegate);
    ContentAppObserver::SetDefaultDelegate(endpoint_id, content_app_observer_delegate);
}
void ContentControlDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    ContentControl::Delegate *content_control_delegate = static_cast<ContentControl::Delegate *>(delegate);
    ContentControl::SetDefaultDelegate(endpoint_id, content_control_delegate);
}
void DishwasherModeDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    InitModeDelegate(delegate, endpoint_id, DishwasherMode::Id);
}
void LowPowerDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    LowPower::Delegate *low_power_delegate = static_cast<LowPower::Delegate *>(delegate);
    LowPower::SetDefaultDelegate(endpoint_id, low_power_delegate);
}
void MessagesDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    Messages::Delegate *messages_delegate = static_cast<Messages::Delegate *>(delegate);
    Messages::SetDefaultDelegate(endpoint_id, messages_delegate);
}
void MediaInputDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    MediaInput::Delegate *media_input_delegate = static_cast<MediaInput::Delegate *>(delegate);
    MediaInput::SetDefaultDelegate(endpoint_id, media_input_delegate);
}
void MediaPlaybackDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    MediaPlayback::Delegate *media_playback_delegate = static_cast<MediaPlayback::Delegate *>(delegate);
    MediaPlayback::SetDefaultDelegate(endpoint_id, media_playback_delegate);
}
void OvenModeDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    InitModeDelegate(delegate, endpoint_id, OvenMode::Id);
}

static void OvenCavityOperationalStateShutdownCB(void *managed_instance, uint16_t /*endpoint_id*/)
{
    delete static_cast<OvenCavityOperationalState::Instance*>(managed_instance);
}

void OvenCavityOperationalStateDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    cluster_t *cl = cluster::get(endpoint_id, OvenCavityOperationalState::Id);
    VerifyOrReturn(cl != nullptr);
    OvenCavityOperationalState::Instance *instance = static_cast<OvenCavityOperationalState::Instance*>(get_delegate_managed_instance(cl));
    if (!instance) {
        OperationalState::Delegate *operational_state_delegate = static_cast<OperationalState::Delegate *>(delegate);
        instance = new OvenCavityOperationalState::Instance(operational_state_delegate, endpoint_id);
        set_delegate_shutdown_callback_and_managed_instance(cl, OvenCavityOperationalStateShutdownCB, instance);
    }
    (void) instance->Init();
}

void RefrigeratorAndTemperatureControlledCabinetModeDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    InitModeDelegate(delegate, endpoint_id, RefrigeratorAndTemperatureControlledCabinetMode::Id);
}

static void RvcOperationalStateShutdownCB(void *managed_instance, uint16_t /*endpoint_id*/)
{
    delete static_cast<RvcOperationalState::Instance*>(managed_instance);
}

void RvcOperationalStateDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    cluster_t *cl = cluster::get(endpoint_id, RvcOperationalState::Id);
    VerifyOrReturn(cl != nullptr);
    RvcOperationalState::Instance *instance = static_cast<RvcOperationalState::Instance*>(get_delegate_managed_instance(cl));
    if (!instance) {
        RvcOperationalState::Delegate *rvc_operational_state_delegate = static_cast<RvcOperationalState::Delegate *>(delegate);
        instance = new RvcOperationalState::Instance(rvc_operational_state_delegate, endpoint_id);
        set_delegate_shutdown_callback_and_managed_instance(cl, RvcOperationalStateShutdownCB, instance);
    }
    (void) instance->Init();
}
void TargetNavigatorDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    TargetNavigator::Delegate *target_navigator_delegate = static_cast<TargetNavigator::Delegate *>(delegate);
    TargetNavigator::SetDefaultDelegate(endpoint_id, target_navigator_delegate);
}
void WakeOnLanDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    WakeOnLan::Delegate *wake_on_lan_delegate = static_cast<WakeOnLan::Delegate *>(delegate);
    WakeOnLan::SetDefaultDelegate(endpoint_id, wake_on_lan_delegate);
}

void TlsClientManagementDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    TLSClientManagementDelegate *tls_client_management_delegate = static_cast<TLSClientManagementDelegate *>(delegate);
    TlsClientManagement::SetDelegate(endpoint_id, *tls_client_management_delegate);
}
void TlsCertificateManagementDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    TLSCertificateManagementDelegate *tls_certificate_management_delegate = static_cast<TLSCertificateManagementDelegate *>(delegate);
    TlsCertificateManagement::SetDelegate(endpoint_id, *tls_certificate_management_delegate);
}
} // namespace delegate_cb
} // namespace cluster
} // namespace esp_matter
