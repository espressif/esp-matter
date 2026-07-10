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
namespace closure_control {

inline constexpr uint32_t Id = 0x0104;

namespace feature {
namespace Positioning {
inline constexpr uint32_t Id = 0x1;
} /* Positioning */
namespace MotionLatching {
inline constexpr uint32_t Id = 0x2;
} /* MotionLatching */
namespace Instantaneous {
inline constexpr uint32_t Id = 0x4;
} /* Instantaneous */
namespace Speed {
inline constexpr uint32_t Id = 0x8;
} /* Speed */
namespace Ventilation {
inline constexpr uint32_t Id = 0x10;
} /* Ventilation */
namespace Pedestrian {
inline constexpr uint32_t Id = 0x20;
} /* Pedestrian */
namespace Calibration {
inline constexpr uint32_t Id = 0x40;
} /* Calibration */
namespace Protection {
inline constexpr uint32_t Id = 0x80;
} /* Protection */
namespace ManuallyOperable {
inline constexpr uint32_t Id = 0x100;
} /* ManuallyOperable */
} /* feature */

namespace attribute {
namespace CountdownTime {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint32_t Min = 0;
inline constexpr uint32_t Max = 259200;
} /* CountdownTime */
namespace MainState {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 7;
} /* MainState */
namespace CurrentErrorList {
inline constexpr uint32_t Id = 0x0002;
} /* CurrentErrorList */
namespace OverallCurrentState {
inline constexpr uint32_t Id = 0x0003;
} /* OverallCurrentState */
namespace OverallTargetState {
inline constexpr uint32_t Id = 0x0004;
} /* OverallTargetState */
namespace LatchControlModes {
inline constexpr uint32_t Id = 0x0005;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* LatchControlModes */
} /* attribute */

namespace command {
namespace Stop {
inline constexpr uint32_t Id = 0x00;
} /* Stop */
namespace MoveTo {
inline constexpr uint32_t Id = 0x01;
} /* MoveTo */
namespace Calibrate {
inline constexpr uint32_t Id = 0x02;
} /* Calibrate */
} /* command */

namespace event {
namespace OperationalError {
inline constexpr uint32_t Id = 0x00;
} /* OperationalError */
namespace MovementCompleted {
inline constexpr uint32_t Id = 0x01;
} /* MovementCompleted */
namespace EngageStateChanged {
inline constexpr uint32_t Id = 0x02;
} /* EngageStateChanged */
namespace SecureStateChanged {
inline constexpr uint32_t Id = 0x03;
} /* SecureStateChanged */
} /* event */

} /* closure_control */
} /* cluster */
} /* esp_matter */
