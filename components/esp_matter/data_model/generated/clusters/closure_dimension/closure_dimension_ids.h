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
namespace closure_dimension {

inline constexpr uint32_t Id = 0x0105;

namespace feature {
namespace Positioning {
inline constexpr uint32_t Id = 0x1;
} /* Positioning */
namespace MotionLatching {
inline constexpr uint32_t Id = 0x2;
} /* MotionLatching */
namespace Unit {
inline constexpr uint32_t Id = 0x4;
} /* Unit */
namespace Limitation {
inline constexpr uint32_t Id = 0x8;
} /* Limitation */
namespace Speed {
inline constexpr uint32_t Id = 0x10;
} /* Speed */
namespace Translation {
inline constexpr uint32_t Id = 0x20;
} /* Translation */
namespace Rotation {
inline constexpr uint32_t Id = 0x40;
} /* Rotation */
namespace Modulation {
inline constexpr uint32_t Id = 0x80;
} /* Modulation */
} /* feature */

namespace attribute {
namespace CurrentState {
inline constexpr uint32_t Id = 0x0000;
} /* CurrentState */
namespace TargetState {
inline constexpr uint32_t Id = 0x0001;
} /* TargetState */
namespace Resolution {
inline constexpr uint32_t Id = 0x0002;
} /* Resolution */
namespace StepValue {
inline constexpr uint32_t Id = 0x0003;
} /* StepValue */
namespace Unit {
inline constexpr uint32_t Id = 0x0004;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 1;
} /* Unit */
namespace UnitRange {
inline constexpr uint32_t Id = 0x0005;
} /* UnitRange */
namespace LimitRange {
inline constexpr uint32_t Id = 0x0006;
} /* LimitRange */
namespace TranslationDirection {
inline constexpr uint32_t Id = 0x0007;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 11;
} /* TranslationDirection */
namespace RotationAxis {
inline constexpr uint32_t Id = 0x0008;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 10;
} /* RotationAxis */
namespace Overflow {
inline constexpr uint32_t Id = 0x0009;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 10;
} /* Overflow */
namespace ModulationType {
inline constexpr uint32_t Id = 0x000A;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 4;
} /* ModulationType */
namespace LatchControlModes {
inline constexpr uint32_t Id = 0x000B;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* LatchControlModes */
} /* attribute */

namespace command {
namespace SetTarget {
inline constexpr uint32_t Id = 0x00;
} /* SetTarget */
namespace Step {
inline constexpr uint32_t Id = 0x01;
} /* Step */
} /* command */

} /* closure_dimension */
} /* cluster */
} /* esp_matter */
