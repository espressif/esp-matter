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
namespace icd_management {

inline constexpr uint32_t Id = 0x0046;

namespace feature {
namespace CheckInProtocolSupport {
inline constexpr uint32_t Id = 0x1;
} /* CheckInProtocolSupport */
namespace UserActiveModeTrigger {
inline constexpr uint32_t Id = 0x2;
} /* UserActiveModeTrigger */
namespace LongIdleTimeSupport {
inline constexpr uint32_t Id = 0x4;
} /* LongIdleTimeSupport */
namespace DynamicSitLitSupport {
inline constexpr uint32_t Id = 0x8;
} /* DynamicSitLitSupport */
} /* feature */

namespace attribute {
namespace IdleModeDuration {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint32_t Min = 1;
inline constexpr uint32_t Max = 64800;
} /* IdleModeDuration */
namespace ActiveModeDuration {
inline constexpr uint32_t Id = 0x0001;
} /* ActiveModeDuration */
namespace ActiveModeThreshold {
inline constexpr uint32_t Id = 0x0002;
} /* ActiveModeThreshold */
namespace RegisteredClients {
inline constexpr uint32_t Id = 0x0003;
} /* RegisteredClients */
namespace ICDCounter {
inline constexpr uint32_t Id = 0x0004;
} /* ICDCounter */
namespace ClientsSupportedPerFabric {
inline constexpr uint32_t Id = 0x0005;
inline constexpr uint16_t Min = 1;
inline constexpr uint16_t Max = 65535;
} /* ClientsSupportedPerFabric */
namespace UserActiveModeTriggerHint {
inline constexpr uint32_t Id = 0x0006;
inline constexpr uint32_t Min = 0;
inline constexpr uint32_t Max = 131071;
} /* UserActiveModeTriggerHint */
namespace UserActiveModeTriggerInstruction {
inline constexpr uint32_t Id = 0x0007;
} /* UserActiveModeTriggerInstruction */
namespace OperatingMode {
inline constexpr uint32_t Id = 0x0008;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 1;
} /* OperatingMode */
namespace MaximumCheckInBackoff {
inline constexpr uint32_t Id = 0x0009;
inline constexpr uint32_t Min = 0;
inline constexpr uint32_t Max = 64800;
} /* MaximumCheckInBackoff */
} /* attribute */

namespace command {
namespace RegisterClient {
inline constexpr uint32_t Id = 0x00;
} /* RegisterClient */
namespace RegisterClientResponse {
inline constexpr uint32_t Id = 0x01;
} /* RegisterClientResponse */
namespace UnregisterClient {
inline constexpr uint32_t Id = 0x02;
} /* UnregisterClient */
namespace StayActiveRequest {
inline constexpr uint32_t Id = 0x03;
} /* StayActiveRequest */
namespace StayActiveResponse {
inline constexpr uint32_t Id = 0x04;
} /* StayActiveResponse */
} /* command */

} /* icd_management */
} /* cluster */
} /* esp_matter */
