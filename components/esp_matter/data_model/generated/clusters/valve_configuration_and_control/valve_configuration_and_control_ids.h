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
namespace valve_configuration_and_control {

inline constexpr uint32_t Id = 0x0081;

namespace feature {
namespace TimeSync {
inline constexpr uint32_t Id = 0x1;
} /* TimeSync */
namespace Level {
inline constexpr uint32_t Id = 0x2;
} /* Level */
} /* feature */

namespace attribute {
namespace OpenDuration {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint32_t Min = 1;
inline constexpr uint32_t Max = 4294967295;
} /* OpenDuration */
namespace DefaultOpenDuration {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint32_t Min = 1;
inline constexpr uint32_t Max = 4294967295;
} /* DefaultOpenDuration */
namespace AutoCloseTime {
inline constexpr uint32_t Id = 0x0002;
} /* AutoCloseTime */
namespace RemainingDuration {
inline constexpr uint32_t Id = 0x0003;
} /* RemainingDuration */
namespace CurrentState {
inline constexpr uint32_t Id = 0x0004;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* CurrentState */
namespace TargetState {
inline constexpr uint32_t Id = 0x0005;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* TargetState */
namespace CurrentLevel {
inline constexpr uint32_t Id = 0x0006;
} /* CurrentLevel */
namespace TargetLevel {
inline constexpr uint32_t Id = 0x0007;
} /* TargetLevel */
namespace DefaultOpenLevel {
inline constexpr uint32_t Id = 0x0008;
inline constexpr uint8_t Min = 1;
inline constexpr uint8_t Max = 100;
} /* DefaultOpenLevel */
namespace ValveFault {
inline constexpr uint32_t Id = 0x0009;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65535;
} /* ValveFault */
namespace LevelStep {
inline constexpr uint32_t Id = 0x000A;
inline constexpr uint8_t Min = 1;
inline constexpr uint8_t Max = 50;
} /* LevelStep */
} /* attribute */

namespace command {
namespace Open {
inline constexpr uint32_t Id = 0x00;
} /* Open */
namespace Close {
inline constexpr uint32_t Id = 0x01;
} /* Close */
} /* command */

namespace event {
namespace ValveStateChanged {
inline constexpr uint32_t Id = 0x00;
} /* ValveStateChanged */
namespace ValveFault {
inline constexpr uint32_t Id = 0x01;
} /* ValveFault */
} /* event */

} /* valve_configuration_and_control */
} /* cluster */
} /* esp_matter */
