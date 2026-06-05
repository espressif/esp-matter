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

#include <esp_matter_data_model_priv.h>

#include <app/ClusterCallbacks.h>
#include <app/ConcreteAttributePath.h>
#include <app/clusters/valve-configuration-and-control-server/ValveConfigurationAndControlCluster.h>
#include <clusters/ValveConfigurationAndControl/Attributes.h>
#include <clusters/ValveConfigurationAndControl/ClusterId.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <data_model/esp_matter_attribute_helpers.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ValveConfigurationAndControl;
using namespace esp_matter;

namespace {

std::unordered_map<EndpointId, LazyRegisteredServerCluster<ValveConfigurationAndControlCluster>> gServers;

DataModel::Nullable<uint32_t> ReadNullableU32(EndpointId endpointId, AttributeId attributeId)
{
    attribute_t *attr = attribute::get(endpointId, ValveConfigurationAndControl::Id, attributeId);
    if (attr == nullptr) {
        return DataModel::NullNullable;
    }
    esp_matter_attr_val_t val;
    if (attribute::get_val_internal(attr, &val) != ESP_OK) {
        return DataModel::NullNullable;
    }
    using Traits = chip::app::NumericAttributeTraits<uint32_t>;
    if (val.type == ESP_MATTER_VAL_TYPE_NULLABLE_UINT32 && Traits::IsNullValue(val.val.u32)) {
        return DataModel::NullNullable;
    }
    if (val.type == ESP_MATTER_VAL_TYPE_UINT32 || val.type == ESP_MATTER_VAL_TYPE_NULLABLE_UINT32) {
        return DataModel::Nullable<uint32_t>(val.val.u32);
    }
    return DataModel::NullNullable;
}

uint8_t ReadU8OrDefault(EndpointId endpointId, AttributeId attributeId, uint8_t defaultValue)
{
    uint8_t v = defaultValue;
    if (read_attribute_raw_value(endpointId, ValveConfigurationAndControl::Id, attributeId, v)) {
        return v;
    }
    return defaultValue;
}

ValveConfigurationAndControlCluster::OptionalAttributeSet BuildOptionalSet(EndpointId endpointId)
{
    using namespace chip::app::Clusters::ValveConfigurationAndControl::Attributes;
    ValveConfigurationAndControlCluster::OptionalAttributeSet optionalSet(0);
    if (attribute::get(endpointId, ValveConfigurationAndControl::Id, DefaultOpenLevel::Id)) {
        optionalSet.Set<DefaultOpenLevel::Id>();
    }
    if (attribute::get(endpointId, ValveConfigurationAndControl::Id, ValveFault::Id)) {
        optionalSet.Set<ValveFault::Id>();
    }
    if (attribute::get(endpointId, ValveConfigurationAndControl::Id, LevelStep::Id)) {
        optionalSet.Set<LevelStep::Id>();
    }
    return optionalSet;
}

} // namespace

void ESPMatterValveConfigurationAndControlClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(cluster::get(endpointId, ValveConfigurationAndControl::Id) != nullptr,
                   ChipLogError(AppServer,
                                "ValveConfigurationAndControl: cluster missing in esp-matter data model for endpoint %u",
                                endpointId));
    if (!gServers[endpointId].IsConstructed()) {
        const uint32_t featureMap = read_feature_map_u32(endpointId, ValveConfigurationAndControl::Id);
        const auto optionalSet    = BuildOptionalSet(endpointId);

        DataModel::Nullable<uint32_t> defaultOpenDuration = ReadNullableU32(endpointId, Attributes::DefaultOpenDuration::Id);
        Percent defaultOpenLevel = ReadU8OrDefault(endpointId, Attributes::DefaultOpenLevel::Id,
                                                   ValveConfigurationAndControlCluster::kDefaultOpenLevel);
        uint8_t levelStep = ReadU8OrDefault(endpointId, Attributes::LevelStep::Id,
                                            ValveConfigurationAndControlCluster::kDefaultLevelStep);

        ValveConfigurationAndControlCluster::StartupConfiguration startupConfig{ .defaultOpenDuration = defaultOpenDuration,
                                                                                 .defaultOpenLevel    = defaultOpenLevel,
                                                                                 .levelStep           = levelStep };

        ValveConfigurationAndControlCluster::ValveContext context = {
            .features             = BitFlags<ValveConfigurationAndControl::Feature>(featureMap),
            .optionalAttributeSet = optionalSet,
            .config               = startupConfig,
            .tsTracker            = nullptr,
            .delegate             = nullptr,
        };

        gServers[endpointId].Create(endpointId, context);
    }
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(
                         gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "ValveConfigurationAndControl register failed ep %u: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }
}

void ESPMatterValveConfigurationAndControlClusterServerShutdownCallback(EndpointId endpointId,
                                                                        ClusterShutdownType shutdownType)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end());
    VerifyOrReturn(it->second.IsConstructed());
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&it->second.Cluster(),
                                                                                            shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "ValveConfigurationAndControl unregister failed ep %u: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.Destroy();
        gServers.erase(it);
    }
}

void MatterValveConfigurationAndControlPluginServerInitCallback() {}

namespace chip {
namespace app {
namespace Clusters {
namespace ValveConfigurationAndControl {

void SetDefaultDelegate(EndpointId endpointId, Delegate * delegate)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end() && it->second.IsConstructed());
    it->second.Cluster().SetDelegate(delegate);
}

} // namespace ValveConfigurationAndControl
} // namespace Clusters
} // namespace app
} // namespace chip
