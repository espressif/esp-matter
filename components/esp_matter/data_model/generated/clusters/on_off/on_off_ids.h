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
namespace on_off {

inline constexpr uint32_t Id = 0x0006;

namespace feature {
namespace Lighting {
inline constexpr uint32_t Id = 0x1;
} /* Lighting */
namespace DeadFrontBehavior {
inline constexpr uint32_t Id = 0x2;
} /* DeadFrontBehavior */
namespace OffOnly {
inline constexpr uint32_t Id = 0x4;
} /* OffOnly */
} /* feature */

namespace attribute {
namespace OnOff {
inline constexpr uint32_t Id = 0x0000;
} /* OnOff */
namespace GlobalSceneControl {
inline constexpr uint32_t Id = 0x4000;
} /* GlobalSceneControl */
namespace OnTime {
inline constexpr uint32_t Id = 0x4001;
} /* OnTime */
namespace OffWaitTime {
inline constexpr uint32_t Id = 0x4002;
} /* OffWaitTime */
namespace StartUpOnOff {
inline constexpr uint32_t Id = 0x4003;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* StartUpOnOff */
} /* attribute */

namespace command {
namespace Off {
inline constexpr uint32_t Id = 0x00;
} /* Off */
namespace On {
inline constexpr uint32_t Id = 0x01;
} /* On */
namespace Toggle {
inline constexpr uint32_t Id = 0x02;
} /* Toggle */
namespace OffWithEffect {
inline constexpr uint32_t Id = 0x40;
} /* OffWithEffect */
namespace OnWithRecallGlobalScene {
inline constexpr uint32_t Id = 0x41;
} /* OnWithRecallGlobalScene */
namespace OnWithTimedOff {
inline constexpr uint32_t Id = 0x42;
} /* OnWithTimedOff */
} /* command */

} /* on_off */
} /* cluster */
} /* esp_matter */
