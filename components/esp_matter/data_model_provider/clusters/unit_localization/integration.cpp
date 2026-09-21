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

#include "integration.h"
#include "esp_matter_data_model.h"
#include "esp_matter_data_model_priv.h"
#include <app/ClusterCallbacks.h>
#include <app/server-cluster/ServerClusterInterfaceRegistry.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <data_model/esp_matter_attribute_helpers.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::UnitLocalization;
using namespace esp_matter;

namespace {

LazyRegisteredServerCluster<UnitLocalizationServer> gServer;

} // namespace

UnitLocalizationServer &UnitLocalizationServer::Instance()
{
    VerifyOrDie(gServer.IsConstructed());
    return gServer.Cluster();
}

void ESPMatterUnitLocalizationClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(endpointId == kRootEndpointId);
    if (!gServer.IsConstructed()) {
        esp_matter::cluster_t *cluster = esp_matter::cluster::get(endpointId, UnitLocalization::Id);
        VerifyOrReturn(cluster != nullptr,
                       ChipLogError(AppServer,
                                    "UnitLocalization: cluster missing in esp-matter data model for endpoint %u", endpointId));

        gServer.Create(endpointId, BitFlags<UnitLocalization::Feature>(read_feature_map_u32(cluster)));
    }
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(gServer.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register UnitLocalization - Error: %" CHIP_ERROR_FORMAT, err.Format());
    }
}

void ESPMatterUnitLocalizationClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    VerifyOrReturn(endpointId == kRootEndpointId);
    VerifyOrReturn(gServer.IsConstructed());
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&gServer.Cluster(),
                                                                                            shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "UnitLocalization unregister error: %" CHIP_ERROR_FORMAT, err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        gServer.Destroy();
    }
}

void MatterUnitLocalizationPluginServerInitCallback() {}

void MatterUnitLocalizationPluginServerShutdownCallback() {}
