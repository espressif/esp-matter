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

#include "esp_matter_core.h"
#include <app/clusters/software-diagnostics-server/software-diagnostics-cluster.h>
#include <app/clusters/software-diagnostics-server/software-diagnostics-logic.h>
#include <app/util/attribute-storage.h>
#include <data-model-providers/codegen/CodegenDataModelProvider.h>
#include "clusters/SoftwareDiagnostics/AttributeIds.h"
#include "clusters/SoftwareDiagnostics/ClusterId.h"
#include <data_model/esp_matter_data_model.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::SoftwareDiagnostics;

namespace {
LazyRegisteredServerCluster<SoftwareDiagnosticsServerCluster<DeviceLayerSoftwareDiagnosticsLogic>> gServer;
}

bool isAttributeEnabled(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id)
{
    esp_matter::attribute_t *attr = esp_matter::attribute::get(endpoint_id, cluster_id, attribute_id);
    printf("attr %ld enabled %d", attribute_id, attr != nullptr);
    return attr != nullptr;
}

void emberAfSoftwareDiagnosticsClusterInitCallback(chip::EndpointId endpointId)
{
    VerifyOrReturn(endpointId == chip::kRootEndpointId);
    SoftwareDiagnosticsEnabledAttributes enabledAttributes{
        .enableThreadMetrics     = isAttributeEnabled(endpointId, Id, Attributes::ThreadMetrics::Id), 
        .enableCurrentHeapFree   = isAttributeEnabled(endpointId, Id, Attributes::CurrentHeapFree::Id),
        .enableCurrentHeapUsed   = isAttributeEnabled(endpointId, Id, Attributes::CurrentHeapUsed::Id), 
        .enableCurrentWatermarks = isAttributeEnabled(endpointId, Id, Attributes::CurrentHeapHighWatermark::Id), 
    };

    gServer.Create(enabledAttributes);

    CHIP_ERROR err = CodegenDataModelProvider::Instance().Registry().Register(gServer.Registration());
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "Failed to register SoftwareDiagnostics on endpoint %u: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }
}

void emberAfSoftwareDiagnosticsClusterShutdownCallback(EndpointId endpointId)
{
    VerifyOrReturn(endpointId == kRootEndpointId);
    CHIP_ERROR err = CodegenDataModelProvider::Instance().Registry().Unregister(&gServer.Cluster());
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "Failed to unregister SoftwareDiagnostics on endpoint %u: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }
    gServer.Destroy();
}

void MatterSoftwareDiagnosticsPluginServerInitCallback() {}

void MatterSoftwareDiagnosticsPluginServerShutdownCallback() {}
