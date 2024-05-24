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
#include <app/clusters/mode-base-server/mode-base-server.h>
#include <app/clusters/energy-evse-server/energy-evse-server.h>
#include <app/clusters/microwave-oven-control-server/microwave-oven-control-server.h>
#include <app/clusters/operational-state-server/operational-state-server.h>
#include <app/clusters/resource-monitoring-server/resource-monitoring-server.h>
#include <app/clusters/fan-control-server/fan-control-server.h>
#include <app/clusters/laundry-dryer-controls-server/laundry-dryer-controls-server.h>

using namespace chip::app::Clusters;
namespace esp_matter {
namespace cluster {

static uint32_t get_feature_map_value(uint16_t endpoint_id, uint32_t cluster_id)
{
    node_t *node = node::get();
    endpoint_t *endpoint = endpoint::get(node, endpoint_id);
    cluster_t *cluster = cluster::get(endpoint, cluster_id);
    uint32_t attribute_id = Globals::Attributes::FeatureMap::Id;
    attribute_t *attribute = attribute::get(cluster, attribute_id);

    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    attribute::get_val(attribute, &val);
    return val.val.u32;
}

namespace delegate_cb {

void InitModeDelegate(void *delegate, uint16_t endpoint_id, uint32_t cluster_id)
{
    if(delegate == nullptr)
    {
        return;
    }
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

void EnergyEvseModeDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    InitModeDelegate(delegate, endpoint_id, EnergyEvseMode::Id);
}

void MicrowaveOvenModeDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    InitModeDelegate(delegate, endpoint_id, MicrowaveOvenMode::Id);
}

void EnergyEvseDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    if(delegate == nullptr)
    {
        return;
    }
    static EnergyEvse::Instance * energyEvseInstance = nullptr;
    EnergyEvse::Delegate *energy_evse_delegate = static_cast<EnergyEvse::Delegate*>(delegate);
    uint32_t feature_map = get_feature_map_value(endpoint_id, EnergyEvse::Id);
    energyEvseInstance = new EnergyEvse::Instance(endpoint_id, *energy_evse_delegate, chip::BitMask<EnergyEvse::Feature, uint32_t>(feature_map),
                            chip::BitMask<EnergyEvse::OptionalAttributes, uint32_t>(), chip::BitMask<EnergyEvse::OptionalCommands, uint32_t>());
    energyEvseInstance->Init();
}

void MicrowaveOvenControlDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    // Get delegates of MicrowaveOvenMode and OperationalState clusters.
    node_t *node = node::get();
    endpoint_t *endpoint = endpoint::get(node, endpoint_id);
    cluster_t *cluster = cluster::get(endpoint, MicrowaveOvenMode::Id);
    ModeBase::Delegate *microwave_oven_mode_delegate = static_cast<ModeBase::Delegate*>(get_delegate_impl(cluster));
    cluster = cluster::get(endpoint, OperationalState::Id);
    OperationalState::Delegate *operational_state_delegate = static_cast<OperationalState::Delegate*>(get_delegate_impl(cluster));
    if(delegate == nullptr || microwave_oven_mode_delegate == nullptr || operational_state_delegate == nullptr)
    {
        return;
    }
    // Create instances of clusters.
    static ModeBase::Instance * microwaveOvenModeInstance = nullptr;
    static OperationalState::Instance * operationalStateInstance = nullptr;
    static MicrowaveOvenControl::Instance * microwaveOvenControlInstance = nullptr;
    MicrowaveOvenControl::Delegate *microwave_oven_control_delegate = static_cast<MicrowaveOvenControl::Delegate*>(delegate);
    uint32_t feature_map = get_feature_map_value(endpoint_id, MicrowaveOvenMode::Id);

    microwaveOvenModeInstance = new ModeBase::Instance(microwave_oven_mode_delegate, endpoint_id, MicrowaveOvenMode::Id, feature_map);
    operationalStateInstance = new OperationalState::Instance(operational_state_delegate, endpoint_id);

    feature_map = get_feature_map_value(endpoint_id, MicrowaveOvenControl::Id);
    microwaveOvenControlInstance = new MicrowaveOvenControl::Instance(microwave_oven_control_delegate, endpoint_id, MicrowaveOvenControl::Id, feature_map,
                                        *operationalStateInstance, *microwaveOvenModeInstance);
    microwaveOvenControlInstance->Init();
}

void OperationalStateDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    if(delegate == nullptr)
    {
        return;
    }
    static OperationalState::Instance * operationalStateInstance = nullptr;
    OperationalState::Delegate *operational_state_delegate = static_cast<OperationalState::Delegate*>(delegate);
    operationalStateInstance = new OperationalState::Instance(operational_state_delegate, endpoint_id);
    operationalStateInstance->Init();
}

void FanControlDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    if(delegate == nullptr)
    {
        return;
    }
    FanControl::Delegate *fan_control_delegate = static_cast<FanControl::Delegate*>(delegate);
    FanControl::SetDefaultDelegate(endpoint_id, fan_control_delegate);
}

void HepaFilterMonitoringDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    if(delegate == nullptr)
    {
        return;
    }
    static ResourceMonitoring::Instance * hepaFilterMonitoringInstance = nullptr;
    ResourceMonitoring::Delegate *resource_monitoring_delegate = static_cast<ResourceMonitoring::Delegate*>(delegate);
    uint32_t feature_map = get_feature_map_value(endpoint_id, HepaFilterMonitoring::Id);
    hepaFilterMonitoringInstance = new ResourceMonitoring::Instance(resource_monitoring_delegate, endpoint_id, HepaFilterMonitoring::Id,
                                        static_cast<uint32_t>(feature_map), ResourceMonitoring::DegradationDirectionEnum::kDown, true);
    hepaFilterMonitoringInstance->Init();
}

void ActivatedCarbonFilterMonitoringDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    if(delegate == nullptr)
    {
        return;
    }
    static ResourceMonitoring::Instance * activatedCarbonFilterMonitoringInstance = nullptr;
    ResourceMonitoring::Delegate *resource_monitoring_delegate = static_cast<ResourceMonitoring::Delegate*>(delegate);
    uint32_t feature_map = get_feature_map_value(endpoint_id, ActivatedCarbonFilterMonitoring::Id);
    activatedCarbonFilterMonitoringInstance = new ResourceMonitoring::Instance(resource_monitoring_delegate, endpoint_id, ActivatedCarbonFilterMonitoring::Id,
                                            static_cast<uint32_t>(feature_map), ResourceMonitoring::DegradationDirectionEnum::kDown, true);
    activatedCarbonFilterMonitoringInstance->Init();
}

void LaundryDryerControlsDelegateInitCB(void *delegate, uint16_t endpoint_id)
{
    if(delegate == nullptr)
    {
        return;
    }
    LaundryDryerControls::Delegate *laundry_dryer_controls_delegate = static_cast<LaundryDryerControls::Delegate*>(delegate);
    LaundryDryerControls::LaundryDryerControlsServer::SetDefaultDelegate(endpoint_id, laundry_dryer_controls_delegate);
}

} // namespace delegate_cb

} // namespace cluster
} // namespace esp_matter
