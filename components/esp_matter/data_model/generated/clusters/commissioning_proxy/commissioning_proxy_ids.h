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
namespace commissioning_proxy {

inline constexpr uint32_t Id = 0x0455;

namespace feature {
namespace WiFiNetworkInterface {
inline constexpr uint32_t Id = 0x1;
} /* WiFiNetworkInterface */
namespace BackgroundScan {
inline constexpr uint32_t Id = 0x2;
} /* BackgroundScan */
} /* feature */

namespace attribute {
namespace Transport {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint8_t Min = 1;
inline constexpr uint8_t Max = 31;
} /* Transport */
namespace ScanMaxTime {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint8_t Min = 1;
inline constexpr uint8_t Max = 255;
} /* ScanMaxTime */
namespace MaxSessions {
inline constexpr uint32_t Id = 0x0002;
inline constexpr uint8_t Min = 1;
inline constexpr uint8_t Max = 255;
} /* MaxSessions */
namespace MaxCachedResults {
inline constexpr uint32_t Id = 0x0003;
inline constexpr uint8_t Min = 1;
inline constexpr uint8_t Max = 255;
} /* MaxCachedResults */
namespace NumCachedResults {
inline constexpr uint32_t Id = 0x0004;
} /* NumCachedResults */
namespace CacheTimeout {
inline constexpr uint32_t Id = 0x0005;
inline constexpr uint16_t Min = 1;
inline constexpr uint16_t Max = 65535;
} /* CacheTimeout */
namespace CachedResults {
inline constexpr uint32_t Id = 0x0006;
} /* CachedResults */
namespace WiFiBand {
inline constexpr uint32_t Id = 0x0007;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 7;
} /* WiFiBand */
} /* attribute */

namespace command {
namespace ProxyConnectRequest {
inline constexpr uint32_t Id = 0x00;
} /* ProxyConnectRequest */
namespace ProxyConnectResponse {
inline constexpr uint32_t Id = 0x01;
} /* ProxyConnectResponse */
namespace ProxyDisconnectRequest {
inline constexpr uint32_t Id = 0x02;
} /* ProxyDisconnectRequest */
namespace ProxyScanRequest {
inline constexpr uint32_t Id = 0x03;
} /* ProxyScanRequest */
namespace ProxyScanResponse {
inline constexpr uint32_t Id = 0x04;
} /* ProxyScanResponse */
namespace ProxyBackGroundScanStartRequest {
inline constexpr uint32_t Id = 0x05;
} /* ProxyBackGroundScanStartRequest */
namespace ProxyBackGroundScanStopRequest {
inline constexpr uint32_t Id = 0x06;
} /* ProxyBackGroundScanStopRequest */
namespace ProxyMessageRequest {
inline constexpr uint32_t Id = 0x07;
} /* ProxyMessageRequest */
namespace ProxyMessageResponse {
inline constexpr uint32_t Id = 0x08;
} /* ProxyMessageResponse */
} /* command */

} /* commissioning_proxy */
} /* cluster */
} /* esp_matter */
