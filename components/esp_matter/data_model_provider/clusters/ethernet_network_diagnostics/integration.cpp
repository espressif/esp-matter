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

#include <app/ClusterCallbacks.h>
#include <app/clusters/ethernet-network-diagnostics-server/EthernetDiagnosticsCluster.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <esp_matter_data_model.h>
#include <esp_matter_data_model_priv.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;

namespace {
std::unordered_map<EndpointId, LazyRegisteredServerCluster<EthernetDiagnosticsServerCluster>> gServers;

uint32_t get_feature_map(esp_matter::cluster_t *cluster)
{
    esp_matter::attribute_t *attribute = esp_matter::attribute::get(cluster, Globals::Attributes::FeatureMap::Id);
    if (attribute) {
        esp_matter_attr_val_t val = esp_matter_invalid(nullptr);
        if (esp_matter::attribute::get_val_internal(attribute, &val) == ESP_OK &&
                val.type == ESP_MATTER_VAL_TYPE_BITMAP32) {
            return val.val.u32;
        }
    }
    return 0;
}

EthernetDiagnosticsServerCluster::OptionalAttributeSet get_attribute_set(esp_matter::cluster_t *cluster)
{
    EthernetDiagnosticsServerCluster::OptionalAttributeSet ret;
    if (esp_matter::attribute::get(cluster, EthernetNetworkDiagnostics::Attributes::CarrierDetect::Id)) {
        ret.Set<EthernetNetworkDiagnostics::Attributes::CarrierDetect::Id>();
    }
    if (esp_matter::attribute::get(cluster, EthernetNetworkDiagnostics::Attributes::FullDuplex::Id)) {
        ret.Set<EthernetNetworkDiagnostics::Attributes::FullDuplex::Id>();
    }
    if (esp_matter::attribute::get(cluster, EthernetNetworkDiagnostics::Attributes::PHYRate::Id)) {
        ret.Set<EthernetNetworkDiagnostics::Attributes::PHYRate::Id>();
    }
    if (esp_matter::attribute::get(cluster, EthernetNetworkDiagnostics::Attributes::TimeSinceReset::Id)) {
        ret.Set<EthernetNetworkDiagnostics::Attributes::TimeSinceReset::Id>();
    }
    return ret;
}

} // namespace

void ESPMatterEthernetNetworkDiagnosticsClusterServerInitCallback(EndpointId endpointId)
{
    if (gServers[endpointId].IsConstructed()) {
        return;
    }
    esp_matter::cluster_t *cluster = esp_matter::cluster::get(endpointId, EthernetNetworkDiagnostics::Id);

    gServers[endpointId].Create(DeviceLayer::GetDiagnosticDataProvider(),
                                BitFlags<EthernetNetworkDiagnostics::Feature>(get_feature_map(cluster)),
                                get_attribute_set(cluster));
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Register(gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer,
                     "Failed to register EthernetNetworkDiagnostics on endpoint %u - Error: %" CHIP_ERROR_FORMAT,
                     endpointId, err.Format());
    }
}

void ESPMatterEthernetNetworkDiagnosticsClusterServerShutdownCallback(EndpointId endpointId,
                                                                      ClusterShutdownType shutdownType)
{
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(
                         &gServers[endpointId].Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer,
                     "Failed to unregister EthernetNetworkDiagnostics on endpoint %u - Error: %" CHIP_ERROR_FORMAT,
                     endpointId, err.Format());
    }
    gServers[endpointId].Destroy();
}

void MatterEthernetNetworkDiagnosticsPluginServerInitCallback()
{
}

void MatterEthernetNetworkDiagnosticsPluginServerShutdownCallback()
{
}
