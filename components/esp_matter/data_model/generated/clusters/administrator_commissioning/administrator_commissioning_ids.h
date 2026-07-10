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
namespace administrator_commissioning {

inline constexpr uint32_t Id = 0x003C;

namespace feature {
namespace Basic {
inline constexpr uint32_t Id = 0x1;
} /* Basic */
} /* feature */

namespace attribute {
namespace WindowStatus {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* WindowStatus */
namespace AdminFabricIndex {
inline constexpr uint32_t Id = 0x0001;
} /* AdminFabricIndex */
namespace AdminVendorId {
inline constexpr uint32_t Id = 0x0002;
} /* AdminVendorId */
} /* attribute */

namespace command {
namespace OpenCommissioningWindow {
inline constexpr uint32_t Id = 0x00;
} /* OpenCommissioningWindow */
namespace OpenBasicCommissioningWindow {
inline constexpr uint32_t Id = 0x01;
} /* OpenBasicCommissioningWindow */
namespace RevokeCommissioning {
inline constexpr uint32_t Id = 0x02;
} /* RevokeCommissioning */
} /* command */

} /* administrator_commissioning */
} /* cluster */
} /* esp_matter */
