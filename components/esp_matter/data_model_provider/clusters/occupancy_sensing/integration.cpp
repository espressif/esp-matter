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
#include "esp_matter_attribute_utils.h"
#include "esp_matter_data_model_priv.h"
#include <esp_matter_data_model.h>

#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/cluster-objects.h>
#include <app/clusters/occupancy-sensor-server/OccupancySensingCluster.h>
#include <app/server-cluster/OptionalAttributeSet.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <platform/DefaultTimerDelegate.h>

#include <unordered_map>
#include "support/logging/TextOnlyLogging.h"

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::OccupancySensing;
using namespace chip::app::Clusters::OccupancySensing::Attributes;

namespace {

std::unordered_map<EndpointId, LazyRegisteredServerCluster<OccupancySensingCluster>> gServers;

DefaultTimerDelegate gDefaultTimerDelegate;

OccupancySensing::Feature getFeature(EndpointId endpointId)
{
    esp_matter::attribute_t *attr =
        esp_matter::attribute::get(endpointId, OccupancySensing::Id, Globals::Attributes::FeatureMap::Id);
    esp_matter_attr_val_t val = esp_matter_invalid(nullptr);
    if (attr && esp_matter::attribute::get_val_internal(attr, &val) == ESP_OK &&
        val.type == ESP_MATTER_VAL_TYPE_BITMAP32) {
        return static_cast<OccupancySensing::Feature>(val.val.u32);
    }
    return static_cast<OccupancySensing::Feature>(0);
}

} // namespace

void ESPMatterOccupancySensingClusterServerInitCallback(EndpointId endpointId)
{
    if (gServers[endpointId].IsConstructed()) {
        return;
    }
    OccupancySensingCluster::Config config(endpointId);

    config.WithFeatures(getFeature(endpointId));
    if (esp_matter::endpoint::is_attribute_enabled(endpointId, OccupancySensing::Id,
                                                   OccupancySensing::Attributes::HoldTime::Id)) {
        // Initializes hold time with default limits and timer delegate. The Application can use SetHoldTimeLimits() and
        // SetHoldTime() later to customize. Initial defaults come from typical values found in real devices on the
        // market.
        constexpr chip::app::Clusters::OccupancySensing::Structs::HoldTimeLimitsStruct::Type kDefaultHoldTimeLimits = {
            .holdTimeMin = 1, .holdTimeMax = 300, .holdTimeDefault = 30};
        config.WithHoldTime(kDefaultHoldTimeLimits.holdTimeDefault, kDefaultHoldTimeLimits, gDefaultTimerDelegate);

        // Show deprecated attributes if enabled in Zap
        config.WithDeprecatedAttributes(
            esp_matter::endpoint::is_attribute_enabled(endpointId, OccupancySensing::Id,
                                                       Attributes::PIROccupiedToUnoccupiedDelay::Id) ||
            esp_matter::endpoint::is_attribute_enabled(endpointId, OccupancySensing::Id,
                                                       Attributes::UltrasonicOccupiedToUnoccupiedDelay::Id) ||
            esp_matter::endpoint::is_attribute_enabled(endpointId, OccupancySensing::Id,
                                                       Attributes::PhysicalContactOccupiedToUnoccupiedDelay::Id));
    }
    gServers[endpointId].Create(config);
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Register(gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register OccupancySensing - Error %" CHIP_ERROR_FORMAT, err.Format());
    }
}

void ESPMatterOccupancySensingClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    if (!gServers[endpointId].IsConstructed()) {
        return;
    }
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(
        &gServers[endpointId].Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister OccupancySensing - Error %" CHIP_ERROR_FORMAT, err.Format());
    }
    gServers[endpointId].Destroy();
}

namespace chip::app::Clusters::OccupancySensing {

OccupancySensingCluster *FindClusterOnEndpoint(EndpointId endpointId)
{
    if (gServers[endpointId].IsConstructed()) {
        return &gServers[endpointId].Cluster();
    }
    return nullptr;
}

} // namespace chip::app::Clusters::OccupancySensing

void MatterOccupancySensingPluginServerInitCallback()
{
}
void MatterOccupancySensingPluginServerShutdownCallback()
{
}
