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
#include <app/clusters/wifi-network-diagnostics-server/WiFiNetworkDiagnosticsCluster.h>
#include <app/server-cluster/ServerClusterInterfaceRegistry.h>
#include <clusters/WiFiNetworkDiagnostics/ClusterId.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <data_model/esp_matter_attribute_helpers.h>
#include <lib/support/logging/CHIPLogging.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace esp_matter;

namespace {
std::unordered_map<EndpointId, LazyRegisteredServerCluster<WiFiDiagnosticsServerCluster>> gServers;

bool IsClusterEnabled(EndpointId endpointId)
{
    cluster_t *cluster = cluster::get(endpointId, WiFiNetworkDiagnostics::Id);
    return cluster != nullptr;
}

bool IsAttributeEnabled(EndpointId endpointId, AttributeId attributeId)
{
    return endpoint::is_attribute_enabled(endpointId, WiFiNetworkDiagnostics::Id, attributeId);
}

} // namespace

void ESPMatterWiFiNetworkDiagnosticsClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(IsClusterEnabled(endpointId));
    if (!gServers[endpointId].IsConstructed()) {
        WiFiDiagnosticsServerCluster::OptionalAttributeSet attrSet;
        if (IsAttributeEnabled(endpointId, WiFiNetworkDiagnostics::Attributes::CurrentMaxRate::Id)) {
            attrSet.Set<WiFiNetworkDiagnostics::Attributes::CurrentMaxRate::Id>();
        }

        // NOTE: Currently, diagnostics only support a single provider (DeviceLayer::GetDiagnosticDataProvider())
        // and do not properly support secondary network interfaces or per-endpoint diagnostics.
        // See issue:#40317
        gServers[endpointId].Create(endpointId, DeviceLayer::GetDiagnosticDataProvider(), attrSet,
                                    BitFlags<WiFiNetworkDiagnostics::Feature>(
                                        read_feature_map_u32(endpointId, WiFiNetworkDiagnostics::Id)));
    }
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(
                         gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register WiFiNetworkDiagnostics on endpoint %u - Error: %" CHIP_ERROR_FORMAT,
                     endpointId,
                     err.Format());
    }
}

// This callback is called for any endpoint (fixed or dynamic) that is registered with the Ember machinery.
void ESPMatterWiFiNetworkDiagnosticsClusterServerShutdownCallback(EndpointId endpointId,
                                                                  ClusterShutdownType shutdownType)
{
    VerifyOrReturn(IsClusterEnabled(endpointId));
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end());
    VerifyOrReturn(it->second.IsConstructed());
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&it->second.Cluster(),
                                                                                            shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister WiFiNetworkDiagnostics on endpoint %u - Error: %" CHIP_ERROR_FORMAT,
                     endpointId,
                     err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.Destroy();
        gServers.erase(it);
    }
}

void MatterWiFiNetworkDiagnosticsPluginServerInitCallback() {}

void MatterWiFiNetworkDiagnosticsPluginServerShutdownCallback() {}
