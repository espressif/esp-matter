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

#include <app/clusters/chime-server/ChimeCluster.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/support/CodeUtils.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;

namespace {
std::unordered_map<EndpointId, LazyRegisteredServerCluster<ChimeCluster>> gServers;
std::unordered_map<EndpointId, Clusters::ChimeDelegate *> gDelegates;
} // namespace

namespace chip::app::Clusters::Chime {

void SetDelegate(EndpointId endpointId, ChimeDelegate *delegate) {
    gDelegates[endpointId] = delegate;
}

} // namespace chip::app::Clusters::Chime

void ESPMatterChimeClusterServerInitCallback(EndpointId endpointId) {
    if (gServers[endpointId].IsConstructed()) {
        return;
    }

    gServers[endpointId].Create(endpointId, *gDelegates[endpointId]);
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Register(gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register Chime on endpoint %u - Error: %" CHIP_ERROR_FORMAT,
                     endpointId, err.Format());
    }
}

void ESPMatterChimeClusterServerShutdownCallback(EndpointId endpointId) {
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Unregister(&gServers[endpointId].Cluster());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister Chime on endpoint %u - Error: %" CHIP_ERROR_FORMAT,
                     endpointId, err.Format());
    }
    gServers[endpointId].Destroy();
}

// Legacy PluginServer callback stubs
void MatterChimePluginServerInitCallback() {}
void MatterChimePluginServerShutdownCallback() {}