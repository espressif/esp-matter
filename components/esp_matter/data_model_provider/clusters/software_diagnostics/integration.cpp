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
#include <app/clusters/software-diagnostics-server/SoftwareDiagnosticsCluster.h>
#include <app/clusters/software-diagnostics-server/SoftwareDiagnosticsLogic.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>

using namespace esp_matter;
using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::SoftwareDiagnostics;

namespace {

LazyRegisteredServerCluster<SoftwareDiagnosticsServerCluster> gServer;

bool IsAttributeEnabled(EndpointId endpointId, AttributeId attributeId)
{
    return endpoint::is_attribute_enabled(endpointId, SoftwareDiagnostics::Id, attributeId);
}

} // namespace

void ESPMatterSoftwareDiagnosticsClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(endpointId == kRootEndpointId);
    SoftwareDiagnosticsLogic::OptionalAttributeSet attrSet;
    if (IsAttributeEnabled(kRootEndpointId, Attributes::ThreadMetrics::Id)) {
        attrSet.Set<Attributes::ThreadMetrics::Id>();
    }
    if (IsAttributeEnabled(kRootEndpointId, Attributes::CurrentHeapFree::Id)) {
        attrSet.Set<Attributes::CurrentHeapFree::Id>();
    }
    if (IsAttributeEnabled(kRootEndpointId, Attributes::CurrentHeapUsed::Id)) {
        attrSet.Set<Attributes::CurrentHeapUsed::Id>();
    }
    if (IsAttributeEnabled(kRootEndpointId, Attributes::CurrentHeapHighWatermark::Id)) {
        attrSet.Set<Attributes::CurrentHeapHighWatermark::Id>();
    }

    gServer.Create(attrSet);

    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(gServer.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register SoftwareDiagnostics - Error: %" CHIP_ERROR_FORMAT, err.Format());
    }
}

void ESPMatterSoftwareDiagnosticsClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    VerifyOrReturn(endpointId == kRootEndpointId);
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&gServer.Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister SoftwareDiagnostics - Error: %" CHIP_ERROR_FORMAT, err.Format());
    }
    gServer.Destroy();
}

void MatterSoftwareDiagnosticsPluginServerInitCallback() {}

void MatterSoftwareDiagnosticsPluginServerShutdownCallback() {}
