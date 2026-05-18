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
#include <app/server/Server.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <platform/DeviceControlServer.h>
#include <lib/support/CodeUtils.h>
#include <platform/ESP32/NetworkCommissioningDriver.h>
#include <platform/OpenThread/GenericNetworkCommissioningThreadDriver.h>
#include <platform/PlatformManager.h>
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

NetworkCommissioningCluster::Context MakeNetworkCommissioningClusterContext()
{
    return NetworkCommissioningCluster::Context{
        .breadcrumbTracker   = gBreadcrumbTracker,
        .failSafeContext     = Server::GetInstance().GetFailSafeContext(),
        .platformManager     = DeviceLayer::PlatformMgr(),
        .deviceControlServer = DeviceLayer::DeviceControlServer::DeviceControlSvr(),
    };
}

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
        return ret < kNetworkCommissioningClusterCount ? ret : UINT16_MAX;
    }
    return UINT16_MAX;
}

bool endpointIdIsValid(EndpointId endpointId)
{
    bool endpointIdMatched = false;
#ifdef CONFIG_THREAD_NETWORK_COMMISSIONING_DRIVER
    endpointIdMatched |= endpointId == CONFIG_THREAD_NETWORK_ENDPOINT_ID;
#endif
#ifdef CONFIG_WIFI_NETWORK_COMMISSIONING_DRIVER
    endpointIdMatched |= endpointId == CONFIG_WIFI_NETWORK_ENDPOINT_ID;
#endif
#ifdef CONFIG_ETHERNET_NETWORK_COMMISSIONING_DRIVER
    endpointIdMatched |= endpointId == CONFIG_ETHERNET_NETWORK_ENDPOINT_ID;
#endif
    return endpointIdMatched;
}

} // namespace

void ESPMatterNetworkCommissioningClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(endpointIdIsValid(endpointId));
    uint16_t index = GetServerIndex(endpointId);
    VerifyOrReturn(index != UINT16_MAX);
    if (!gServers[index].IsConstructed()) {
#ifdef CONFIG_THREAD_NETWORK_COMMISSIONING_DRIVER
        if (endpointId == CONFIG_THREAD_NETWORK_ENDPOINT_ID) {
            static DeviceLayer::NetworkCommissioning::GenericThreadDriver sThreadDriver;
            gServers[index].Create(endpointId, &sThreadDriver, MakeNetworkCommissioningClusterContext());
        }
#endif
#ifdef CONFIG_WIFI_NETWORK_COMMISSIONING_DRIVER
        if (endpointId == CONFIG_WIFI_NETWORK_ENDPOINT_ID) {
            gServers[index].Create(endpointId, &(DeviceLayer::NetworkCommissioning::ESPWiFiDriver::GetInstance()),
                                   MakeNetworkCommissioningClusterContext());
        }
#endif
#ifdef CONFIG_ETHERNET_NETWORK_COMMISSIONING_DRIVER
        if (endpointId == CONFIG_ETHERNET_NETWORK_ENDPOINT_ID) {
            gServers[index].Create(endpointId, &(DeviceLayer::NetworkCommissioning::ESPEthernetDriver::GetInstance()),
                                   MakeNetworkCommissioningClusterContext());
        }
#endif
    }
    LogErrorOnFailure(gServers[index].Cluster().Init());
    LogErrorOnFailure(esp_matter::data_model::provider::get_instance().registry().Register(gServers[index].Registration()));
}

void ESPMatterNetworkCommissioningClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    VerifyOrReturn(endpointIdIsValid(endpointId));
    uint16_t index = GetServerIndex(endpointId);
    VerifyOrReturn(index != UINT16_MAX);
    LogErrorOnFailure(esp_matter::data_model::provider::get_instance().registry().Unregister(&gServers[index].Cluster(),
                                                                                             shutdownType));
    gServers[index].Cluster().Deinit();
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        gServers[index].Destroy();
    }
}

void MatterNetworkCommissioningPluginServerInitCallback()
{
}
void MatterNetworkCommissioningPluginServerShutdownCallback()
{
}
