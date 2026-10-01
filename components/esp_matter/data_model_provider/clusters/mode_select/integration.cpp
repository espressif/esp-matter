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

#include <app-common/zap-generated/attributes/Accessors.h>
#include <app/ClusterCallbacks.h>
#include <app/clusters/mode-select-server/ModeSelectCluster.h>
#include <app/clusters/mode-select-server/SupportedModesManager.h>
#include <clusters/ModeSelect/ClusterId.h>
#include <data_model/esp_matter_attribute_helpers.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/DiagnosticDataProvider.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::Protocols::InteractionModel;
using namespace esp_matter;

namespace {

// Adapts the application-set SupportedModesManager to the per-endpoint delegate
// the code-driven ModeSelectCluster expects.
class SupportedModesManagerDelegate : public ModeSelectCluster::Delegate {
public:
    void SetEndpointId(EndpointId endpointId)
    {
        mEndpointId = endpointId;
    }

    Span<const ModeSelect::Structs::ModeOptionStruct::Type> GetSupportedModes() const override
    {
        const ModeSelect::SupportedModesManager *mgr = ModeSelect::getSupportedModesManager();
        VerifyOrReturnValue(mgr != nullptr, Span<const ModeSelect::Structs::ModeOptionStruct::Type>());
        auto provider = mgr->getModeOptionsProvider(mEndpointId);
        VerifyOrReturnValue(provider.begin() != nullptr, Span<const ModeSelect::Structs::ModeOptionStruct::Type>());
        return Span<const ModeSelect::Structs::ModeOptionStruct::Type>(provider.begin(),
                                                                       static_cast<size_t>(provider.end() - provider.begin()));
    }

private:
    EndpointId mEndpointId = kInvalidEndpointId;
};

// Re-exposes the protected ApplyStartupModeLogic() so the manager-set callback can
// run it for clusters that started before the application set the manager.
class ESPModeSelectCluster : public ModeSelectCluster {
public:
    using ModeSelectCluster::ApplyStartupModeLogic;
    using ModeSelectCluster::ModeSelectCluster;
};

constexpr size_t k_max_description_length = 64;

struct ModeSelectEntry {
    char description_buffer[k_max_description_length]; // persistent backing store for the Description CharSpan
    SupportedModesManagerDelegate delegate;
    LazyRegisteredServerCluster<ESPModeSelectCluster> server;
};

std::unordered_map<EndpointId, ModeSelectEntry> gServers;

} // namespace

void ESPMatterModeSelectClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(cluster::get(endpointId, ModeSelect::Id) != nullptr,
                   ChipLogError(AppServer, "ModeSelect: cluster missing in esp-matter data model for endpoint %u", endpointId));

    auto &entry = gServers[endpointId];
    if (!entry.server.IsConstructed()) {
        entry.delegate.SetEndpointId(endpointId);

        BitMask<ModeSelect::Feature> features(read_feature_map_u32(endpointId, ModeSelect::Id));

        ModeSelectCluster::OptionalAttributeSet optionalAttributeSet(0);
        if (endpoint::is_attribute_enabled(endpointId, ModeSelect::Id, ModeSelect::Attributes::StartUpMode::Id)) {
            optionalAttributeSet.Set<ModeSelect::Attributes::StartUpMode::Id>();
        }

        // te2's Description::GetDefault fills a CharSpan pointing at metadata default
        // storage; copy it into our persistent per-endpoint buffer so the cluster's
        // Config.description backing memory stays owned by gServers.
        MutableCharSpan descSpan(entry.description_buffer, sizeof(entry.description_buffer));
        CharSpan defaultDesc;
        if (ModeSelect::Attributes::Description::GetDefault(endpointId, defaultDesc) != Status::Success ||
                CopyCharSpanToMutableCharSpan(defaultDesc, descSpan) != CHIP_NO_ERROR) {
            descSpan = MutableCharSpan(entry.description_buffer, 0);
        }

        DataModel::Nullable<uint16_t> standardNamespace;
        if (ModeSelect::Attributes::StandardNamespace::GetDefault(endpointId, standardNamespace) != Status::Success) {
            standardNamespace.SetNull();
        }

        // TODO: The upstream codegen integration derives onOffValueForStartUp from the
        // OnOff cluster's StartUpOnOff attribute when the OnOff feature is enabled.
        // Mirror that behaviour here once an esp-matter accessor for it is available.
        ModeSelectCluster::Config config{
            .featureMap = features,
            .optionalAttributeSet = optionalAttributeSet,
            .description = CharSpan(entry.description_buffer, descSpan.size()),
            .standardNamespace = standardNamespace,
            .onOffValueForStartUp = false,
            .diagnosticDataProvider = DeviceLayer::GetDiagnosticDataProvider(),
        };

        entry.server.Create(endpointId, entry.delegate, config);
    }

    CHIP_ERROR err = data_model::provider::get_instance().registry().Register(entry.server.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register ModeSelect on endpoint %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
}

void ESPMatterModeSelectClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end());
    VerifyOrReturn(it->second.server.IsConstructed());

    CHIP_ERROR err = data_model::provider::get_instance().registry().Unregister(&it->second.server.Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister ModeSelect on endpoint %u: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }

    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.server.Destroy();
        gServers.erase(it);
    }
}

namespace {

// The application may call setSupportedModesManager() after the clusters have already
// started; re-run the startup mode logic for them once the manager becomes available.
void OnSupportedModesManagerSet(ModeSelect::SupportedModesManager *)
{
    for (auto &entry : gServers) {
        if (entry.second.server.IsConstructed()) {
            entry.second.server.Cluster().ApplyStartupModeLogic();
        }
    }
}

} // namespace

__attribute__((weak)) void MatterModeSelectPluginServerInitCallback()
{
    ModeSelect::internal::RegisterOnManagerSetCallback(OnSupportedModesManagerSet);
}

__attribute__((weak)) void MatterModeSelectPluginServerShutdownCallback()
{
    ModeSelect::internal::RegisterOnManagerSetCallback(nullptr);
}
