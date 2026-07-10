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

/* THIS IS A GENERATED FILE, DO NOT EDIT */

#pragma once
#include <esp_matter_data_model.h>

namespace esp_matter {
namespace cluster {
namespace network_commissioning {

inline constexpr uint32_t Id = 0x0031;

namespace feature {
namespace WiFiNetworkInterface {
inline constexpr uint32_t Id = 0x1;
} /* WiFiNetworkInterface */
namespace ThreadNetworkInterface {
inline constexpr uint32_t Id = 0x2;
} /* ThreadNetworkInterface */
namespace EthernetNetworkInterface {
inline constexpr uint32_t Id = 0x4;
} /* EthernetNetworkInterface */
} /* feature */

namespace attribute {
namespace MaxNetworks {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint8_t Min = 1;
inline constexpr uint8_t Max = 255;
} /* MaxNetworks */
namespace Networks {
inline constexpr uint32_t Id = 0x0001;
} /* Networks */
namespace ScanMaxTimeSeconds {
inline constexpr uint32_t Id = 0x0002;
} /* ScanMaxTimeSeconds */
namespace ConnectMaxTimeSeconds {
inline constexpr uint32_t Id = 0x0003;
} /* ConnectMaxTimeSeconds */
namespace InterfaceEnabled {
inline constexpr uint32_t Id = 0x0004;
} /* InterfaceEnabled */
namespace LastNetworkingStatus {
inline constexpr uint32_t Id = 0x0005;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 12;
} /* LastNetworkingStatus */
namespace LastNetworkID {
inline constexpr uint32_t Id = 0x0006;
} /* LastNetworkID */
namespace LastConnectErrorValue {
inline constexpr uint32_t Id = 0x0007;
} /* LastConnectErrorValue */
namespace SupportedWiFiBands {
inline constexpr uint32_t Id = 0x0008;
} /* SupportedWiFiBands */
namespace SupportedThreadFeatures {
inline constexpr uint32_t Id = 0x0009;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65535;
} /* SupportedThreadFeatures */
namespace ThreadVersion {
inline constexpr uint32_t Id = 0x000A;
} /* ThreadVersion */
} /* attribute */

namespace command {
namespace ScanNetworks {
inline constexpr uint32_t Id = 0x00;
} /* ScanNetworks */
namespace ScanNetworksResponse {
inline constexpr uint32_t Id = 0x01;
} /* ScanNetworksResponse */
namespace AddOrUpdateWiFiNetwork {
inline constexpr uint32_t Id = 0x02;
} /* AddOrUpdateWiFiNetwork */
namespace AddOrUpdateThreadNetwork {
inline constexpr uint32_t Id = 0x03;
} /* AddOrUpdateThreadNetwork */
namespace RemoveNetwork {
inline constexpr uint32_t Id = 0x04;
} /* RemoveNetwork */
namespace NetworkConfigResponse {
inline constexpr uint32_t Id = 0x05;
} /* NetworkConfigResponse */
namespace ConnectNetwork {
inline constexpr uint32_t Id = 0x06;
} /* ConnectNetwork */
namespace ConnectNetworkResponse {
inline constexpr uint32_t Id = 0x07;
} /* ConnectNetworkResponse */
namespace ReorderNetwork {
inline constexpr uint32_t Id = 0x08;
} /* ReorderNetwork */
} /* command */

} /* network_commissioning */
} /* cluster */
} /* esp_matter */
