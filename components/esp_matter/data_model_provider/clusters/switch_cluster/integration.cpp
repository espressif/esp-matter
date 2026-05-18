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
#include <app/clusters/switch-server/SwitchCluster.h>
#include <clusters/Switch/Attributes.h>
#include <clusters/Switch/ClusterId.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <data_model/esp_matter_attribute_helpers.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::Switch;
using namespace esp_matter;

namespace {

std::unordered_map<EndpointId, LazyRegisteredServerCluster<SwitchCluster>> gServers;

} // namespace

void ESPMatterSwitchClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(cluster::get(endpointId, Switch::Id) != nullptr,
                   ChipLogError(AppServer, "Switch: cluster missing in esp-matter data model for endpoint %u", endpointId));

    if (!gServers[endpointId].IsConstructed()) {
        const uint32_t rawFeatureMap = read_feature_map_u32(endpointId, Switch::Id);
        BitFlags<Switch::Feature> features(rawFeatureMap);

        uint8_t numberOfPositions = 0;
        VerifyOrReturn(read_attribute_raw_value(endpointId, Switch::Id, Attributes::NumberOfPositions::Id, numberOfPositions),
                       ChipLogError(AppServer, "Switch: NumberOfPositions read failed on ep %u", endpointId));

        uint8_t multiPressMax = 0;
        if (features.Has(Switch::Feature::kMomentarySwitchMultiPress)) {
            VerifyOrReturn(read_attribute_raw_value(endpointId, Switch::Id, Attributes::MultiPressMax::Id, multiPressMax),
                           ChipLogError(AppServer, "Switch: MultiPressMax read failed on ep %u", endpointId));
        }

        SwitchCluster::StartupConfiguration startupConfig{ .numberOfPositions = numberOfPositions, .multiPressMax = multiPressMax };

        gServers[endpointId].Create(endpointId, features, startupConfig);
    }
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(
                         gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Switch cluster register failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
}

void ESPMatterSwitchClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end());
    VerifyOrReturn(it->second.IsConstructed());
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&it->second.Cluster(),
                                                                                            shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Switch cluster unregister failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.Destroy();
        gServers.erase(it);
    }
}

void MatterSwitchPluginServerInitCallback() {}

namespace chip {
namespace app {
namespace Clusters {
namespace Switch {

SwitchCluster * FindClusterOnEndpoint(EndpointId endpointId)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturnValue(it != gServers.end(), nullptr);
    VerifyOrReturnValue(it->second.IsConstructed(), nullptr);
    return &it->second.Cluster();
}

} // namespace Switch
} // namespace Clusters
} // namespace app
} // namespace chip
