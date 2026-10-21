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
namespace proximity_ranging {

inline constexpr uint32_t Id = 0x0433;

namespace feature {
namespace WiFiUsdProximityDetection {
inline constexpr uint32_t Id = 0x1;
} /* WiFiUsdProximityDetection */
namespace BluetoothChannelSounding {
inline constexpr uint32_t Id = 0x2;
} /* BluetoothChannelSounding */
namespace BleBeaconRssi {
inline constexpr uint32_t Id = 0x4;
} /* BleBeaconRssi */
} /* feature */

namespace attribute {
namespace RangingCapabilities {
inline constexpr uint32_t Id = 0x0000;
} /* RangingCapabilities */
namespace WiFiDevIK {
inline constexpr uint32_t Id = 0x0001;
} /* WiFiDevIK */
namespace BLEDeviceID {
inline constexpr uint32_t Id = 0x0002;
} /* BLEDeviceID */
namespace BLTDevIK {
inline constexpr uint32_t Id = 0x0003;
} /* BLTDevIK */
namespace BLTCSSecurityLevel {
inline constexpr uint32_t Id = 0x0004;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* BLTCSSecurityLevel */
namespace BLTCSModeCapability {
inline constexpr uint32_t Id = 0x0005;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* BLTCSModeCapability */
namespace SessionIDList {
inline constexpr uint32_t Id = 0x0006;
} /* SessionIDList */
namespace RangingConstraints {
inline constexpr uint32_t Id = 0x0007;
} /* RangingConstraints */
} /* attribute */

namespace command {
namespace StartRangingRequest {
inline constexpr uint32_t Id = 0x00;
} /* StartRangingRequest */
namespace StartRangingResponse {
inline constexpr uint32_t Id = 0x01;
} /* StartRangingResponse */
namespace StopRangingRequest {
inline constexpr uint32_t Id = 0x02;
} /* StopRangingRequest */
} /* command */

namespace event {
namespace RangingResult {
inline constexpr uint32_t Id = 0x00;
} /* RangingResult */
namespace RangingSessionStatus {
inline constexpr uint32_t Id = 0x01;
} /* RangingSessionStatus */
} /* event */

} /* proximity_ranging */
} /* cluster */
} /* esp_matter */
