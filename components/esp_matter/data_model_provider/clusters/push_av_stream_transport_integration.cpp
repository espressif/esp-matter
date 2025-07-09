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

#include <app/ClusterCallbacks.h>
#include <app/clusters/push-av-stream-transport-server/push-av-stream-transport-cluster.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::PushAvStreamTransport::Attributes;
using namespace esp_matter;

namespace {
std::unordered_map<EndpointId, LazyRegisteredServerCluster<PushAvStreamTransportServer>> gServers;

bool IsClusterEnabled(EndpointId endpointId)
{
    cluster_t *cluster = cluster::get(endpointId, PushAvStreamTransport::Id);
    return cluster != nullptr;
}

uint32_t GetFeatureMap(EndpointId endpointId)
{
    attribute_t *attribute = attribute::get(endpointId, PushAvStreamTransport::Id, Globals::Attributes::FeatureMap::Id);

    VerifyOrReturnError(attribute, ESP_ERR_INVALID_STATE);

    /* Update the value if the attribute already exists */
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    attribute::get_val(attribute, &val);
    return val.val.u32;
}
} // namespace

void ESPMatterPushAvStreamTransportClusterServerInitCallback(EndpointId endpointId)
{
    if (!IsClusterEnabled(endpointId) || gServers[endpointId].IsConstructed()) {
        return;
    }
    uint32_t rawFeatureMap = GetFeatureMap(endpointId);
    ChipLogProgress(AppServer, "Registering Push AV Stream Transport on endpoint %u", endpointId);
    gServers[endpointId].Create(endpointId, BitFlags<PushAvStreamTransport::Feature>(rawFeatureMap));
    CHIP_ERROR err = data_model::provider::get_instance().registry().Register(gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register Push AV Stream Transport on endpoint %u: %" CHIP_ERROR_FORMAT,
                     endpointId, err.Format());
    }
}

void ESPMatterPushAvStreamTransportClusterServerShutdownCallback(EndpointId endpointId)
{
    if (!IsClusterEnabled(endpointId) || !gServers[endpointId].IsConstructed()) {
        return;
    }

    CHIP_ERROR err = data_model::provider::get_instance().registry().Unregister(&gServers[endpointId].Cluster());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister Push AV Stream Transport on endpoint %u: %" CHIP_ERROR_FORMAT,
                     endpointId, err.Format());
    }
    gServers[endpointId].Cluster().Deinit();
    gServers[endpointId].Destroy();
}

void MatterPushAvStreamTransportPluginServerInitCallback() {}

void MatterPushAvStreamTransportPluginServerShutdownCallback() {}

namespace chip {
namespace app {
namespace Clusters {
namespace PushAvStreamTransport {

void SetDelegate(EndpointId endpointId, PushAvStreamTransportDelegate *delegate)
{
    gServers[endpointId].Cluster().SetDelegate(endpointId, delegate);
    gServers[endpointId].Cluster().Init();
}

} // namespace PushAvStreamTransport
} // namespace Clusters
} // namespace app
} // namespace chip
