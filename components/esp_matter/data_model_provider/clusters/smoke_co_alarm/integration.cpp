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
#include <app/clusters/smoke-co-alarm-server/SmokeCoAlarmCluster.h>
#include <clusters/SmokeCoAlarm/Attributes.h>
#include <clusters/SmokeCoAlarm/ClusterId.h>
#include <data_model/esp_matter_attribute_helpers.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::SmokeCoAlarm;
using namespace chip::app::Clusters::SmokeCoAlarm::Attributes;
using namespace esp_matter;

namespace {

struct SmokeCoAlarmEndpoint {
    LazyRegisteredServerCluster<SmokeCoAlarmCluster> server;
};

std::unordered_map<EndpointId, SmokeCoAlarmEndpoint> gServers;

} // namespace

void ESPMatterSmokeCoAlarmClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(cluster::get(endpointId, SmokeCoAlarm::Id) != nullptr,
                   ChipLogError(AppServer, "SmokeCoAlarm: cluster missing in esp-matter data model for endpoint %u", endpointId));

    auto &entry = gServers[endpointId];
    if (!entry.server.IsConstructed()) {
        const uint32_t featureMap = read_feature_map_u32(endpointId, SmokeCoAlarm::Id);

        SmokeCoAlarmCluster::Config config;
        config.featureMap = BitFlags<Feature>(featureMap);

        SmokeCoAlarmCluster::OptionalAttributeSet optAttribs;
        if (endpoint::is_attribute_enabled(endpointId, SmokeCoAlarm::Id, DeviceMuted::Id)) {
            optAttribs.Set<DeviceMuted::Id>();
        }
        if (endpoint::is_attribute_enabled(endpointId, SmokeCoAlarm::Id, InterconnectSmokeAlarm::Id)) {
            optAttribs.Set<InterconnectSmokeAlarm::Id>();
        }
        if (endpoint::is_attribute_enabled(endpointId, SmokeCoAlarm::Id, InterconnectCOAlarm::Id)) {
            optAttribs.Set<InterconnectCOAlarm::Id>();
        }
        if (endpoint::is_attribute_enabled(endpointId, SmokeCoAlarm::Id, ContaminationState::Id)) {
            optAttribs.Set<ContaminationState::Id>();
        }
        if (endpoint::is_attribute_enabled(endpointId, SmokeCoAlarm::Id, SmokeSensitivityLevel::Id)) {
            optAttribs.Set<SmokeSensitivityLevel::Id>();
        }
        if (endpoint::is_attribute_enabled(endpointId, SmokeCoAlarm::Id, ExpiryDate::Id)) {
            optAttribs.Set<ExpiryDate::Id>();
        }
        if (endpoint::is_attribute_enabled(endpointId, SmokeCoAlarm::Id, Unmounted::Id)) {
            optAttribs.Set<Unmounted::Id>();
        }
        config.optionalAttribs = optAttribs;

        entry.server.Create(endpointId, config);
    }

    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(entry.server.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "SmokeCoAlarm register failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
}

void ESPMatterSmokeCoAlarmClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end());
    VerifyOrReturn(it->second.server.IsConstructed());
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&it->second.server.Cluster(),
                                                                                            shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "SmokeCoAlarm unregister failed ep %u: %" CHIP_ERROR_FORMAT, endpointId, err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.server.Destroy();
        gServers.erase(it);
    }
}

__attribute__((weak)) void MatterSmokeCoAlarmPluginServerInitCallback() {}
__attribute__((weak)) void MatterSmokeCoAlarmPluginServerShutdownCallback() {}

namespace chip {
namespace app {
namespace Clusters {

void SetSmokeCoAlarmDefaultDelegate(EndpointId endpointId, SmokeCoAlarmDelegate * delegate)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end() && it->second.server.IsConstructed());
    it->second.server.Cluster().SetDelegate(delegate);
}

} // namespace Clusters
} // namespace app
} // namespace chip
