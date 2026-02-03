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
#include <app/clusters/diagnostic-logs-server/DiagnosticLogsCluster.h>
#include <app/server-cluster/ServerClusterInterfaceRegistry.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include "integration.h"

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;

LazyRegisteredServerCluster<DiagnosticLogsCluster> gServer;

void ESPMatterDiagnosticLogsClusterServerInitCallback(EndpointId endpoint)
{
    // We implement the cluster as a singleton on the root endpoint.
    VerifyOrReturn(endpoint == kRootEndpointId && !gServer.IsConstructed());
    gServer.Create();
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(gServer.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register DiagnosticLogs on endpoint %u - Error: %" CHIP_ERROR_FORMAT,
                     endpoint, err.Format());
    }
}

void ESPMatterDiagnosticLogsClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    // We implement the cluster as a singleton on the root endpoint.
    VerifyOrReturn(endpointId == kRootEndpointId && gServer.IsConstructed());
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Unregister(&gServer.Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister DiagnosticLogs on endpoint %u - Error: %" CHIP_ERROR_FORMAT,
                     endpointId, err.Format());
    }
    gServer.Destroy();
}

void MatterDiagnosticLogsPluginServerInitCallback() {}
void MatterDiagnosticLogsPluginServerShutdownCallback() {}

namespace chip::app::Clusters::DiagnosticLogs {

void SetDiagnosticLogsProviderDelegate(DiagnosticLogsProviderDelegate * delegate)
{
    gServer.Cluster().SetDelegate(delegate);
}

} // namespace chip::app::Clusters::DiagnosticLogs
