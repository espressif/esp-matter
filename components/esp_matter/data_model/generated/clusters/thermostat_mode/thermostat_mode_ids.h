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
namespace thermostat_mode {

inline constexpr uint32_t Id = 0x0063;

namespace feature {
namespace CoreModes {
inline constexpr uint32_t Id = 0x2;
} /* CoreModes */
} /* feature */

namespace attribute {
namespace SupportedModes {
inline constexpr uint32_t Id = 0x0000;
} /* SupportedModes */
namespace CurrentMode {
inline constexpr uint32_t Id = 0x0001;
} /* CurrentMode */
namespace StartUpMode {
inline constexpr uint32_t Id = 0x0002;
} /* StartUpMode */
} /* attribute */

namespace command {
namespace ChangeToMode {
inline constexpr uint32_t Id = 0x00;
} /* ChangeToMode */
namespace ChangeToModeResponse {
inline constexpr uint32_t Id = 0x01;
} /* ChangeToModeResponse */
} /* command */

} /* thermostat_mode */
} /* cluster */
} /* esp_matter */
