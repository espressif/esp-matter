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
namespace electrical_protection_alarm {

inline constexpr uint32_t Id = 0x00A3;

namespace feature {
namespace ShortCircuit {
inline constexpr uint32_t Id = 0x100000;
} /* ShortCircuit */
namespace OverLoad {
inline constexpr uint32_t Id = 0x200000;
} /* OverLoad */
namespace OverVoltage {
inline constexpr uint32_t Id = 0x400000;
} /* OverVoltage */
namespace SurgeProtection {
inline constexpr uint32_t Id = 0x800000;
} /* SurgeProtection */
namespace ResidualCurrent {
inline constexpr uint32_t Id = 0x1000000;
} /* ResidualCurrent */
namespace ArcFault {
inline constexpr uint32_t Id = 0x2000000;
} /* ArcFault */
namespace SelfTest {
inline constexpr uint32_t Id = 0x4000000;
} /* SelfTest */
} /* feature */

namespace attribute {
namespace Mask {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint32_t Min = 0;
inline constexpr uint32_t Max = 4294967295;
} /* Mask */
namespace State {
inline constexpr uint32_t Id = 0x0002;
inline constexpr uint32_t Min = 0;
inline constexpr uint32_t Max = 4294967295;
} /* State */
namespace Supported {
inline constexpr uint32_t Id = 0x0003;
inline constexpr uint32_t Min = 0;
inline constexpr uint32_t Max = 4294967295;
} /* Supported */
namespace ArcCause {
inline constexpr uint32_t Id = 0x0080;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 7;
} /* ArcCause */
namespace OverLoadRating {
inline constexpr uint32_t Id = 0x0081;
} /* OverLoadRating */
namespace OverVoltageRating {
inline constexpr uint32_t Id = 0x0082;
} /* OverVoltageRating */
namespace SurgeProtectionRating {
inline constexpr uint32_t Id = 0x0083;
} /* SurgeProtectionRating */
namespace ShortCircuitRating {
inline constexpr uint32_t Id = 0x0084;
} /* ShortCircuitRating */
namespace ResidualCurrentRating {
inline constexpr uint32_t Id = 0x0085;
} /* ResidualCurrentRating */
namespace ArcFaultRating {
inline constexpr uint32_t Id = 0x0086;
} /* ArcFaultRating */
} /* attribute */

namespace command {
namespace ModifyEnabledAlarms {
inline constexpr uint32_t Id = 0x01;
} /* ModifyEnabledAlarms */
} /* command */

namespace event {
namespace Notify {
inline constexpr uint32_t Id = 0x00;
} /* Notify */
} /* event */

} /* electrical_protection_alarm */
} /* cluster */
} /* esp_matter */
