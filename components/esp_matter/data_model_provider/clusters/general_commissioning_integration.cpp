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
#include <app/clusters/general-commissioning-server/general-commissioning-cluster.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <esp_matter_data_model.h>
#include "app/server-cluster/ServerClusterInterfaceRegistry.h"
#include "core/DataModelTypes.h"
#include "support/CodeUtils.h"

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;

namespace {
LazyRegisteredServerCluster<GeneralCommissioningCluster> gServer;
}

namespace chip::app::Clusters::GeneralCommissioning {

GeneralCommissioningCluster * Instance()
{
    if (!gServer.IsConstructed()) {
        gServer.Create();
    }
    return &gServer.Cluster();
}

} // namespace chip::app::Clusters::GeneralCommissioning

void ESPMatterGeneralCommissioningClusterServerInitCallback(EndpointId endpointId)
{
    if (endpointId != kRootEndpointId) {
        return;
    }
    if (!gServer.IsConstructed()) {
        gServer.Create();
    }
    if (esp_matter::endpoint::is_attribute_enabled(endpointId, GeneralCommissioning::Id, GeneralCommissioning::Attributes::IsCommissioningWithoutPower::Id)) {
        gServer.Cluster().GetOptionalAttributes().Set<GeneralCommissioning::Attributes::IsCommissioningWithoutPower::Id>();
    }        

    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(gServer.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register GeneralCommissioning - Error: %" CHIP_ERROR_FORMAT, err.Format());
    }
}

void ESPMatterGeneralCommissioningClusterServerShutdownCallback(EndpointId endpointId)
{
    VerifyOrReturn(endpointId == kRootEndpointId && gServer.IsConstructed());
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Unregister(&gServer.Cluster());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister GeneralCommissioning - Error: %" CHIP_ERROR_FORMAT, err.Format());
    }
    gServer.Destroy();
}

void MatterGeneralCommissioningPluginServerInitCallback() {}
void MatterGeneralCommissioningPluginServerShutdownCallback() {}
