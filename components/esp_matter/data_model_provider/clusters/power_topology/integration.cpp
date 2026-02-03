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

#include <app/ClusterCallbacks.h>
#include <app/clusters/power-topology-server/PowerTopologyCluster.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <unordered_map>

#include "integration.h"

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;

namespace {
std::unordered_map<EndpointId, LazyRegisteredServerCluster<PowerTopology::PowerTopologyCluster>> gServers;

} // namespace

namespace chip::app::Clusters::PowerTopology {

CHIP_ERROR Instance::Init()
{
    return esp_matter::data_model::provider::get_instance().registry().Register(mCluster.Registration());
}

void Instance::Shutdown()
{
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&mCluster.Cluster());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister Power Topology cluster: %" CHIP_ERROR_FORMAT, err.Format());
    }
}

} // namespace chip::app::Clusters::PowerTopology

void ESPMatterPowerTopologyClusterServerInitCallback(chip::EndpointId endpoint) {}
void ESPMatterPowerTopologyClusterServerShutdownCallback(chip::EndpointId endpoint, chip::app::ClusterShutdownType shutdownType) {}
