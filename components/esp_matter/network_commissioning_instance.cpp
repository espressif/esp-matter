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

#include <esp_matter_core.h>

#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER

#include <app/clusters/network-commissioning/CodegenInstance.h>
#include <platform/ESP32/NetworkCommissioningDriver.h>

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD && CONFIG_THREAD_NETWORK_COMMISSIONING_DRIVER
#include <platform/OpenThread/GenericNetworkCommissioningThreadDriver.h>
#endif

namespace esp_matter {

void network_commissioning_instance_init()
{
#if CHIP_DEVICE_CONFIG_ENABLE_WIFI && CHIP_DEVICE_CONFIG_WIFI_NETWORK_DRIVER
    static chip::app::Clusters::NetworkCommissioning::Instance sWiFiNetworkCommissioningInstance(CONFIG_WIFI_NETWORK_ENDPOINT_ID /* Endpoint Id */,
                                      &(chip::DeviceLayer::NetworkCommissioning::ESPWiFiDriver::GetInstance()));
    sWiFiNetworkCommissioningInstance.Init();
#endif // CHIP_DEVICE_CONFIG_WIFI_NETWORK_DRIVER

#if CHIP_DEVICE_CONFIG_ENABLE_ETHERNET && CHIP_DEVICE_CONFIG_ETHERNET_NETWORK_DRIVER
    static chip::app::Clusters::NetworkCommissioning::Instance sEthernetNetworkCommissioningInstance(CONFIG_ETHERNET_NETWORK_ENDPOINT_ID /* Endpoint Id */,
                                          &(chip::DeviceLayer::NetworkCommissioning::ESPEthernetDriver::GetInstance()));
    sEthernetNetworkCommissioningInstance.Init();
#endif // CHIP_DEVICE_CONFIG_ETHERNET_NETWORK_DRIVER

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD && CONFIG_THREAD_NETWORK_COMMISSIONING_DRIVER
    static chip::app::Clusters::NetworkCommissioning::InstanceAndDriver<chip::DeviceLayer::NetworkCommissioning::GenericThreadDriver> sThreadNetworkDriver(CONFIG_THREAD_NETWORK_ENDPOINT_ID);
    sThreadNetworkDriver.Init();
#endif
}

} // namespace esp_matter

#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER 
