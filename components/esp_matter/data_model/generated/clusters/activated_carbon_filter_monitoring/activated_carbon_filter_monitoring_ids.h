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
namespace activated_carbon_filter_monitoring {

inline constexpr uint32_t Id = 0x0072;

namespace feature {
namespace Condition {
inline constexpr uint32_t Id = 0x1;
} /* Condition */
namespace Warning {
inline constexpr uint32_t Id = 0x2;
} /* Warning */
namespace ReplacementProductList {
inline constexpr uint32_t Id = 0x4;
} /* ReplacementProductList */
} /* feature */

namespace attribute {
namespace Condition {
inline constexpr uint32_t Id = 0x0000;
} /* Condition */
namespace DegradationDirection {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 1;
} /* DegradationDirection */
namespace ChangeIndication {
inline constexpr uint32_t Id = 0x0002;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* ChangeIndication */
namespace InPlaceIndicator {
inline constexpr uint32_t Id = 0x0003;
} /* InPlaceIndicator */
namespace LastChangedTime {
inline constexpr uint32_t Id = 0x0004;
} /* LastChangedTime */
namespace ReplacementProductList {
inline constexpr uint32_t Id = 0x0005;
} /* ReplacementProductList */
} /* attribute */

namespace command {
namespace ResetCondition {
inline constexpr uint32_t Id = 0x00;
} /* ResetCondition */
} /* command */

} /* activated_carbon_filter_monitoring */
} /* cluster */
} /* esp_matter */
