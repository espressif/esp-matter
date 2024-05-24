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

} // namespace delegate_cb

} // namespace cluster
} // namespace esp_matter
