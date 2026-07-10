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
namespace microwave_oven_control {

inline constexpr uint32_t Id = 0x005F;

namespace feature {
namespace PowerAsNumber {
inline constexpr uint32_t Id = 0x1;
} /* PowerAsNumber */
namespace PowerNumberLimits {
inline constexpr uint32_t Id = 0x4;
} /* PowerNumberLimits */
} /* feature */

namespace attribute {
namespace CookTime {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint32_t Min = 1;
inline constexpr uint32_t Max = 4294967295;
} /* CookTime */
namespace MaxCookTime {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint32_t Min = 1;
inline constexpr uint32_t Max = 86400;
} /* MaxCookTime */
namespace PowerSetting {
inline constexpr uint32_t Id = 0x0002;
} /* PowerSetting */
namespace MinPower {
inline constexpr uint32_t Id = 0x0003;
inline constexpr uint8_t Min = 1;
inline constexpr uint8_t Max = 99;
} /* MinPower */
namespace MaxPower {
inline constexpr uint32_t Id = 0x0004;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 100;
} /* MaxPower */
namespace PowerStep {
inline constexpr uint32_t Id = 0x0005;
} /* PowerStep */
namespace WattRating {
inline constexpr uint32_t Id = 0x0008;
} /* WattRating */
} /* attribute */

namespace command {
namespace SetCookingParameters {
inline constexpr uint32_t Id = 0x00;
} /* SetCookingParameters */
namespace AddMoreTime {
inline constexpr uint32_t Id = 0x01;
} /* AddMoreTime */
} /* command */

} /* microwave_oven_control */
} /* cluster */
} /* esp_matter */
