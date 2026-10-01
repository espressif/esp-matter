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
#include <app/clusters/laundry-dryer-controls-server/LaundryDryerControlsCluster.h>
#include <app/clusters/laundry-dryer-controls-server/laundry-dryer-controls-delegate.h>
#include <clusters/LaundryDryerControls/ClusterId.h>
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

// Forwards to the application delegate once it is set via SetDefaultDelegate(); the
// cluster requires a delegate reference at construction, before the app can provide one.
class DryerDelegateWrapper final : public LaundryDryerControls::Delegate {
public:
    void SetWrapped(LaundryDryerControls::Delegate *wrapped)
    {
        mWrapped = wrapped;
    }

    CHIP_ERROR GetSupportedDrynessLevelAtIndex(size_t index, LaundryDryerControls::DrynessLevelEnum &supportedDryness) override
    {
        VerifyOrReturnError(mWrapped != nullptr, CHIP_ERROR_PROVIDER_LIST_EXHAUSTED);
        return mWrapped->GetSupportedDrynessLevelAtIndex(index, supportedDryness);
    }

private:
    LaundryDryerControls::Delegate *mWrapped = nullptr;
};

struct LaundryDryerControlsEntry {
    DryerDelegateWrapper delegate;
    LazyRegisteredServerCluster<LaundryDryerControlsCluster> server;
};

std::unordered_map<EndpointId, LaundryDryerControlsEntry> gServers;

} // namespace

void ESPMatterLaundryDryerControlsClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(cluster::get(endpointId, LaundryDryerControls::Id) != nullptr,
                   ChipLogError(AppServer, "LaundryDryerControls: cluster missing in esp-matter data model for endpoint %u",
                                endpointId));

    auto &entry = gServers[endpointId];
    if (!entry.server.IsConstructed()) {
        entry.server.Create(endpointId, entry.delegate);
    }

    CHIP_ERROR err = data_model::provider::get_instance().registry().Register(entry.server.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register LaundryDryerControls on endpoint %u: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }
}

void ESPMatterLaundryDryerControlsClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end());
    VerifyOrReturn(it->second.server.IsConstructed());

    CHIP_ERROR err = data_model::provider::get_instance().registry().Unregister(&it->second.server.Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister LaundryDryerControls on endpoint %u: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }

    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.server.Destroy();
        gServers.erase(it);
    }
}

__attribute__((weak)) void MatterLaundryDryerControlsPluginServerInitCallback() {}
__attribute__((weak)) void MatterLaundryDryerControlsPluginServerShutdownCallback() {}

namespace chip::app::Clusters::LaundryDryerControls {

LaundryDryerControlsCluster * FindClusterOnEndpoint(EndpointId endpoint)
{
    auto it = gServers.find(endpoint);
    if (it == gServers.end() || !it->second.server.IsConstructed()) {
        return nullptr;
    }
    return &it->second.server.Cluster();
}

namespace LaundryDryerControlsServer {

void SetDelegate(EndpointId endpoint, Delegate &delegate)
{
    auto it = gServers.find(endpoint);
    VerifyOrReturn(it != gServers.end() && it->second.server.IsConstructed(),
                   ChipLogError(Zcl, "LaundryDryerControls cluster on endpoint %d not found", endpoint));
    it->second.delegate.SetWrapped(&delegate);
}

void SetDefaultDelegate(EndpointId endpoint, Delegate *delegate)
{
    VerifyOrDie(delegate != nullptr);
    SetDelegate(endpoint, *delegate);
}

Protocols::InteractionModel::Status SetSelectedDrynessLevel(EndpointId endpointId, DrynessLevelEnum newSelectedDrynessLevel)
{
    auto cluster = FindClusterOnEndpoint(endpointId);
    VerifyOrReturnValue(cluster != nullptr, Protocols::InteractionModel::Status::Failure);
    return cluster->SetSelectedDrynessLevel(DataModel::MakeNullable(newSelectedDrynessLevel));
}

Protocols::InteractionModel::Status GetSelectedDrynessLevel(EndpointId endpointId,
                                                            DataModel::Nullable<DrynessLevelEnum> &selectedDrynessLevel)
{
    auto cluster = FindClusterOnEndpoint(endpointId);
    VerifyOrReturnValue(cluster != nullptr, Protocols::InteractionModel::Status::Failure);
    selectedDrynessLevel = cluster->GetSelectedDrynessLevel();
    return Protocols::InteractionModel::Status::Success;
}

} // namespace LaundryDryerControlsServer

} // namespace chip::app::Clusters::LaundryDryerControls
