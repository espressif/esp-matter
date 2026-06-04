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

#include <app-common/zap-generated/attributes/Accessors.h>
#include <app/clusters/groupcast/GroupcastCluster.h>
#include <app/clusters/groupcast/GroupcastContext.h>
#include <access/AccessControl.h>
#include <app/server/Server.h>
#include <clusters/Groupcast/Enums.h>
#include <transport/raw/GroupcastTesting.h>
#include <credentials/GroupDataProvider.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/support/CodeUtils.h>
#include <platform/DefaultTimerDelegate.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::Groupcast::Attributes;

namespace {

LazyRegisteredServerCluster<GroupcastCluster> gServer;
DefaultTimerDelegate sTimerDelegate;

} // namespace

void ESPMatterGroupcastClusterServerInitCallback(chip::EndpointId endpointId)
{
    VerifyOrReturn(endpointId == kRootEndpointId);
    if (!gServer.IsConstructed()) {
        // Currently we don't support groupcast cluster in our data model; create with Listener (LN) feature only.
        // TODO: Create from enabled ZAP features once the cluster is in the ESP-Matter data model.
        Credentials::GroupDataProvider * groupDataProvider = Credentials::GetGroupDataProvider();
        VerifyOrDie(groupDataProvider != nullptr);

        BitFlags<chip::app::Clusters::Groupcast::Feature> features;
        features.Set(chip::app::Clusters::Groupcast::Feature::kListener);

        gServer.Create(
        GroupcastContext{
            .fabricTable       = Server::GetInstance().GetFabricTable(),
            .groupDataProvider = *groupDataProvider,
            .timerDelegate     = sTimerDelegate,
            .accessControl     = Access::GetAccessControl(),
            .testing           = chip::Groupcast::GetTesting(),
        },
        features);
    }
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(gServer.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register Groupcast - Error %" CHIP_ERROR_FORMAT, err.Format());
    }
}

void ESPMatterGroupcastClusterServerShutdownCallback(chip::EndpointId endpointId, ClusterShutdownType shutdownType)
{
    VerifyOrReturn(endpointId == chip::kRootEndpointId);
    VerifyOrReturn(gServer.IsConstructed());
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&gServer.Cluster(),
                                                                                            shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister Groupcast - Error %" CHIP_ERROR_FORMAT, err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        gServer.Destroy();
    }
}

void MatterGroupcastPluginServerInitCallback() {}
void MatterGroupcastPluginServerShutdownCallback() {}
