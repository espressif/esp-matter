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
namespace time_format_localization {

inline constexpr uint32_t Id = 0x002C;

namespace feature {
namespace CalendarFormat {
inline constexpr uint32_t Id = 0x1;
} /* CalendarFormat */
} /* feature */

namespace attribute {
namespace HourFormat {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* HourFormat */
namespace ActiveCalendarType {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 12;
} /* ActiveCalendarType */
namespace SupportedCalendarTypes {
inline constexpr uint32_t Id = 0x0002;
} /* SupportedCalendarTypes */
} /* attribute */

} /* time_format_localization */
} /* cluster */
} /* esp_matter */
