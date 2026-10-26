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

#include <clusters/closure_dimension/integration.h>

#include <app/ClusterCallbacks.h>
#include <app/clusters/closure-dimension-server/ClosureDimensionCluster.h>
#include <clusters/ClosureDimension/ClusterId.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <data_model/esp_matter_attribute_helpers.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ClosureDimension;
using namespace esp_matter;

namespace {

std::unordered_map<EndpointId, LazyRegisteredServerCluster<ClosureDimensionCluster>> gServers;
std::unordered_map<EndpointId, ClosureDimensionClusterDelegate *> gDelegates;

} // namespace

namespace chip {
namespace app {
namespace Clusters {
namespace ClosureDimension {

void MatterClosureDimensionSetDelegate(EndpointId endpointId, ClosureDimensionClusterDelegate &delegate)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it == gServers.end() || !it->second.IsConstructed(),
                   ChipLogError(AppServer, "ClosureDimension: cluster already initialized; cannot set delegate"));
    gDelegates[endpointId] = &delegate;
}

ClosureDimensionCluster * GetClusterInstance(EndpointId endpointId)
{
    auto it = gServers.find(endpointId);
    if (it == gServers.end() || !it->second.IsConstructed()) {
        return nullptr;
    }
    return &it->second.Cluster();
}

} // namespace ClosureDimension
} // namespace Clusters
} // namespace app
} // namespace chip

void ESPMatterClosureDimensionClusterServerInitCallback(EndpointId endpointId)
{
    cluster_t *cluster = cluster::get(endpointId, ClosureDimension::Id);
    VerifyOrReturn(cluster != nullptr,
                   ChipLogError(AppServer,
                                "ClosureDimension: cluster missing in esp-matter data model for endpoint %u", endpointId));

    if (!gServers[endpointId].IsConstructed()) {
        // Prefer the delegate from MatterClosureDimensionSetDelegate(), else the one from the cluster config.
        // The config delegate is registered by its delegate init callback, which runs after this callback.
        ClosureDimensionClusterDelegate *delegate = static_cast<ClosureDimensionClusterDelegate *>(cluster::get_delegate_impl(cluster));
        auto it = gDelegates.find(endpointId);
        if (it != gDelegates.end() && it->second != nullptr) {
            delegate = it->second;
        }
        VerifyOrReturn(delegate != nullptr,
                       ChipLogError(AppServer,
                                    "ClosureDimension: delegate not set for ep %u (set it in the cluster config or call MatterClosureDimensionSetDelegate first)",
                                    endpointId));

        // Read feature map from esp-matter data model
        BitFlags<ClosureDimension::Feature> features(read_feature_map_u32(endpointId, ClosureDimension::Id));

        // Build Config from the feature map and the feature attributes stored in the esp-matter data model
        ClosureDimensionCluster::Config config(endpointId, *delegate);

        if (features.Has(Feature::kPositioning)) {
            esp_matter_attr_val_t resolution_val = esp_matter_uint16(1);
            esp_matter_attr_val_t step_val = esp_matter_uint16(1);
            get_stored_attr_val(cluster, Attributes::Resolution::Id, resolution_val);
            get_stored_attr_val(cluster, Attributes::StepValue::Id, step_val);
            config.WithPositioning(resolution_val.val.u16, step_val.val.u16);
        }
        // Values not present in the esp-matter data model keep the Config defaults.
        if (features.Has(Feature::kMotionLatching)) {
            esp_matter_attr_val_t latch_val = esp_matter_bitmap8(0);
            get_stored_attr_val(cluster, Attributes::LatchControlModes::Id, latch_val);
            config.WithMotionLatching(BitFlags<LatchControlModesBitmap>(latch_val.val.u8));
        }

        if (features.Has(Feature::kUnit)) {
            esp_matter_attr_val_t unit_val = esp_matter_enum8(to_underlying(ClosureUnitEnum::kUnknownEnumValue));
            get_stored_attr_val(cluster, Attributes::Unit::Id, unit_val);
            config.WithUnit(static_cast<ClosureUnitEnum>(unit_val.val.u8), DataModel::NullNullable);
        }
        if (features.Has(Feature::kLimitation)) {
            config.WithLimitation({});
        }
        if (features.Has(Feature::kSpeed)) {
            config.WithSpeed();
        }
        if (features.Has(Feature::kTranslation)) {
            esp_matter_attr_val_t direction_val =
                esp_matter_enum8(to_underlying(TranslationDirectionEnum::kUnknownEnumValue));
            get_stored_attr_val(cluster, Attributes::TranslationDirection::Id, direction_val);
            config.WithTranslation(static_cast<TranslationDirectionEnum>(direction_val.val.u8));
        }
        if (features.Has(Feature::kRotation)) {
            esp_matter_attr_val_t axis_val = esp_matter_enum8(to_underlying(RotationAxisEnum::kUnknownEnumValue));
            esp_matter_attr_val_t overflow_val = esp_matter_enum8(to_underlying(OverflowEnum::kUnknownEnumValue));
            get_stored_attr_val(cluster, Attributes::RotationAxis::Id, axis_val);
            get_stored_attr_val(cluster, Attributes::Overflow::Id, overflow_val);
            config.WithRotation(static_cast<RotationAxisEnum>(axis_val.val.u8),
                                static_cast<OverflowEnum>(overflow_val.val.u8));
        }
        if (features.Has(Feature::kModulation)) {
            esp_matter_attr_val_t modulation_val =
                esp_matter_enum8(to_underlying(ModulationTypeEnum::kUnknownEnumValue));
            get_stored_attr_val(cluster, Attributes::ModulationType::Id, modulation_val);
            config.WithModulation(static_cast<ModulationTypeEnum>(modulation_val.val.u8));
        }
        if (features.Has(Feature::kAccess)) {
            config.WithAccess();
        }

        gServers[endpointId].Create(config);
    }

    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Register(gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "ClosureDimension register failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
}

void ESPMatterClosureDimensionClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    auto it = gServers.find(endpointId);
    if (it == gServers.end() || !it->second.IsConstructed()) {
        return;
    }
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Unregister(&it->second.Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "ClosureDimension unregister failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.Destroy();
        gServers.erase(it);
    }
}

__attribute__((weak)) void MatterClosureDimensionPluginServerInitCallback() {}
__attribute__((weak)) void MatterClosureDimensionPluginServerShutdownCallback() {}
