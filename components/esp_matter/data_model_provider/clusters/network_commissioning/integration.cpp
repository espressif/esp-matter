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

#include <sdkconfig.h>

#include <app/ClusterCallbacks.h>
#include <app/clusters/network-commissioning/NetworkCommissioningCluster.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/support/CodeUtils.h>
#include <platform/ESP32/NetworkCommissioningDriver.h>
#include <platform/OpenThread/GenericNetworkCommissioningThreadDriver.h>
#include <clusters/general_commissioning/integration.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;

namespace {
#ifdef CONFIG_THREAD_NETWORK_COMMISSIONING_DRIVER
constexpr size_t kThreadNetworkCommissioningClusterCount = 1;
#else
constexpr size_t kThreadNetworkCommissioningClusterCount = 0;
#endif
#ifdef CONFIG_WIFI_NETWORK_COMMISSIONING_DRIVER
constexpr size_t kWifiNetworkCommissioningClusterCount = 1;
#else
constexpr size_t kWifiNetworkCommissioningClusterCount = 0;
#endif

#ifdef CONFIG_ETHERNET_NETWORK_COMMISSIONING_DRIVER
constexpr size_t kEthernetNetworkCommissioningClusterCount = 1;
#else
constexpr size_t kEthernetNetworkCommissioningClusterCount = 0;
#endif

constexpr size_t kNetworkCommissioningClusterCount = kThreadNetworkCommissioningClusterCount +
                                                     kWifiNetworkCommissioningClusterCount + kEthernetNetworkCommissioningClusterCount;

static_assert(kNetworkCommissioningClusterCount > 0, "NetworkCommissioningCluster count must be more than 0");

LazyRegisteredServerCluster<NetworkCommissioningCluster> gServers[kNetworkCommissioningClusterCount];

class ESPMatterGeneralCommissioningBreadcrumbTracker : public BreadCrumbTracker {
public:
    void SetBreadCrumb(uint64_t value) override
    {
        chip::app::Clusters::GeneralCommissioning::Instance()->SetBreadCrumb(value);
    }
};

ESPMatterGeneralCommissioningBreadcrumbTracker gBreadcrumbTracker;

uint16_t GetServerIndex(EndpointId endpointId)
{
    esp_matter::endpoint_t *ep = esp_matter::endpoint::get(endpointId);
    if (ep) {
        esp_matter::cluster_t *cluster = esp_matter::cluster::get(ep, NetworkCommissioning::Id);
        if (!cluster) {
            return UINT16_MAX;
        }
        ep = esp_matter::endpoint::get_first(esp_matter::node::get());
        uint16_t ret = 0;
        while (ep && esp_matter::endpoint::get_id(ep) != endpointId) {
            if (esp_matter::cluster::get(ep, NetworkCommissioning::Id)) {
                ret++;
            }
            ep = esp_matter::endpoint::get_next(ep);
        }
        return ret;
    }
    return UINT16_MAX;
}

} // namespace

void ESPMatterNetworkCommissioningClusterServerInitCallback(EndpointId endpointId)
{
    uint16_t index = GetServerIndex(endpointId);
    VerifyOrReturn(index != UINT16_MAX);
#ifdef CONFIG_THREAD_NETWORK_COMMISSIONING_DRIVER
    if (endpointId == CONFIG_THREAD_NETWORK_ENDPOINT_ID) {
        static DeviceLayer::NetworkCommissioning::GenericThreadDriver sThreadDriver;
        gServers[index].Create(endpointId, &sThreadDriver, gBreadcrumbTracker);
        LogErrorOnFailure(gServers[index].Cluster().Init());
        LogErrorOnFailure(esp_matter::data_model::provider::get_instance().registry().Register(gServers[index].Registration()));
    }
#endif
#ifdef CONFIG_WIFI_NETWORK_COMMISSIONING_DRIVER
    if (endpointId == CONFIG_WIFI_NETWORK_ENDPOINT_ID) {
        gServers[index].Create(endpointId, &(DeviceLayer::NetworkCommissioning::ESPWiFiDriver::GetInstance()), gBreadcrumbTracker);
        LogErrorOnFailure(gServers[index].Cluster().Init());
        LogErrorOnFailure(esp_matter::data_model::provider::get_instance().registry().Register(gServers[index].Registration()));
    }
#endif
#ifdef CONFIG_ETHERNET_NETWORK_COMMISSIONING_DRIVER
    if (endpointId == CONFIG_ETHERNET_NETWORK_ENDPOINT_ID) {
        gServers[index].Create(endpointId, &(DeviceLayer::NetworkCommissioning::ESPEthernetDriver::GetInstance()), gBreadcrumbTracker);
        LogErrorOnFailure(gServers[index].Cluster().Init());
        LogErrorOnFailure(esp_matter::data_model::provider::get_instance().registry().Register(gServers[index].Registration()));
    }
#endif
}

void ESPMatterNetworkCommissioningClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    uint16_t index = GetServerIndex(endpointId);
    VerifyOrReturn(index != UINT16_MAX);
#ifdef CONFIG_THREAD_NETWORK_COMMISSIONING_DRIVER
    if (endpointId == CONFIG_THREAD_NETWORK_ENDPOINT_ID) {
        LogErrorOnFailure(esp_matter::data_model::provider::get_instance().registry().Unregister(&gServers[index].Cluster(), shutdownType));
        gServers[index].Cluster().Deinit();
    }
#endif
#ifdef CONFIG_WIFI_NETWORK_COMMISSIONING_DRIVER
    if (endpointId == CONFIG_WIFI_NETWORK_ENDPOINT_ID) {
        LogErrorOnFailure(esp_matter::data_model::provider::get_instance().registry().Unregister(&gServers[index].Cluster(), shutdownType));
        gServers[index].Cluster().Deinit();
    }
#endif
#ifdef CONFIG_ETHERNET_NETWORK_COMMISSIONING_DRIVER
    if (endpointId == CONFIG_ETHERNET_NETWORK_ENDPOINT_ID) {
        LogErrorOnFailure(esp_matter::data_model::provider::get_instance().registry().Unregister(&gServers[index].Cluster(), shutdownType));
        gServers[index].Cluster().Deinit();
    }
#endif
}

void MatterNetworkCommissioningPluginServerInitCallback()
{
}
void MatterNetworkCommissioningPluginServerShutdownCallback()
{
}
