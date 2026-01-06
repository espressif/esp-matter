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
#include <app/clusters/ota-provider/OTAProviderCluster.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <unordered_map>

#include "integration.h"

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;

namespace {
std::unordered_map<EndpointId, LazyRegisteredServerCluster<OtaProviderServer>> gServers;
} // namespace

void ESPMatterOtaSoftwareUpdateProviderClusterServerInitCallback(EndpointId endpointId)
{
    if (gServers[endpointId].IsConstructed()) {
        return;
    }

    gServers[endpointId].Create(endpointId);
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Register(gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register OTA on endpoint %u - Error: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }
}

void ESPMatterOtaSoftwareUpdateProviderClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Unregister(&gServers[endpointId].Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister OTA on endpoint %u - Error: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }
    gServers[endpointId].Destroy();
}

void MatterOtaSoftwareUpdateProviderPluginServerInitCallback() {}

void MatterOtaSoftwareUpdateProviderPluginServerShutdownCallback() {}

namespace chip::app::Clusters::OTAProvider {

void SetDelegate(EndpointId endpointId, OTAProviderDelegate *delegate)
{
    gServers[endpointId].Cluster().SetDelegate(delegate);
}

} // namespace chip::app::Clusters::OTAProvider
