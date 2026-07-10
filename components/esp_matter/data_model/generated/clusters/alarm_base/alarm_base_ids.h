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
namespace alarm_base {

inline constexpr uint32_t Id = 0xffff;

namespace feature {
namespace Reset {
inline constexpr uint32_t Id = 0x1;
} /* Reset */
} /* feature */

namespace attribute {
namespace Mask {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint32_t Min = 0;
inline constexpr uint32_t Max = 4294967295;
} /* Mask */
namespace Latch {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint32_t Min = 0;
inline constexpr uint32_t Max = 4294967295;
} /* Latch */
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
} /* attribute */

namespace command {
namespace Reset {
inline constexpr uint32_t Id = 0x00;
} /* Reset */
namespace ModifyEnabledAlarms {
inline constexpr uint32_t Id = 0x01;
} /* ModifyEnabledAlarms */
} /* command */

namespace event {
namespace Notify {
inline constexpr uint32_t Id = 0x00;
} /* Notify */
} /* event */

} /* alarm_base */
} /* cluster */
} /* esp_matter */
