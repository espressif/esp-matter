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
#include <app/clusters/group-key-mgmt-server/group-key-mgmt-cluster.h>
#include <data_model_provider/esp_matter_data_model_provider.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;

namespace {
LazyRegisteredServerCluster<GroupKeyManagementCluster> gServer;
}

void ESPMatterGroupKeyManagementClusterServerInitCallback(EndpointId endpoint)
{
    // We implement the cluster as a singleton on the root endpoint.
    VerifyOrReturn(endpoint == kRootEndpointId);

    gServer.Create();
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(gServer.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register GroupKeyManagement - Error: %" CHIP_ERROR_FORMAT, err.Format());
    }
}

void ESPMatterGroupKeyManagementClusterServerShutdownCallback(EndpointId endpointId)
{
    // We implement the cluster as a singleton on the root endpoint.
    VerifyOrReturn(endpointId == kRootEndpointId);
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&gServer.Cluster());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister GroupKeyManagement - Error: %" CHIP_ERROR_FORMAT, err.Format());
    }
    gServer.Destroy();
}

void MatterGroupKeyManagementPluginServerInitCallback() {}
void MatterGroupKeyManagementPluginServerShutdownCallback() {}
