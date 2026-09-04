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

#include <esp_matter_data_model_priv.h>

#include <app/ClusterCallbacks.h>
#include <app/clusters/laundry-washer-controls-server/CodegenIntegration.h>
#include <app/clusters/laundry-washer-controls-server/LaundryWasherControlsCluster.h>
#include <app/clusters/laundry-washer-controls-server/laundry-washer-controls-delegate.h>
#include <clusters/LaundryWasherControls/ClusterId.h>
#include <data_model/esp_matter_attribute_helpers.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace esp_matter;

namespace {

std::unordered_map<EndpointId, LazyRegisteredServerCluster<LaundryWasherControlsCluster>> gServers;

} // namespace

void ESPMatterLaundryWasherControlsClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(cluster::get(endpointId, LaundryWasherControls::Id) != nullptr,
                   ChipLogError(AppServer, "LaundryWasherControls: cluster missing in esp-matter data model for endpoint %u",
                                endpointId));

    auto  &entry = gServers[endpointId];
    if (!entry.IsConstructed()) {
        BitFlags<LaundryWasherControls::Feature> features(read_feature_map_u32(endpointId, LaundryWasherControls::Id));
        LaundryWasherControlsCluster::Config config(features);
        entry.Create(endpointId, config);
    }

    CHIP_ERROR err = data_model::provider::get_instance().registry().Register(entry.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register LaundryWasherControls on endpoint %u: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }
}

void ESPMatterLaundryWasherControlsClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end());
    VerifyOrReturn(it->second.IsConstructed());

    CHIP_ERROR err = data_model::provider::get_instance().registry().Unregister(&it->second.Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister LaundryWasherControls on endpoint %u: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }

    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.Destroy();
        gServers.erase(it);
    }
}

__attribute__((weak)) void MatterLaundryWasherControlsPluginServerInitCallback() {}
__attribute__((weak)) void MatterLaundryWasherControlsPluginServerShutdownCallback() {}

namespace chip::app::Clusters::LaundryWasherControls {

LaundryWasherControlsCluster * FindClusterOnEndpoint(EndpointId endpoint)
{
    auto it = gServers.find(endpoint);
    if (it == gServers.end() || !it->second.IsConstructed()) {
        return nullptr;
    }
    return &it->second.Cluster();
}

namespace LaundryWasherControlsServer {

void SetDefaultDelegate(EndpointId endpoint, Delegate * delegate)
{
    VerifyOrDie(delegate != nullptr);
    SetDelegate(endpoint, *delegate);
}

void SetDelegate(EndpointId endpoint, Delegate  &delegate)
{
    auto cluster = FindClusterOnEndpoint(endpoint);
    if (cluster != nullptr) {
        cluster->SetDelegate(delegate);
    } else {
        ChipLogError(Zcl, "LaundryWasherControls cluster on endpoint %d not found", endpoint);
    }
}

CHIP_ERROR SetSpinSpeedCurrent(EndpointId endpointId, DataModel::Nullable<uint8_t> spinSpeedCurrent)
{
    auto cluster = FindClusterOnEndpoint(endpointId);
    return cluster != nullptr ? cluster->SetSpinSpeedCurrent(spinSpeedCurrent) : CHIP_ERROR_NOT_FOUND;
}

CHIP_ERROR GetSpinSpeedCurrent(EndpointId endpointId, DataModel::Nullable<uint8_t>  &spinSpeedCurrent)
{
    auto cluster = FindClusterOnEndpoint(endpointId);
    VerifyOrReturnError(cluster != nullptr, CHIP_ERROR_NOT_FOUND);
    spinSpeedCurrent = cluster->GetSpinSpeedCurrent();
    return CHIP_NO_ERROR;
}

CHIP_ERROR SetNumberOfRinses(EndpointId endpointId, NumberOfRinsesEnum newNumberOfRinses)
{
    auto cluster = FindClusterOnEndpoint(endpointId);
    return cluster != nullptr ? cluster->SetNumberOfRinses(newNumberOfRinses) : CHIP_ERROR_NOT_FOUND;
}

CHIP_ERROR GetNumberOfRinses(EndpointId endpointId, NumberOfRinsesEnum  &numberOfRinses)
{
    auto cluster = FindClusterOnEndpoint(endpointId);
    VerifyOrReturnError(cluster != nullptr, CHIP_ERROR_NOT_FOUND);
    numberOfRinses = cluster->GetNumberOfRinses();
    return CHIP_NO_ERROR;
}

} // namespace LaundryWasherControlsServer

} // namespace chip::app::Clusters::LaundryWasherControls
