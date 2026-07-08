// Copyright 2025 Espressif Systems (Shanghai) PTE LTD
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
#include <app/clusters/thread-network-diagnostics-server/DirectThreadNetworkDiagnosticsProvider.h>
#include <app/clusters/thread-network-diagnostics-server/ThreadNetworkDiagnosticsCluster.h>
#include <app/server-cluster/ServerClusterInterfaceRegistry.h>
#include <clusters/ThreadNetworkDiagnostics/ClusterId.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <data_model/esp_matter_attribute_helpers.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace esp_matter;

namespace {

std::unordered_map<EndpointId, LazyRegisteredServerCluster<ThreadNetworkDiagnosticsCluster>> gServers;

ThreadNetworkDiagnostics::DirectThreadNetworkDiagnosticsProvider  &GetDirectProvider()
{
    static ThreadNetworkDiagnostics::DirectThreadNetworkDiagnosticsProvider sDirectProvider;
    return sDirectProvider;
}

} // namespace

void ESPMatterThreadNetworkDiagnosticsClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(cluster::get(endpointId, ThreadNetworkDiagnostics::Id) != nullptr,
                   ChipLogError(AppServer, "ThreadNetworkDiagnostics: cluster missing in esp-matter data model for endpoint %u",
                                endpointId));
    if (!gServers[endpointId].IsConstructed()) {
        const uint32_t rawFeatureMap = read_feature_map_u32(endpointId, ThreadNetworkDiagnostics::Id);
        VerifyOrDie(rawFeatureMap == 0 || rawFeatureMap == ThreadNetworkDiagnostics::kFeaturesAll.Raw());

        const auto cluster_type = rawFeatureMap == 0 ? ThreadNetworkDiagnosticsCluster::ClusterType::kMinimal
                                  : ThreadNetworkDiagnosticsCluster::ClusterType::kFull;

        gServers[endpointId].Create(endpointId, cluster_type, GetDirectProvider());
    }

    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(
                         gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer,
                     "Failed to register ThreadNetworkDiagnostics on endpoint %u - Error: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
}

void ESPMatterThreadNetworkDiagnosticsClusterServerShutdownCallback(EndpointId endpointId,
                                                                    ClusterShutdownType shutdownType)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end());
    VerifyOrReturn(it->second.IsConstructed());
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&it->second.Cluster(),
                                                                                            shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer,
                     "Failed to unregister ThreadNetworkDiagnostics on endpoint %u - Error: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.Destroy();
        gServers.erase(it);
    }
}

void MatterThreadNetworkDiagnosticsPluginServerInitCallback() {}
