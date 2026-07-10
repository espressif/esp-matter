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
namespace commissioner_control {

inline constexpr uint32_t Id = 0x0751;

namespace attribute {
namespace SupportedDeviceCategories {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint32_t Min = 0;
inline constexpr uint32_t Max = 4294967295;
} /* SupportedDeviceCategories */
} /* attribute */

namespace command {
namespace RequestCommissioningApproval {
inline constexpr uint32_t Id = 0x00;
} /* RequestCommissioningApproval */
namespace CommissionNode {
inline constexpr uint32_t Id = 0x01;
} /* CommissionNode */
namespace ReverseOpenCommissioningWindow {
inline constexpr uint32_t Id = 0x02;
} /* ReverseOpenCommissioningWindow */
} /* command */

namespace event {
namespace CommissioningRequestResult {
inline constexpr uint32_t Id = 0x00;
} /* CommissioningRequestResult */
} /* event */

} /* commissioner_control */
} /* cluster */
} /* esp_matter */
