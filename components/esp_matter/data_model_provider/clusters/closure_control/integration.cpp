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

#include <clusters/closure_control/integration.h>

#include <app/ClusterCallbacks.h>
#include <app/clusters/closure-control-server/ClosureControlCluster.h>
#include <clusters/ClosureControl/Attributes.h>
#include <clusters/ClosureControl/ClusterId.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <data_model/esp_matter_attribute_helpers.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/DefaultTimerDelegate.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ClosureControl;
using namespace esp_matter;

namespace {

DefaultTimerDelegate gTimerDelegate;

std::unordered_map<EndpointId, LazyRegisteredServerCluster<ClosureControlCluster>> gServers;
std::unordered_map<EndpointId, ClosureControlClusterDelegate *> gDelegates;

} // namespace

namespace chip {
namespace app {
namespace Clusters {
namespace ClosureControl {

void MatterClosureControlSetDelegate(EndpointId endpointId, ClosureControlClusterDelegate &delegate)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it == gServers.end() || !it->second.IsConstructed(),
                   ChipLogError(AppServer, "ClosureControl: cluster already initialized; cannot set delegate"));
    gDelegates[endpointId] = &delegate;
}

} // namespace ClosureControl
} // namespace Clusters
} // namespace app
} // namespace chip

void ESPMatterClosureControlClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(cluster::get(endpointId, ClosureControl::Id) != nullptr,
                   ChipLogError(AppServer, "ClosureControl: cluster missing in esp-matter data model for endpoint %u", endpointId));
    if (!gServers[endpointId].IsConstructed()) {
        ClosureControlClusterDelegate *delegate = nullptr;
        {
            auto it = gDelegates.find(endpointId);
            VerifyOrReturn(it != gDelegates.end() && it->second != nullptr,
                           ChipLogError(AppServer,
                                        "ClosureControl: delegate not set for ep %u (call MatterClosureControlSetDelegate first)",
                                        endpointId));
            delegate = it->second;
        }

        // Read feature map from esp-matter data model
        BitFlags<ClosureControl::Feature> features(read_feature_map_u32(endpointId, ClosureControl::Id));

        // Build Config from esp-matter data model
        ClosureControlCluster::Config config(endpointId, *delegate, gTimerDelegate);

        if (features.Has(Feature::kPositioning)) {
            config.WithPositioning();
        }
        if (features.Has(Feature::kMotionLatching)) {
            config.WithMotionLatching({});
        }
        if (features.Has(Feature::kInstantaneous)) {
            config.WithInstantaneous();
        }
        if (features.Has(Feature::kSpeed)) {
            config.WithSpeed();
        }
        if (features.Has(Feature::kVentilation)) {
            config.WithVentilation();
        }
        if (features.Has(Feature::kPedestrian)) {
            config.WithPedestrian();
        }
        if (features.Has(Feature::kCalibration)) {
            config.WithCalibration();
        }
        if (features.Has(Feature::kProtection)) {
            config.WithProtection();
        }
        if (features.Has(Feature::kManuallyOperable)) {
            config.WithManuallyOperable();
        }
        if (endpoint::is_attribute_enabled(endpointId, ClosureControl::Id, Attributes::CountdownTime::Id)) {
            config.WithCountdownTime();
        }

        // MainState, OverallCurrentState, LatchControlModes are MANAGED_INTERNALLY —
        // no reliable values in esp-matter store. Use Config defaults; the code-driven
        // cluster initializes them properly via Startup().

        gServers[endpointId].Create(config);
    }
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(
                         gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "ClosureControl register failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
}

void ESPMatterClosureControlClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end());
    VerifyOrReturn(it->second.IsConstructed());
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&it->second.Cluster(),
                                                                                            shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "ClosureControl unregister failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.Destroy();
        gServers.erase(it);
    }
}

void MatterClosureControlPluginServerInitCallback() {}
