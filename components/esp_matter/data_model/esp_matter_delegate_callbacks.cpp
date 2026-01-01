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
#include <app/clusters/energy-evse-server/energy-evse-server.h>
#include <app/clusters/microwave-oven-control-server/microwave-oven-control-server.h>
#include <app/clusters/operational-state-server/operational-state-server.h>
#include <app/clusters/resource-monitoring-server/resource-monitoring-server.h>
#include <app/clusters/fan-control-server/fan-control-server.h>
#include <app/clusters/laundry-dryer-controls-server/laundry-dryer-controls-server.h>
#include <app/clusters/valve-configuration-and-control-server/valve-configuration-and-control-cluster.h>
#include <app/clusters/device-energy-management-server/device-energy-management-server.h>
#include <app/clusters/door-lock-server/door-lock-server.h>
#include <app/clusters/boolean-state-configuration-server/boolean-state-configuration-server.h>
#include <app/clusters/time-synchronization-server/time-synchronization-cluster.h>
#include <app/clusters/application-basic-server/application-basic-server.h>
#include <app/clusters/power-topology-server/power-topology-server.h>
#include <app/clusters/electrical-power-measurement-server/electrical-power-measurement-server.h>
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
#include <app/clusters/ota-provider/ota-provider-cluster.h>
#include <app/clusters/ota-provider/CodegenIntegration.h>
#include <app/clusters/diagnostic-logs-server/diagnostic-logs-server.h>
#include <app/clusters/chime-server/chime-server.h>
#include <app/clusters/closure-control-server/closure-control-server.h>
#include <app/clusters/closure-dimension-server/closure-dimension-server.h>
#include <app/clusters/push-av-stream-transport-server/push-av-stream-transport-cluster.h>
#include <app/clusters/commodity-tariff-server/commodity-tariff-server.h>
#include <app/clusters/commodity-price-server/commodity-price-server.h>
#include <app/clusters/electrical-grid-conditions-server/electrical-grid-conditions-server.h>
#include <app/clusters/meter-identification-server/meter-identification-server.h>
#include <unordered_map>

using namespace chip::app::Clusters;
namespace esp_matter {
namespace cluster {

static std::unordered_map<chip::EndpointId, ModeBase::Instance*> s_microwave_oven_mode_instances;
static std::unordered_map<chip::EndpointId, OperationalState::Instance*> s_operational_state_instances;

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

chip::BitMask<PowerTopology::OptionalAttributes> get_power_topology_enabled_optional_attributes(uint16_t endpoint_id)
{
    chip::BitMask<PowerTopology::OptionalAttributes> optional_attrs = 0;
    
    if (endpoint::is_attribute_enabled(endpoint_id, PowerTopology::Id, PowerTopology::Attributes::AvailableEndpoints::Id)) {
        optional_attrs.Set(PowerTopology::OptionalAttributes::kOptionalAttributeAvailableEndpoints);
    }
    
    if (endpoint::is_attribute_enabled(endpoint_id, PowerTopology::Id, PowerTopology::Attributes::ActiveEndpoints::Id)) {
        optional_attrs.Set(PowerTopology::OptionalAttributes::kOptionalAttributeActiveEndpoints);
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

void InitModeDelegate(void *delegate, uint16_t endpoint_id, uint32_t cluster_id)
{
    VerifyOrReturn(delegate != nullptr);
    static ModeBase::Instance * modeInstance = nullptr;
    ModeBase::Delegate *mode_delegate = static_cast<ModeBase::Delegate*>(delegate);
    uint32_t feature_map = get_feature_map_value(endpoint_id, cluster_id);
    modeInstance = new ModeBase::Instance(mode_delegate, endpoint_id, cluster_id, feature_map);
    modeInstance->Init();
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
    VerifyOrReturn(delegate != nullptr);
    ModeBase::Delegate *mode_delegate = static_cast<ModeBase::Delegate*>(delegate);
    ModeBase::Instance * modeInstance = nullptr;
    // Create new instance of MicrowaveOvenMode if not found in the map, otherwise use existing instance.
    if(s_microwave_oven_mode_instances.find(endpoint_id) == s_microwave_oven_mode_instances.end())
    {
        uint32_t feature_map = get_feature_map_value(endpoint_id, MicrowaveOvenMode::Id);
        modeInstance = new ModeBase::Instance(mode_delegate, endpoint_id, MicrowaveOvenMode::Id, feature_map);
        s_microwave_oven_mode_instances[endpoint_id] = modeInstance;
    } else {
        modeInstance = s_microwave_oven_mode_instances[endpoint_id];
    }
    modeInstance->Init();
}

void DeviceEnergyManagementModeDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    InitModeDelegate(delegate, endpoint_id, DeviceEnergyManagementMode::Id);
}

void EnergyEvseDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    static EnergyEvse::Instance * energyEvseInstance = nullptr;
    EnergyEvse::Delegate *energy_evse_delegate = static_cast<EnergyEvse::Delegate*>(delegate);
    uint32_t feature_map = get_feature_map_value(endpoint_id, EnergyEvse::Id);
    chip::BitMask<EnergyEvse::OptionalAttributes> optional_attrs = get_energy_evse_enabled_optional_attributes(endpoint_id);
    chip::BitMask<EnergyEvse::OptionalCommands> optional_cmds = get_energy_evse_enabled_optional_commands(endpoint_id);
    energyEvseInstance = new EnergyEvse::Instance(endpoint_id, *energy_evse_delegate, chip::BitMask<EnergyEvse::Feature, uint32_t>(feature_map),
                            optional_attrs, optional_cmds);
    energyEvseInstance->Init();
}

void MicrowaveOvenControlDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    // Get delegates of MicrowaveOvenMode and OperationalState clusters.
    cluster_t *cluster = cluster::get(endpoint_id, MicrowaveOvenMode::Id);
    ModeBase::Delegate *microwave_oven_mode_delegate = static_cast<ModeBase::Delegate*>(get_delegate_impl(cluster));
    cluster = cluster::get(endpoint_id, OperationalState::Id);
    OperationalState::Delegate *operational_state_delegate = static_cast<OperationalState::Delegate*>(get_delegate_impl(cluster));
    VerifyOrReturn(delegate != nullptr && microwave_oven_mode_delegate != nullptr && operational_state_delegate != nullptr);
    ModeBase::Instance* microwaveOvenModeInstance = nullptr;
    OperationalState::Instance* operationalStateInstance = nullptr;
    
    // Create new instance of MicrowaveOvenMode if not found in the map, otherwise use existing instance.
    if(s_microwave_oven_mode_instances.find(endpoint_id) == s_microwave_oven_mode_instances.end())
    {
        uint32_t feature_map = get_feature_map_value(endpoint_id, MicrowaveOvenMode::Id);
        microwaveOvenModeInstance = new ModeBase::Instance(microwave_oven_mode_delegate, endpoint_id, MicrowaveOvenMode::Id, feature_map);
        s_microwave_oven_mode_instances[endpoint_id] = microwaveOvenModeInstance;
    } else {
        microwaveOvenModeInstance = s_microwave_oven_mode_instances[endpoint_id];
    }
    
    // Create new instance of OperationalState if not found in the map, otherwise use existing instance.
    if(s_operational_state_instances.find(endpoint_id) == s_operational_state_instances.end())
    {
        operationalStateInstance = new OperationalState::Instance(operational_state_delegate, endpoint_id);
        s_operational_state_instances[endpoint_id] = operationalStateInstance;
    } else {
        operationalStateInstance = s_operational_state_instances[endpoint_id];
    }
    
    // Create MicrowaveOvenControl instance
    static MicrowaveOvenControl::Instance * microwaveOvenControlInstance = nullptr;
    MicrowaveOvenControl::Delegate *microwave_oven_control_delegate = static_cast<MicrowaveOvenControl::Delegate*>(delegate);
    uint32_t feature_map = get_feature_map_value(endpoint_id, MicrowaveOvenControl::Id);
    microwaveOvenControlInstance = new MicrowaveOvenControl::Instance(microwave_oven_control_delegate, endpoint_id, MicrowaveOvenControl::Id, feature_map,
                                        *operationalStateInstance, *microwaveOvenModeInstance);
    microwaveOvenControlInstance->Init();
}

void OperationalStateDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    static OperationalState::Instance * operationalStateInstance = nullptr;
    OperationalState::Delegate *operational_state_delegate = static_cast<OperationalState::Delegate*>(delegate);
    // Create new instance of OperationalState if not found in the map, otherwise use existing instance.
    if(s_operational_state_instances.find(endpoint_id) == s_operational_state_instances.end())
    {
        operationalStateInstance = new OperationalState::Instance(operational_state_delegate, endpoint_id);
        s_operational_state_instances[endpoint_id] = operationalStateInstance;
    } else {
        operationalStateInstance = s_operational_state_instances[endpoint_id];
    }
    operationalStateInstance->Init();
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
    static ResourceMonitoring::Instance * hepaFilterMonitoringInstance = nullptr;
    ResourceMonitoring::Delegate *resource_monitoring_delegate = static_cast<ResourceMonitoring::Delegate*>(delegate);
    uint32_t feature_map = get_feature_map_value(endpoint_id, HepaFilterMonitoring::Id);
    hepaFilterMonitoringInstance = new ResourceMonitoring::Instance(resource_monitoring_delegate, endpoint_id, HepaFilterMonitoring::Id,
                                        static_cast<uint32_t>(feature_map), ResourceMonitoring::DegradationDirectionEnum::kDown, true);
    hepaFilterMonitoringInstance->Init();
}

void ActivatedCarbonFilterMonitoringDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    static ResourceMonitoring::Instance * activatedCarbonFilterMonitoringInstance = nullptr;
    ResourceMonitoring::Delegate *resource_monitoring_delegate = static_cast<ResourceMonitoring::Delegate*>(delegate);
    uint32_t feature_map = get_feature_map_value(endpoint_id, ActivatedCarbonFilterMonitoring::Id);
    activatedCarbonFilterMonitoringInstance = new ResourceMonitoring::Instance(resource_monitoring_delegate, endpoint_id, ActivatedCarbonFilterMonitoring::Id,
                                            static_cast<uint32_t>(feature_map), ResourceMonitoring::DegradationDirectionEnum::kDown, true);
    activatedCarbonFilterMonitoringInstance->Init();
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

void DeviceEnergyManagementDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    static DeviceEnergyManagement::Instance * deviceEnergyManagementInstance = nullptr;
    DeviceEnergyManagement::Delegate *device_energy_management_delegate = static_cast<DeviceEnergyManagement::Delegate*>(delegate);
    uint32_t feature_map = get_feature_map_value(endpoint_id, DeviceEnergyManagement::Id);
    deviceEnergyManagementInstance = new DeviceEnergyManagement::Instance(endpoint_id, *device_energy_management_delegate, chip::BitMask<DeviceEnergyManagement::Feature, uint32_t>(feature_map));
    deviceEnergyManagementInstance->Init();
}

void DoorLockDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    DoorLock::Delegate *door_lock_delegate = static_cast<DoorLock::Delegate*>(delegate);
    DoorLockServer::Instance().SetDelegate(endpoint_id, door_lock_delegate);
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

void PowerTopologyDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    static PowerTopology::Instance * powerTopologyInstance = nullptr;
    PowerTopology::Delegate *power_topology_delegate = static_cast<PowerTopology::Delegate*>(delegate);
    uint32_t feature_map = get_feature_map_value(endpoint_id, PowerTopology::Id);
    chip::BitMask<PowerTopology::OptionalAttributes> optional_attrs = get_power_topology_enabled_optional_attributes(endpoint_id);
    powerTopologyInstance = new PowerTopology::Instance(endpoint_id, *power_topology_delegate, chip::BitMask<PowerTopology::Feature,
                            uint32_t>(feature_map), optional_attrs);
    powerTopologyInstance->Init();
}

void ElectricalPowerMeasurementDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    static ElectricalPowerMeasurement::Instance * electricalPowerMeasurementInstance = nullptr;
    ElectricalPowerMeasurement::Delegate *electrical_power_measurement_delegate = static_cast<ElectricalPowerMeasurement::Delegate*>(delegate);
    uint32_t feature_map = get_feature_map_value(endpoint_id, ElectricalPowerMeasurement::Id);
    chip::BitMask<ElectricalPowerMeasurement::OptionalAttributes> optional_attrs = get_electrical_power_measurement_enabled_optional_attributes(endpoint_id);
    electricalPowerMeasurementInstance = new ElectricalPowerMeasurement::Instance(endpoint_id, *electrical_power_measurement_delegate,
                            chip::BitMask<ElectricalPowerMeasurement::Feature, uint32_t>(feature_map), optional_attrs);
    electricalPowerMeasurementInstance->Init();
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

void ThreadBorderRouterManagementDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    assert(delegate != nullptr);
    /* Get the attribute */
    attribute_t *attribute = attribute::get(endpoint_id, ThreadBorderRouterManagement::Id, Globals::Attributes::FeatureMap::Id);
    assert(attribute != nullptr);
    /* Update the value if the attribute already exists */
    esp_matter_attr_val_t val = esp_matter_invalid(nullptr);
    attribute::get_val(attribute, &val);
    bool pan_change_supported = (val.val.u32 & thread_border_router_management::feature::pan_change::get_id()) ? true : false;
    ThreadBorderRouterManagement::Delegate *thread_br_delegate = static_cast<ThreadBorderRouterManagement::Delegate *>(delegate);
    assert(thread_br_delegate->GetPanChangeSupported() == pan_change_supported);
    ThreadBorderRouterManagement::ServerInstance *server_instance =
        chip::Platform::New<ThreadBorderRouterManagement::ServerInstance>(endpoint_id, thread_br_delegate, chip::Server::GetInstance().GetFailSafeContext());
    server_instance->Init();
}

void ServiceAreaDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    // TODO: This cluster has two delegates. We need to update existing delegate logic to accommodate multiple delegates.
}

void WaterHeaterManagementDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    if(delegate == nullptr)
    {
        return;
    }
    static WaterHeaterManagement::Instance * wHtrInstance = nullptr;
    WaterHeaterManagement::Delegate *whtr_delegate = static_cast<WaterHeaterManagement::Delegate*>(delegate);
    uint32_t feature_map = get_feature_map_value(endpoint_id, WaterHeaterManagement::Id);
    wHtrInstance = new WaterHeaterManagement::Instance(endpoint_id, *whtr_delegate, chip::BitMask<WaterHeaterManagement::Feature, uint32_t>(feature_map));
    wHtrInstance->Init();
}

void EnergyPreferenceDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    if(delegate == nullptr)
    {
        return;
    }
    EnergyPreference::Delegate *energy_preference_delegate = static_cast<EnergyPreference::Delegate*>(delegate);
    EnergyPreference::SetDelegate(energy_preference_delegate);
}

void CommissionerControlDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    if(delegate == nullptr)
    {
        return;
    }
    CommissionerControl::Delegate *commissioner_control_delegate = static_cast<CommissionerControl::Delegate*>(delegate);
    CommissionerControl::CommissionerControlServer *commissioner_control_instance = nullptr;
    commissioner_control_instance =
        new CommissionerControl::CommissionerControlServer(commissioner_control_delegate, endpoint_id);
    commissioner_control_instance->Init();
}

void ActionsDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    static Actions::ActionsServer *actionsServer = nullptr;
    Actions::Delegate *actions_delegate = static_cast<Actions::Delegate*>(delegate);
    actionsServer = new Actions::ActionsServer(endpoint_id, *actions_delegate);
    actionsServer->Init();
}


void ThermostatDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    Thermostat::Delegate *thermostat_delegate = static_cast<Thermostat::Delegate*>(delegate);
    Thermostat::SetDefaultDelegate(endpoint_id, thermostat_delegate);
}

void OtaSoftwareUpdateProviderDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    chip::app::Clusters::OTAProvider::SetDelegate(endpoint_id, static_cast<chip::app::Clusters::OTAProviderDelegate *>(delegate));
}

void DiagnosticLogsDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    DiagnosticLogs::DiagnosticLogsProviderDelegate *diagnostic_logs_delegate = static_cast<DiagnosticLogs::DiagnosticLogsProviderDelegate*>(delegate);
    DiagnosticLogs::DiagnosticLogsServer::Instance().SetDiagnosticLogsProviderDelegate(endpoint_id, diagnostic_logs_delegate);
}

void ChimeDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    ChimeDelegate *chime_delegate = static_cast<ChimeDelegate*>(delegate);
    ChimeServer *chime_server = new ChimeServer(endpoint_id, *chime_delegate);
    chime_server->Init();
}

void ClosureControlDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    ClosureControl::DelegateBase *closure_control_delegate = static_cast<ClosureControl::DelegateBase*>(delegate);
    ClosureControl::MatterContext *matter_context = new ClosureControl::MatterContext(endpoint_id);
    ClosureControl::ClusterLogic *cluster_logic = new ClosureControl::ClusterLogic(*closure_control_delegate, *matter_context);
    ClosureControl::Interface *server_interface = new ClosureControl::Interface(endpoint_id, *cluster_logic);
    server_interface->Init();
}


void ClosureDimensionDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    ClosureDimension::DelegateBase *closure_dimension_delegate = static_cast<ClosureDimension::DelegateBase*>(delegate);
    ClosureDimension::MatterContext *matter_context = new ClosureDimension::MatterContext(endpoint_id);
    ClosureDimension::ClusterLogic *cluster_logic = new ClosureDimension::ClusterLogic(*closure_dimension_delegate, *matter_context);
    ClosureDimension::Interface *server_interface = new ClosureDimension::Interface(endpoint_id, *cluster_logic);
    server_interface->Init();
}


void PushAvStreamTransportDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    static PushAvStreamTransportServer * pushavstreamtransportserverinstance = nullptr;
    PushAvStreamTransportDelegate *push_av_stream_transport_delegate = static_cast<PushAvStreamTransportDelegate*>(delegate);
    uint32_t feature_map = get_feature_map_value(endpoint_id, PushAvStreamTransport::Id);
    pushavstreamtransportserverinstance = new PushAvStreamTransportServer(endpoint_id, chip::BitMask<PushAvStreamTransport::Feature, uint32_t>(feature_map));
    pushavstreamtransportserverinstance->SetDelegate(push_av_stream_transport_delegate);
    pushavstreamtransportserverinstance->Init();
}


void CommodityTariffDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    CommodityTariff::Delegate *commodity_tariff_delegate = static_cast<CommodityTariff::Delegate*>(delegate);
    uint32_t feature_map = get_feature_map_value(endpoint_id, CommodityTariff::Id);
    CommodityTariff::Instance *commodity_tariff_instance = new CommodityTariff::Instance(endpoint_id, *commodity_tariff_delegate, chip::BitMask<CommodityTariff::Feature, uint32_t>(feature_map));
    commodity_tariff_instance->Init();
}


void CommodityPriceDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    CommodityPrice::Delegate *commodity_price_delegate = static_cast<CommodityPrice::Delegate*>(delegate);
    uint32_t feature_map = get_feature_map_value(endpoint_id, CommodityPrice::Id);
    CommodityPrice::Instance *commodity_price_instance = new CommodityPrice::Instance(endpoint_id, *commodity_price_delegate, chip::BitMask<CommodityPrice::Feature, uint32_t>(feature_map));
    commodity_price_instance->Init();
}


void ElectricalGridConditionsDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    VerifyOrReturn(delegate != nullptr);
    ElectricalGridConditions::Delegate *electrical_grid_conditions_delegate = static_cast<ElectricalGridConditions::Delegate*>(delegate);
    uint32_t feature_map = get_feature_map_value(endpoint_id, ElectricalGridConditions::Id);
    ElectricalGridConditions::Instance *electrical_grid_conditions_instance = new ElectricalGridConditions::Instance(endpoint_id, *electrical_grid_conditions_delegate, chip::BitMask<ElectricalGridConditions::Feature, uint32_t>(feature_map));
    electrical_grid_conditions_instance->Init();
}

/* Not a delegate but an Initialization callback */
void MeterIdentificationDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    uint32_t feature_map = get_feature_map_value(endpoint_id, MeterIdentification::Id);
    MeterIdentification::Instance *meter_identification_instance = new MeterIdentification::Instance(endpoint_id, chip::BitMask<MeterIdentification::Feature, uint32_t>(feature_map));
    meter_identification_instance->Init();
}

} // namespace delegate_cb
} // namespace cluster
} // namespace esp_matter
