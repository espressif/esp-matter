/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <sdkconfig.h>

#include <lib/core/CHIPConfig.h>
#include <platform/ESP32/NetworkCommissioningDriver.h>
#include <platform/OpenThread/GenericNetworkCommissioningThreadDriver.h>
#include <app/clusters/network-commissioning/network-commissioning.h>

using namespace chip;
using namespace chip::DeviceLayer;

namespace {

#if CHIP_DEVICE_CONFIG_ENABLE_WIFI && CHIP_DEVICE_CONFIG_WIFI_NETWORK_DRIVER
app::Clusters::NetworkCommissioning::Instance
sWiFiNetworkCommissioningInstance(CONFIG_WIFI_NETWORK_ENDPOINT_ID /* Endpoint Id */,
                                  &(NetworkCommissioning::ESPWiFiDriver::GetInstance()));
#endif

#if CHIP_DEVICE_CONFIG_ENABLE_ETHERNET && CHIP_DEVICE_CONFIG_ETHERNET_NETWORK_DRIVER
app::Clusters::NetworkCommissioning::Instance
sEthernetNetworkCommissioningInstance(CONFIG_ETHERNET_NETWORK_ENDPOINT_ID /* Endpoint Id */,
                                      &(NetworkCommissioning::ESPEthernetDriver::GetInstance()));
#endif

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD && CONFIG_THREAD_NETWORK_COMMISSIONING_DRIVER
app::Clusters::NetworkCommissioning::InstanceAndDriver<NetworkCommissioning::GenericThreadDriver>
sThreadNetworkDriver(CONFIG_THREAD_NETWORK_ENDPOINT_ID);
#endif

#if defined(CONFIG_WIFI_NETWORK_ENDPOINT_ID) && defined(CONFIG_THREAD_NETWORK_ENDPOINT_ID)
static_assert(CONFIG_WIFI_NETWORK_ENDPOINT_ID != CONFIG_THREAD_NETWORK_ENDPOINT_ID,
              "Wi-Fi network endpoint id and Thread network endpoint id should not be the same.");
#endif
#if defined(CONFIG_WIFI_NETWORK_ENDPOINT_ID) && defined(CONFIG_ETHERNET_NETWORK_ENDPOINT_ID)
static_assert(CONFIG_WIFI_NETWORK_ENDPOINT_ID != CONFIG_ETHERNET_NETWORK_ENDPOINT_ID,
              "Wi-Fi network endpoint id and Ethernet network endpoint id should not be the same.");
#endif
#if defined(CONFIG_THREAD_NETWORK_ENDPOINT_ID) && defined(CONFIG_ETHERNET_NETWORK_ENDPOINT_ID)
static_assert(CONFIG_THREAD_NETWORK_ENDPOINT_ID != CONFIG_ETHERNET_NETWORK_ENDPOINT_ID,
              "Thread network endpoint id and Ethernet network endpoint id should not be the same.");
#endif
} // namespace

void init_network_driver()
{
#if CHIP_DEVICE_CONFIG_WIFI_NETWORK_DRIVER
    sWiFiNetworkCommissioningInstance.Init();
#endif // CHIP_DEVICE_CONFIG_WIFI_NETWORK_DRIVER
#if CHIP_DEVICE_CONFIG_ETHERNET_NETWORK_DRIVER
    sEthernetNetworkCommissioningInstance.Init();
#endif // CHIP_DEVICE_CONFIG_ETHERNET_NETWORK_DRIVER
#ifdef CONFIG_THREAD_NETWORK_COMMISSIONING_DRIVER
    sThreadNetworkDriver.Init();
#endif // CONFIG_THREAD_NETWORK_COMMISSIONING_DRIVER
}
