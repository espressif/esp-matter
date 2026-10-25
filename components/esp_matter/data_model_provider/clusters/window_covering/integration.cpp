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

#include <app-common/zap-generated/attributes/Accessors.h>
#include <app/ClusterCallbacks.h>
#include <clusters/window_covering/integration.h>
#include <app/clusters/window-covering-server/WindowCoveringCluster.h>
#include <app/clusters/window-covering-server/WindowCoveringDelegate.h>
#include <clusters/WindowCovering/ClusterId.h>
#include <data_model/esp_matter_attribute_helpers.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::Protocols::InteractionModel;
using namespace esp_matter;

namespace {

// Forwards to the application delegate once it is set via SetDefaultDelegate(); the
// cluster requires a delegate reference at construction, before the app can provide one.
class WindowCoveringDelegateWrapper final : public WindowCovering::WindowCoveringDelegate {
public:
    void SetEndpointId(EndpointId endpointId)
    {
        mEndpointId = endpointId;
    }
    void SetWrapped(WindowCovering::WindowCoveringDelegate *wrapped)
    {
        mWrapped = wrapped;
    }

    CHIP_ERROR HandleMovement(WindowCovering::WindowCoveringType type) override
    {
        VerifyOrReturnValue(mWrapped != nullptr, CHIP_NO_ERROR,
                            ChipLogProgress(Zcl, "WindowCovering has no delegate set for endpoint %u", mEndpointId));
        return mWrapped->HandleMovement(type);
    }

    CHIP_ERROR HandleStopMotion() override
    {
        VerifyOrReturnValue(mWrapped != nullptr, CHIP_NO_ERROR,
                            ChipLogProgress(Zcl, "WindowCovering has no delegate set for endpoint %u", mEndpointId));
        return mWrapped->HandleStopMotion();
    }

private:
    WindowCovering::WindowCoveringDelegate *mWrapped = nullptr;
    EndpointId mEndpointId = kInvalidEndpointId;
};

// Exposes the protected fixed-attribute setters kept for backward compatibility.
class ESPWindowCoveringCluster : public WindowCovering::WindowCoveringCluster {
public:
    using WindowCoveringCluster::SetEndProductType;
    using WindowCoveringCluster::SetType;
    using WindowCoveringCluster::WindowCoveringCluster;
};

struct WindowCoveringEntry {
    WindowCoveringDelegateWrapper delegate;
    LazyRegisteredServerCluster<ESPWindowCoveringCluster> server;
};

std::unordered_map<EndpointId, WindowCoveringEntry> gServers;

} // namespace

void ESPMatterWindowCoveringClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(cluster::get(endpointId, WindowCovering::Id) != nullptr,
                   ChipLogError(AppServer, "WindowCovering: cluster missing in esp-matter data model for endpoint %u",
                                endpointId));

    auto &entry = gServers[endpointId];
    if (!entry.server.IsConstructed()) {
        entry.delegate.SetEndpointId(endpointId);

        BitFlags<WindowCovering::Feature> features(read_feature_map_u32(endpointId, WindowCovering::Id));

        namespace Attrs = WindowCovering::Attributes;

        WindowCovering::OptionalAttributeSet optionalAttributes;
        if (features.Has(WindowCovering::Feature::kLift) &&
                endpoint::is_attribute_enabled(endpointId, WindowCovering::Id, Attrs::NumberOfActuationsLift::Id)) {
            optionalAttributes.Set<Attrs::NumberOfActuationsLift::Id>();
        }
        if (features.Has(WindowCovering::Feature::kTilt) &&
                endpoint::is_attribute_enabled(endpointId, WindowCovering::Id, Attrs::NumberOfActuationsTilt::Id)) {
            optionalAttributes.Set<Attrs::NumberOfActuationsTilt::Id>();
        }
        if (features.HasAll(WindowCovering::Feature::kLift, WindowCovering::Feature::kPositionAwareLift) &&
                endpoint::is_attribute_enabled(endpointId, WindowCovering::Id, Attrs::CurrentPositionLiftPercentage::Id)) {
            optionalAttributes.Set<Attrs::CurrentPositionLiftPercentage::Id>();
        }
        if (features.HasAll(WindowCovering::Feature::kTilt, WindowCovering::Feature::kPositionAwareTilt) &&
                endpoint::is_attribute_enabled(endpointId, WindowCovering::Id, Attrs::CurrentPositionTiltPercentage::Id)) {
            optionalAttributes.Set<Attrs::CurrentPositionTiltPercentage::Id>();
        }
        if (endpoint::is_attribute_enabled(endpointId, WindowCovering::Id, Attrs::SafetyStatus::Id)) {
            optionalAttributes.Set<Attrs::SafetyStatus::Id>();
        }

        WindowCovering::WindowCoveringCluster::Config config(entry.delegate);
        config.WithFeatures(features).WithOptionalAttributes(optionalAttributes);

        WindowCovering::Type type{};
        if (WindowCovering::Attributes::Type::GetDefault(endpointId, type) == Status::Success) {
            config.WithType(type);
        }

        WindowCovering::EndProductType endProductType{};
        if (WindowCovering::Attributes::EndProductType::GetDefault(endpointId, endProductType) == Status::Success) {
            config.WithEndProductType(endProductType);
        }

        entry.server.Create(endpointId, config);

        auto &clusterServer = entry.server.Cluster();

        BitMask<WindowCovering::ConfigStatus> configStatus;
        if (WindowCovering::Attributes::ConfigStatus::GetDefault(endpointId, configStatus) == Status::Success) {
            clusterServer.SetConfigStatus(configStatus);
        }

        BitMask<WindowCovering::Mode> mode;
        if (WindowCovering::Attributes::Mode::GetDefault(endpointId, mode) == Status::Success) {
            clusterServer.SetMode(mode);
        }

        if (features.HasAll(WindowCovering::Feature::kLift, WindowCovering::Feature::kPositionAwareLift)) {
            DataModel::Nullable<Percent100ths> percent100ths;
            if (WindowCovering::Attributes::CurrentPositionLiftPercent100ths::GetDefault(endpointId, percent100ths) ==
                    Status::Success) {
                clusterServer.SetCurrentPositionLiftPercent100ths(percent100ths);
            }
            if (WindowCovering::Attributes::TargetPositionLiftPercent100ths::GetDefault(endpointId, percent100ths) ==
                    Status::Success) {
                clusterServer.SetTargetPositionLiftPercent100ths(percent100ths);
            }
        }

        if (features.HasAll(WindowCovering::Feature::kTilt, WindowCovering::Feature::kPositionAwareTilt)) {
            DataModel::Nullable<Percent100ths> percent100ths;
            if (WindowCovering::Attributes::CurrentPositionTiltPercent100ths::GetDefault(endpointId, percent100ths) ==
                    Status::Success) {
                clusterServer.SetCurrentPositionTiltPercent100ths(percent100ths);
            }
            if (WindowCovering::Attributes::TargetPositionTiltPercent100ths::GetDefault(endpointId, percent100ths) ==
                    Status::Success) {
                clusterServer.SetTargetPositionTiltPercent100ths(percent100ths);
            }
        }
    }

    CHIP_ERROR err = data_model::provider::get_instance().registry().Register(entry.server.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register WindowCovering on endpoint %u: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }
}

void ESPMatterWindowCoveringClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end());
    VerifyOrReturn(it->second.server.IsConstructed());

    CHIP_ERROR err = data_model::provider::get_instance().registry().Unregister(&it->second.server.Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister WindowCovering on endpoint %u: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }

    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.server.Destroy();
        gServers.erase(it);
    }
}

__attribute__((weak)) void MatterWindowCoveringPluginServerInitCallback() {}
__attribute__((weak)) void MatterWindowCoveringPluginServerShutdownCallback() {}

namespace chip::app::Clusters::WindowCovering {

WindowCoveringCluster * FindClusterOnEndpoint(EndpointId endpointId)
{
    auto it = gServers.find(endpointId);
    if (it == gServers.end() || !it->second.server.IsConstructed()) {
        return nullptr;
    }
    return &it->second.server.Cluster();
}

void SetDefaultDelegate(EndpointId endpointId, WindowCoveringDelegate *delegate)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end() && it->second.server.IsConstructed(),
                   ChipLogError(Zcl, "WindowCovering cluster on endpoint %u not found", endpointId));
    it->second.delegate.SetWrapped(delegate);
}

} // namespace chip::app::Clusters::WindowCovering
