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
#include "esp_matter_data_model_provider.h"

#include <app/clusters/soil-measurement-server/SoilMeasurementCluster.h>
#include <app/server-cluster/ServerClusterInterfaceRegistry.h>
#include <clusters/SoilMeasurement/Attributes.h>
#include <unordered_map>
#include "support/CodeUtils.h"

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::SoilMeasurement;

namespace {
std::unordered_map<EndpointId, LazyRegisteredServerCluster<SoilMeasurementCluster>> gServers;
std::unordered_map<EndpointId, Attributes::SoilMoistureMeasurementLimits::TypeInfo::Type> gLimits;
} // namespace

namespace chip::app::Clusters::SoilMeasurement {

CHIP_ERROR SetSoilMoistureMeasuredValue(
    EndpointId endpointId,
    const SoilMeasurement::Attributes::SoilMoistureMeasuredValue::TypeInfo::Type &soilMoistureMeasuredValue)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturnError(it != gServers.end(), CHIP_ERROR_NOT_FOUND);
    VerifyOrReturnError(it->second.IsConstructed(), CHIP_ERROR_INCORRECT_STATE);
    return it->second.Cluster().SetSoilMoistureMeasuredValue(soilMoistureMeasuredValue);
}

void SetSoilMoistureLimits(
    EndpointId endpointId,
    const SoilMeasurement::Attributes::SoilMoistureMeasurementLimits::TypeInfo::Type &soilMoistureLimits)
{
    auto [it, _] = gLimits.try_emplace(endpointId);
    it->second = soilMoistureLimits;
}

} // namespace chip::app::Clusters::SoilMeasurement

void ESPMatterSoilMeasurementClusterServerInitCallback(EndpointId endpointId)
{
    if (!gServers[endpointId].IsConstructed()) {
        VerifyOrDieWithMsg(gLimits.find(endpointId) != gLimits.end(), AppServer,
                           "Please set the limit for SoilMeasurementCluster on Endpoint 0x%" PRIx16, endpointId);
        gServers[endpointId].Create(endpointId, gLimits[endpointId]);
    }
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(
                         gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register SoilMeasurement - Error: %" CHIP_ERROR_FORMAT, err.Format());
    }
}

void ESPMatterSoilMeasurementClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    auto serverIt = gServers.find(endpointId);
    VerifyOrReturn(serverIt != gServers.end());
    VerifyOrReturn(serverIt->second.IsConstructed());
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&serverIt->second.Cluster(),
                                                                                            shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "SoilMeasurement unregister error: %" CHIP_ERROR_FORMAT, err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        serverIt->second.Destroy();
        gServers.erase(serverIt);
        auto limitIt = gLimits.find(endpointId);
        VerifyOrReturn(limitIt != gLimits.end());
        gLimits.erase(limitIt);
    }
}
