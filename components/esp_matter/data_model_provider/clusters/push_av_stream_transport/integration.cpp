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
#include <app/clusters/push-av-stream-transport-server/PushAVStreamTransportCluster.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/support/CodeUtils.h>
#include <data_model/esp_matter_attribute_helpers.h>
#include <lib/support/logging/CHIPLogging.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::PushAvStreamTransport::Attributes;
using namespace esp_matter;

namespace {
std::unordered_map<EndpointId, LazyRegisteredServerCluster<PushAvStreamTransportServer>> gServers;

} // namespace

void ESPMatterPushAvStreamTransportClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(cluster::get(endpointId, PushAvStreamTransport::Id) != nullptr,
                   ChipLogError(AppServer,
                                "PushAvStreamTransport: cluster missing in esp-matter data model for endpoint %u",
                                endpointId));
    if (!gServers[endpointId].IsConstructed()) {
        uint32_t rawFeatureMap = read_feature_map_u32(endpointId, PushAvStreamTransport::Id);
        ChipLogProgress(AppServer, "Registering Push AV Stream Transport on endpoint %u", endpointId);
        gServers[endpointId].Create(endpointId, BitFlags<PushAvStreamTransport::Feature>(rawFeatureMap));
    }
    CHIP_ERROR err = data_model::provider::get_instance().registry().Register(gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer,
                     "Failed to register Push AV Stream Transport on endpoint %u - Error: %" CHIP_ERROR_FORMAT,
                     endpointId, err.Format());
    }
}

void ESPMatterPushAvStreamTransportClusterServerShutdownCallback(EndpointId endpointId,
                                                                 ClusterShutdownType shutdownType)
{
    VerifyOrReturn(cluster::get(endpointId, PushAvStreamTransport::Id) != nullptr,
                   ChipLogError(AppServer,
                                "PushAvStreamTransport: cluster missing in esp-matter data model for endpoint %u",
                                endpointId));
    auto it = gServers.find(endpointId);
    VerifyOrReturn(it != gServers.end());
    VerifyOrReturn(it->second.IsConstructed());
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&it->second.Cluster(),
                                                                                            shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer,
                     "Failed to unregister Push AV Stream Transport on endpoint %u - Error: %" CHIP_ERROR_FORMAT,
                     endpointId, err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        it->second.Destroy();
        gServers.erase(it);
    }
}

void MatterPushAvStreamTransportPluginServerInitCallback() {}

void MatterPushAvStreamTransportPluginServerShutdownCallback() {}

namespace chip::app::Clusters::PushAvStreamTransport {

void SetDelegate(EndpointId endpointId, PushAvStreamTransportDelegate * delegate)
{
    gServers[endpointId].Cluster().SetDelegate(delegate);
    (void)gServers[endpointId].Cluster().Init();
}

void SetTLSClientManagementDelegate(EndpointId endpointId, TLSClientManagementDelegate * delegate)
{
    gServers[endpointId].Cluster().SetTLSClientManagementDelegate(delegate);
}

void SetTLSCertificateManagementDelegate(EndpointId endpointId, TLSCertificateManagementDelegate * delegate)
{
    gServers[endpointId].Cluster().SetTLSCertificateManagementDelegate(delegate);
}

}
