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

#include <data_model_provider/clusters/webrtc_transport_provider_integration.h>

#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::WebRTCTransportProvider;
using namespace esp_matter;

namespace {
std::unordered_map<EndpointId, LazyRegisteredServerCluster<WebRTCTransportProviderCluster>> gServers;
std::unordered_map<EndpointId, Clusters::WebRTCTransportProvider::Delegate *> gDelegates;

bool IsClusterEnabled(EndpointId endpointId)
{
    cluster_t *cluster = cluster::get(endpointId, WebRTCTransportProvider::Id);
    return cluster != nullptr;
}
} // namespace

namespace chip::app::Clusters::WebRTCTransportProvider {

void SetDelegate(EndpointId endpointId, Delegate * delegate)
{
    gDelegates[endpointId] = delegate;
}

} // namespace chip::app::Clusters::WebRTCTransportProvider

void ESPMatterWebRTCTransportProviderClusterServerInitCallback(EndpointId endpointId)
{
    if (!IsClusterEnabled(endpointId) || gServers[endpointId].IsConstructed()) {
        return;
    }

    if (gDelegates[endpointId] == nullptr) {
        ChipLogError(AppServer, "WebRTC Transport Provider delegate missing for endpoint %u", endpointId);
        return;
    }

    ChipLogProgress(AppServer, "Registering WebRTC Transport Provider on endpoint %u", endpointId);
    gServers[endpointId].Create(endpointId, *gDelegates[endpointId]);
    CHIP_ERROR err = data_model::provider::get_instance().registry().Register(gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register WebRTC Transport Provider on endpoint %u - Error: %" CHIP_ERROR_FORMAT,
                     endpointId, err.Format());
    }
}

void ESPMatterWebRTCTransportProviderClusterServerShutdownCallback(EndpointId endpointId)
{
    if (!IsClusterEnabled(endpointId) || !gServers[endpointId].IsConstructed()) {
        return;
    }

    CHIP_ERROR err = data_model::provider::get_instance().registry().Unregister(&gServers[endpointId].Cluster());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister WebRTC Transport Provider on endpoint %u - Error: %" CHIP_ERROR_FORMAT,
                     endpointId, err.Format());
    }
    gServers[endpointId].Destroy();
}

void MatterWebRTCTransportProviderPluginServerInitCallback() {}

void MatterWebRTCTransportProviderPluginServerShutdownCallback() {}
