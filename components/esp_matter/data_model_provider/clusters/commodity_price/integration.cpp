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

#include <app/ClusterCallbacks.h>
#include <app/clusters/commodity-price-server/CommodityPriceCluster.h>
#include <clusters/CommodityPrice/ClusterId.h>
#include <clusters/CommodityPrice/CommandIds.h>
#include <data_model/esp_matter_attribute_helpers.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace esp_matter;

namespace {

std::unordered_map<EndpointId, LazyRegisteredServerCluster<CommodityPrice::CommodityPriceCluster>> gServers;

} // namespace

void ESPMatterCommodityPriceClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(cluster::get(endpointId, CommodityPrice::Id) != nullptr,
                   ChipLogError(AppServer, "CommodityPrice: cluster missing in esp-matter data model for endpoint %u",
                                endpointId));

    auto &entry = gServers[endpointId];
    if (!entry.IsConstructed()) {
        CommodityPrice::CommodityPriceCluster::OptionalCommandSet optionalCommands(0);
        if (endpoint::is_command_enabled(endpointId, CommodityPrice::Id,
                                         CommodityPrice::Commands::GetDetailedPriceRequest::Id)) {
            optionalCommands.Set<CommodityPrice::Commands::GetDetailedPriceRequest::Id>();
        }
        if (endpoint::is_command_enabled(endpointId, CommodityPrice::Id,
                                         CommodityPrice::Commands::GetDetailedForecastRequest::Id)) {
            optionalCommands.Set<CommodityPrice::Commands::GetDetailedForecastRequest::Id>();
        }

        CommodityPrice::CommodityPriceCluster::Config config{
            .features = BitMask<CommodityPrice::Feature>(read_feature_map_u32(endpointId, CommodityPrice::Id)),
            .optionalCommands = optionalCommands,
        };
        entry.Create(endpointId, config);
    }

    CHIP_ERROR err = data_model::provider::get_instance().registry().Register(entry.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register CommodityPrice on endpoint %u: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }
}

void ESPMatterCommodityPriceClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end());
    VerifyOrReturn(it->second.IsConstructed());

    CHIP_ERROR err = data_model::provider::get_instance().registry().Unregister(&it->second.Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister CommodityPrice on endpoint %u: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }

    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.Destroy();
        gServers.erase(it);
    }
}

__attribute__((weak)) void MatterCommodityPricePluginServerInitCallback() {}
__attribute__((weak)) void MatterCommodityPricePluginServerShutdownCallback() {}

namespace chip::app::Clusters::CommodityPrice {

CommodityPriceCluster * FindClusterOnEndpoint(EndpointId endpoint)
{
    auto it = gServers.find(endpoint);
    if (it == gServers.end() || !it->second.IsConstructed()) {
        return nullptr;
    }
    return &it->second.Cluster();
}

} // namespace chip::app::Clusters::CommodityPrice
