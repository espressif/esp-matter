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
namespace switch_cluster {

inline constexpr uint32_t Id = 0x003B;

namespace feature {
namespace LatchingSwitch {
inline constexpr uint32_t Id = 0x1;
} /* LatchingSwitch */
namespace MomentarySwitch {
inline constexpr uint32_t Id = 0x2;
} /* MomentarySwitch */
namespace MomentarySwitchRelease {
inline constexpr uint32_t Id = 0x4;
} /* MomentarySwitchRelease */
namespace MomentarySwitchLongPress {
inline constexpr uint32_t Id = 0x8;
} /* MomentarySwitchLongPress */
namespace MomentarySwitchMultiPress {
inline constexpr uint32_t Id = 0x10;
} /* MomentarySwitchMultiPress */
namespace ActionSwitch {
inline constexpr uint32_t Id = 0x20;
} /* ActionSwitch */
} /* feature */

namespace attribute {
namespace NumberOfPositions {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint8_t Min = 2;
inline constexpr uint8_t Max = 255;
} /* NumberOfPositions */
namespace CurrentPosition {
inline constexpr uint32_t Id = 0x0001;
} /* CurrentPosition */
namespace MultiPressMax {
inline constexpr uint32_t Id = 0x0002;
inline constexpr uint8_t Min = 2;
inline constexpr uint8_t Max = 255;
} /* MultiPressMax */
} /* attribute */

namespace event {
namespace SwitchLatched {
inline constexpr uint32_t Id = 0x00;
} /* SwitchLatched */
namespace InitialPress {
inline constexpr uint32_t Id = 0x01;
} /* InitialPress */
namespace LongPress {
inline constexpr uint32_t Id = 0x02;
} /* LongPress */
namespace ShortRelease {
inline constexpr uint32_t Id = 0x03;
} /* ShortRelease */
namespace LongRelease {
inline constexpr uint32_t Id = 0x04;
} /* LongRelease */
namespace MultiPressOngoing {
inline constexpr uint32_t Id = 0x05;
} /* MultiPressOngoing */
namespace MultiPressComplete {
inline constexpr uint32_t Id = 0x06;
} /* MultiPressComplete */
} /* event */

} /* switch_cluster */
} /* cluster */
} /* esp_matter */
