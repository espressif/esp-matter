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
namespace relative_humidity_measurement {

inline constexpr uint32_t Id = 0x0405;

namespace attribute {
namespace MeasuredValue {
inline constexpr uint32_t Id = 0x0000;
} /* MeasuredValue */
namespace MinMeasuredValue {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 9999;
} /* MinMeasuredValue */
namespace MaxMeasuredValue {
inline constexpr uint32_t Id = 0x0002;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 10000;
} /* MaxMeasuredValue */
namespace Tolerance {
inline constexpr uint32_t Id = 0x0003;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 2048;
} /* Tolerance */
} /* attribute */

} /* relative_humidity_measurement */
} /* cluster */
} /* esp_matter */
