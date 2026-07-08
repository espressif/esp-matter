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
#include <app/ConcreteAttributePath.h>
#include <app/clusters/fan-control-server/FanControlCluster.h>
#include <app/clusters/fan-control-server/fan-control-delegate.h>
#include <app/util/attribute-metadata.h>
#include <clusters/FanControl/Attributes.h>
#include <clusters/FanControl/ClusterId.h>
#include <clusters/FanControl/EnumsCheck.h>
#include <data_model/esp_matter_attribute_helpers.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::FanControl;
using namespace chip::app::Clusters::FanControl::Attributes;
using namespace esp_matter;

namespace {

// FanControlCluster requires a Delegate& (not pointer) at construction.
// This wrapper satisfies that requirement and optionally forwards to an
// application delegate registered later via SetDefaultDelegate.
class FanControlDelegateWrapper final : public FanControl::Delegate {
public:
    FanControlDelegateWrapper() : FanControl::Delegate(kInvalidEndpointId) {}

    void SetWrapped(EndpointId ep, FanControl::Delegate * delegate)
    {
        mEndpoint = ep;
        mWrapped  = delegate;
    }

    Protocols::InteractionModel::Status HandleStep(StepDirectionEnum dir, bool wrap, bool lowestOff) override
    {
        return mWrapped ? mWrapped->HandleStep(dir, wrap, lowestOff) : Protocols::InteractionModel::Status::Failure;
    }

    void OnFanDriveStateChanged(const FanDriveState  &state) override
    {
        if (mWrapped) {
            mWrapped->OnFanDriveStateChanged(state);
        }
    }

    void OnRockSettingChanged(BitMask<RockBitmap> v) override
    {
        if (mWrapped) {
            mWrapped->OnRockSettingChanged(v);
        }
    }

    void OnWindSettingChanged(BitMask<WindBitmap> v) override
    {
        if (mWrapped) {
            mWrapped->OnWindSettingChanged(v);
        }
    }

    void OnAirflowDirectionChanged(AirflowDirectionEnum v) override
    {
        if (mWrapped) {
            mWrapped->OnAirflowDirectionChanged(v);
        }
    }

private:
    FanControl::Delegate * mWrapped = nullptr;
};

struct FanControlEndpoint {
    FanControlDelegateWrapper delegateWrapper;
    LazyRegisteredServerCluster<FanControlCluster> server;
};

std::unordered_map<EndpointId, FanControlEndpoint> gServers;

} // namespace

void ESPMatterFanControlClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(cluster::get(endpointId, FanControl::Id) != nullptr,
                   ChipLogError(AppServer, "FanControl: cluster missing in esp-matter data model for endpoint %u", endpointId));

    auto  &entry = gServers[endpointId];
    if (!entry.server.IsConstructed()) {
        const uint32_t featureMap = read_feature_map_u32(endpointId, FanControl::Id);
        BitFlags<FanControl::Feature> features(featureMap);

        entry.delegateWrapper.SetWrapped(endpointId, nullptr);
        FanControlCluster::Config config(endpointId, entry.delegateWrapper);

        // FanModeSequence
        uint8_t fanModeSeqRaw = 0;
        FanModeSequenceEnum fanModeSequence = features.Has(FanControl::Feature::kAuto)
                                              ? FanModeSequenceEnum::kOffLowHighAuto : FanModeSequenceEnum::kOffLowHigh;
        if (read_attribute_raw_value(endpointId, FanControl::Id, FanModeSequence::Id, fanModeSeqRaw)) {
            auto parsed = static_cast<FanModeSequenceEnum>(fanModeSeqRaw);
            if (EnsureKnownEnumValue(parsed) != FanModeSequenceEnum::kUnknownEnumValue) {
                fanModeSequence = parsed;
            }
        }
        config.WithFanModeSequence(fanModeSequence);

        if (features.Has(FanControl::Feature::kMultiSpeed)) {
            uint8_t speedMax = 100;
            read_attribute_raw_value(endpointId, FanControl::Id, SpeedMax::Id, speedMax);
            config.WithSpeedMax(speedMax);
        }
        if (features.Has(FanControl::Feature::kRocking)) {
            uint8_t rockSupportRaw = 0;
            read_attribute_raw_value(endpointId, FanControl::Id, RockSupport::Id, rockSupportRaw);
            config.WithRockSupport(BitMask<RockBitmap>(rockSupportRaw));
        }
        if (features.Has(FanControl::Feature::kWind)) {
            uint8_t windSupportRaw = 0;
            read_attribute_raw_value(endpointId, FanControl::Id, WindSupport::Id, windSupportRaw);
            config.WithWindSupport(BitMask<WindBitmap>(windSupportRaw));
        }
        if (features.Has(FanControl::Feature::kAirflowDirection)) {
            config.WithAirflowDirection();
        }
        if (features.Has(FanControl::Feature::kStep)) {
            config.WithStep();
        }

        entry.server.Create(config);
    }

    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(entry.server.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "FanControl register failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
}

void ESPMatterFanControlClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end());
    VerifyOrReturn(it->second.server.IsConstructed());
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&it->second.server.Cluster(),
                                                                                            shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "FanControl unregister failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.server.Destroy();
        gServers.erase(it);
    }
}

// Legacy callbacks referenced by esp_matter_cluster.cpp function_list (non-generated path).
// FanControlCluster handles attribute changes internally now.
void MatterFanControlClusterServerAttributeChangedCallback(const ConcreteAttributePath &) {}
Protocols::InteractionModel::Status MatterFanControlClusterServerPreAttributeChangedCallback(
    const ConcreteAttributePath &, EmberAfAttributeType, uint16_t, uint8_t *)
{
    return Protocols::InteractionModel::Status::Success;
}

__attribute__((weak)) void MatterFanControlPluginServerInitCallback() {}
__attribute__((weak)) void MatterFanControlPluginServerShutdownCallback() {}

namespace chip {
namespace app {
namespace Clusters {
namespace FanControl {

void SetDefaultDelegate(EndpointId endpointId, Delegate * delegate)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end() && it->second.server.IsConstructed());
    it->second.delegateWrapper.SetWrapped(endpointId, delegate);
}

} // namespace FanControl
} // namespace Clusters
} // namespace app
} // namespace chip
