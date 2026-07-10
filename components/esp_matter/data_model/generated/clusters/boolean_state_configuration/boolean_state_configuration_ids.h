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
namespace boolean_state_configuration {

inline constexpr uint32_t Id = 0x0080;

namespace feature {
namespace Visual {
inline constexpr uint32_t Id = 0x1;
} /* Visual */
namespace Audible {
inline constexpr uint32_t Id = 0x2;
} /* Audible */
namespace AlarmSuppress {
inline constexpr uint32_t Id = 0x4;
} /* AlarmSuppress */
namespace SensitivityLevel {
inline constexpr uint32_t Id = 0x8;
} /* SensitivityLevel */
namespace FaultEvents {
inline constexpr uint32_t Id = 0x10;
} /* FaultEvents */
} /* feature */

namespace attribute {
namespace CurrentSensitivityLevel {
inline constexpr uint32_t Id = 0x0000;
} /* CurrentSensitivityLevel */
namespace SupportedSensitivityLevels {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint8_t Min = 2;
inline constexpr uint8_t Max = 10;
} /* SupportedSensitivityLevels */
namespace DefaultSensitivityLevel {
inline constexpr uint32_t Id = 0x0002;
} /* DefaultSensitivityLevel */
namespace AlarmsActive {
inline constexpr uint32_t Id = 0x0003;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* AlarmsActive */
namespace AlarmsSuppressed {
inline constexpr uint32_t Id = 0x0004;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* AlarmsSuppressed */
namespace AlarmsEnabled {
inline constexpr uint32_t Id = 0x0005;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* AlarmsEnabled */
namespace AlarmsSupported {
inline constexpr uint32_t Id = 0x0006;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* AlarmsSupported */
namespace SensorFault {
inline constexpr uint32_t Id = 0x0007;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 1;
} /* SensorFault */
} /* attribute */

namespace command {
namespace SuppressAlarm {
inline constexpr uint32_t Id = 0x00;
} /* SuppressAlarm */
namespace EnableDisableAlarm {
inline constexpr uint32_t Id = 0x01;
} /* EnableDisableAlarm */
} /* command */

namespace event {
namespace AlarmsStateChanged {
inline constexpr uint32_t Id = 0x00;
} /* AlarmsStateChanged */
namespace SensorFault {
inline constexpr uint32_t Id = 0x01;
} /* SensorFault */
} /* event */

} /* boolean_state_configuration */
} /* cluster */
} /* esp_matter */
