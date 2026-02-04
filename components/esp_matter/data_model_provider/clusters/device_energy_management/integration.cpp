// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
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

#include <app/util/generic-callbacks.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <protocols/interaction_model/StatusCode.h>

#include "integration.h"

namespace chip {
namespace app {
namespace Clusters {
namespace DeviceEnergyManagement {

Instance::Instance(EndpointId aEndpointId, Delegate  &aDelegate, BitMask<Feature> aFeature) :
    mCluster(DeviceEnergyManagementCluster::Config(aEndpointId, aFeature, aDelegate))
{}

CHIP_ERROR Instance::Init()
{
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(mCluster.Registration());

    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register cluster %u/" ChipLogFormatMEI ": %" CHIP_ERROR_FORMAT,
                     mCluster.Cluster().GetPaths()[0].mEndpointId, ChipLogValueMEI(DeviceEnergyManagement::Id), err.Format());
    }
    return err;
}

void Instance::Shutdown()
{
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&mCluster.Cluster());

    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister cluster %u/" ChipLogFormatMEI ": %" CHIP_ERROR_FORMAT,
                     mCluster.Cluster().GetPaths()[0].mEndpointId, ChipLogValueMEI(DeviceEnergyManagement::Id), err.Format());
    }
}

bool Instance::HasFeature(Feature aFeature) const
{
    return mCluster.Cluster().Features().Has(aFeature);
}

} // namespace DeviceEnergyManagement
} // namespace Clusters
} // namespace app
} // namespace chip

void ESPMatterDeviceEnergyManagementClusterServerInitCallback(chip::EndpointId endpoint) {}
void ESPMatterDeviceEnergyManagementClusterServerShutdownCallback(chip::EndpointId endpoint, chip::app::ClusterShutdownType shutdownType) {}

// Legacy callback stubs
void MatterDeviceEnergyManagementPluginServerInitCallback() {}
void MatterDeviceEnergyManagementPluginServerShutdownCallback() {}
