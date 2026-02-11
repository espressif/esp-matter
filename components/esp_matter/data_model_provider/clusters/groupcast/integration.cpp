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
#include <clusters/Groupcast/Enums.h>
#include <data_model_provider/esp_matter_data_model_provider.h>

using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::Groupcast::Attributes;
using chip::Protocols::InteractionModel::Status;

namespace {

LazyRegisteredServerCluster<GroupcastCluster> gServer;

} // namespace

void ESPMatterGroupcastClusterServerInitCallback(chip::EndpointId endpointId)
{
    VerifyOrDie(endpointId == chip::kRootEndpointId);

    // Currently we don't support groupcast cluster in our data model, create the cluster with LN feature enabled.
    // TODO: We should create the cluster according to the enabled features after we add the cluster.
    chip::BitFlags<Groupcast::Feature> feature;
    feature.Set(Groupcast::Feature::kListener);
    gServer.Create(feature);
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(gServer.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register Groupcast - Error %" CHIP_ERROR_FORMAT, err.Format());
    }
}

void ESPMatterGroupcastClusterServerShutdownCallback(chip::EndpointId endpointId, ClusterShutdownType shutdownType)
{
    VerifyOrDie(endpointId == chip::kRootEndpointId);

    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&gServer.Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister Groupcast - Error %" CHIP_ERROR_FORMAT, err.Format());
    }
    gServer.Destroy();
}

void MatterGroupcastPluginServerInitCallback() {}
void MatterGroupcastPluginServerShutdownCallback() {}
