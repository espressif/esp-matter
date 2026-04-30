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

#include <app/clusters/groups-server/GroupsCluster.h>
#include <app/server-cluster/ServerClusterInterfaceRegistry.h>
#include <credentials/GroupDataProvider.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <esp_matter_data_model.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <unordered_map>
#include <zap-generated/gen_config.h>

#ifdef MATTER_DM_PLUGIN_SCENES_MANAGEMENT
#include <data_model_provider/clusters/scenes_management/integration.h>
#endif
#ifdef ZCL_USING_IDENTIFY_CLUSTER_SERVER
#include <data_model_provider/clusters/identify/integration.h>
#endif

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::Groups;

namespace {

std::unordered_map<EndpointId, LazyRegisteredServerCluster<GroupsCluster>> gServers;

} // namespace

void ESPMatterGroupsClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(esp_matter::cluster::get(endpointId, Groups::Id) != nullptr,
                   ChipLogError(AppServer, "Groups: cluster missing in esp-matter data model for endpoint %u", endpointId));

    if (!gServers[endpointId].IsConstructed()) {
        Credentials::GroupDataProvider * groupDataProvider = Credentials::GetGroupDataProvider();
        VerifyOrDie(groupDataProvider != nullptr);

        gServers[endpointId].Create(endpointId,
        GroupsCluster::Context{
            .groupDataProvider = *groupDataProvider,
#ifdef MATTER_DM_PLUGIN_SCENES_MANAGEMENT
            .scenesIntegration =
            (esp_matter::cluster::get(endpointId, ScenesManagement::Id) != nullptr)
            ? ScenesManagement::FindClusterOnEndpoint(endpointId)
            : nullptr,
#else
            .scenesIntegration = nullptr,
#endif
#ifdef ZCL_USING_IDENTIFY_CLUSTER_SERVER
            .identifyIntegration =
            (esp_matter::cluster::get(endpointId, Identify::Id) != nullptr)
            ? Identify::FindClusterOnEndpoint(endpointId)
            : nullptr,
#else
            .identifyIntegration = nullptr,
#endif
        });
    }
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(
                         gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register Groups - Error %" CHIP_ERROR_FORMAT, err.Format());
    }
}

void ESPMatterGroupsClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end());
    VerifyOrReturn(it->second.IsConstructed());
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&it->second.Cluster(),
                                                                                            shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister Groups - Error %" CHIP_ERROR_FORMAT, err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.Destroy();
        gServers.erase(it);
    }
}

void MatterGroupsPluginServerInitCallback() {}
void MatterGroupsPluginServerShutdownCallback() {}
