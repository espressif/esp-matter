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
namespace temperature_control {

inline constexpr uint32_t Id = 0x0056;

namespace feature {
namespace TemperatureNumber {
inline constexpr uint32_t Id = 0x1;
} /* TemperatureNumber */
namespace TemperatureLevel {
inline constexpr uint32_t Id = 0x2;
} /* TemperatureLevel */
namespace TemperatureStep {
inline constexpr uint32_t Id = 0x4;
} /* TemperatureStep */
} /* feature */

namespace attribute {
namespace TemperatureSetpoint {
inline constexpr uint32_t Id = 0x0000;
} /* TemperatureSetpoint */
namespace MinTemperature {
inline constexpr uint32_t Id = 0x0001;
} /* MinTemperature */
namespace MaxTemperature {
inline constexpr uint32_t Id = 0x0002;
} /* MaxTemperature */
namespace Step {
inline constexpr uint32_t Id = 0x0003;
inline constexpr int16_t Min = 1;
inline constexpr int16_t Max = 32767;
} /* Step */
namespace SelectedTemperatureLevel {
inline constexpr uint32_t Id = 0x0004;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 31;
} /* SelectedTemperatureLevel */
namespace SupportedTemperatureLevels {
inline constexpr uint32_t Id = 0x0005;
} /* SupportedTemperatureLevels */
} /* attribute */

namespace command {
namespace SetTemperature {
inline constexpr uint32_t Id = 0x00;
} /* SetTemperature */
} /* command */

} /* temperature_control */
} /* cluster */
} /* esp_matter */
