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
namespace fan_control {

inline constexpr uint32_t Id = 0x0202;

namespace feature {
namespace MultiSpeed {
inline constexpr uint32_t Id = 0x1;
} /* MultiSpeed */
namespace FanAuto {
inline constexpr uint32_t Id = 0x2;
} /* FanAuto */
namespace Rocking {
inline constexpr uint32_t Id = 0x4;
} /* Rocking */
namespace Wind {
inline constexpr uint32_t Id = 0x8;
} /* Wind */
namespace Step {
inline constexpr uint32_t Id = 0x10;
} /* Step */
namespace AirflowDirection {
inline constexpr uint32_t Id = 0x20;
} /* AirflowDirection */
} /* feature */

namespace attribute {
namespace FanMode {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 6;
} /* FanMode */
namespace FanModeSequence {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 5;
} /* FanModeSequence */
namespace PercentSetting {
inline constexpr uint32_t Id = 0x0002;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 100;
} /* PercentSetting */
namespace PercentCurrent {
inline constexpr uint32_t Id = 0x0003;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 100;
} /* PercentCurrent */
namespace SpeedMax {
inline constexpr uint32_t Id = 0x0004;
inline constexpr uint8_t Min = 1;
inline constexpr uint8_t Max = 100;
} /* SpeedMax */
namespace SpeedSetting {
inline constexpr uint32_t Id = 0x0005;
} /* SpeedSetting */
namespace SpeedCurrent {
inline constexpr uint32_t Id = 0x0006;
} /* SpeedCurrent */
namespace RockSupport {
inline constexpr uint32_t Id = 0x0007;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 7;
} /* RockSupport */
namespace RockSetting {
inline constexpr uint32_t Id = 0x0008;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 7;
} /* RockSetting */
namespace WindSupport {
inline constexpr uint32_t Id = 0x0009;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* WindSupport */
namespace WindSetting {
inline constexpr uint32_t Id = 0x000A;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* WindSetting */
namespace AirflowDirection {
inline constexpr uint32_t Id = 0x000B;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 1;
} /* AirflowDirection */
} /* attribute */

namespace command {
namespace Step {
inline constexpr uint32_t Id = 0x00;
} /* Step */
} /* command */

} /* fan_control */
} /* cluster */
} /* esp_matter */
