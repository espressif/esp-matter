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

#include <esp_matter_data_model_priv.h>

#include <clusters/zone_management/integration.h>

#include <app/ClusterCallbacks.h>
#include <app/clusters/zone-management-server/Delegate.h>
#include <app/clusters/zone-management-server/ZoneManagementCluster.h>
#include <clusters/ZoneManagement/ClusterId.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <data_model/esp_matter_attribute_helpers.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ZoneManagement;
using namespace esp_matter;

namespace {

struct ZoneManagementConfig {
    uint8_t maxUserDefinedZones = 10;
    uint8_t maxZones            = 20;
    uint8_t sensitivityMax      = 10;
    TwoDCartesianVertexStruct twoDCartesianMax{};
};

std::unordered_map<EndpointId, LazyRegisteredServerCluster<ZoneManagementCluster>> gServers;
std::unordered_map<EndpointId, Delegate *> gDelegates;
std::unordered_map<EndpointId, ZoneManagementConfig> gConfigs;
std::unordered_map<EndpointId, BitFlags<Feature>> gFeatures;

} // namespace

namespace chip {
namespace app {
namespace Clusters {
namespace ZoneManagement {

void MatterZoneManagementSetDelegate(EndpointId endpointId, Delegate *delegate)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it == gServers.end() || !it->second.IsConstructed(),
                   ChipLogError(AppServer, "ZoneManagement: cluster already initialized; cannot set delegate"));
    gDelegates[endpointId] = delegate;
}

void MatterZoneManagementSetConfig(EndpointId endpointId, uint8_t maxUserDefinedZones, uint8_t maxZones,
                                   uint8_t sensitivityMax, const TwoDCartesianVertexStruct &twoDCartesianMax)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it == gServers.end() || !it->second.IsConstructed(),
                   ChipLogError(AppServer, "ZoneManagement: cluster already initialized; cannot set config"));
    gConfigs[endpointId] = ZoneManagementConfig{
        .maxUserDefinedZones = maxUserDefinedZones,
        .maxZones            = maxZones,
        .sensitivityMax      = sensitivityMax,
        .twoDCartesianMax    = twoDCartesianMax,
    };
}

void MatterZoneManagementSetFeatures(EndpointId endpointId, BitFlags<Feature> features)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it == gServers.end() || !it->second.IsConstructed(),
                   ChipLogError(AppServer, "ZoneManagement: cluster already initialized; cannot set features"));
    gFeatures[endpointId] = features;
}

} // namespace ZoneManagement
} // namespace Clusters
} // namespace app
} // namespace chip

void ESPMatterZoneManagementClusterServerInitCallback(EndpointId endpointId)
{
    cluster_t *cluster = cluster::get(endpointId, ZoneManagement::Id);
    VerifyOrReturn(cluster != nullptr,
                   ChipLogError(AppServer,
                                "ZoneManagement: cluster missing in esp-matter data model for endpoint %u", endpointId));

    if (!gServers[endpointId].IsConstructed()) {
        Delegate *delegate = nullptr;
        {
            auto it = gDelegates.find(endpointId);
            VerifyOrReturn(it != gDelegates.end() && it->second != nullptr,
                           ChipLogError(AppServer,
                                        "ZoneManagement: delegate not set for ep %u "
                                        "(call MatterZoneManagementSetDelegate first)",
                                        endpointId));
            delegate = it->second;
        }

        BitFlags<Feature> features;
        auto featIt = gFeatures.find(endpointId);
        if (featIt != gFeatures.end()) {
            features = featIt->second;
        } else {
            features = BitFlags<Feature>(read_feature_map_u32(endpointId, ZoneManagement::Id));
        }

        ZoneManagementConfig config;
        auto cfgIt = gConfigs.find(endpointId);
        if (cfgIt != gConfigs.end()) {
            config = cfgIt->second;
        }

        gServers[endpointId].Create(ZoneManagementCluster::Context{
            .delegate   = *delegate,
            .endpointId = endpointId,
            .features   = features,
            .config     = {
                .maxUserDefinedZones = config.maxUserDefinedZones,
                .maxZones            = config.maxZones,
                .sensitivityMax      = config.sensitivityMax,
                .twoDCartesianMax    = config.twoDCartesianMax,
            },
        });
    }

    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Register(gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "ZoneManagement register failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
}

void ESPMatterZoneManagementClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    auto it = gServers.find(endpointId);
    if (it == gServers.end() || !it->second.IsConstructed()) {
        return;
    }
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Unregister(&it->second.Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "ZoneManagement unregister failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.Destroy();
        gServers.erase(it);
    }
}

void MatterZoneManagementPluginServerInitCallback() {}
void MatterZoneManagementPluginServerShutdownCallback() {}
