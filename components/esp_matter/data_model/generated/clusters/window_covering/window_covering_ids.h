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
namespace window_covering {

inline constexpr uint32_t Id = 0x0102;

namespace feature {
namespace Lift {
inline constexpr uint32_t Id = 0x1;
} /* Lift */
namespace Tilt {
inline constexpr uint32_t Id = 0x2;
} /* Tilt */
namespace PositionAwareLift {
inline constexpr uint32_t Id = 0x4;
} /* PositionAwareLift */
namespace PositionAwareTilt {
inline constexpr uint32_t Id = 0x10;
} /* PositionAwareTilt */
} /* feature */

namespace attribute {
namespace Type {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 10;
} /* Type */
namespace NumberOfActuationsLift {
inline constexpr uint32_t Id = 0x0005;
} /* NumberOfActuationsLift */
namespace NumberOfActuationsTilt {
inline constexpr uint32_t Id = 0x0006;
} /* NumberOfActuationsTilt */
namespace ConfigStatus {
inline constexpr uint32_t Id = 0x0007;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 127;
} /* ConfigStatus */
namespace CurrentPositionLiftPercentage {
inline constexpr uint32_t Id = 0x0008;
} /* CurrentPositionLiftPercentage */
namespace CurrentPositionTiltPercentage {
inline constexpr uint32_t Id = 0x0009;
} /* CurrentPositionTiltPercentage */
namespace OperationalStatus {
inline constexpr uint32_t Id = 0x000A;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 7;
} /* OperationalStatus */
namespace TargetPositionLiftPercent100ths {
inline constexpr uint32_t Id = 0x000B;
} /* TargetPositionLiftPercent100ths */
namespace TargetPositionTiltPercent100ths {
inline constexpr uint32_t Id = 0x000C;
} /* TargetPositionTiltPercent100ths */
namespace EndProductType {
inline constexpr uint32_t Id = 0x000D;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 24;
} /* EndProductType */
namespace CurrentPositionLiftPercent100ths {
inline constexpr uint32_t Id = 0x000E;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 10000;
} /* CurrentPositionLiftPercent100ths */
namespace CurrentPositionTiltPercent100ths {
inline constexpr uint32_t Id = 0x000F;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 10000;
} /* CurrentPositionTiltPercent100ths */
namespace Mode {
inline constexpr uint32_t Id = 0x0017;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 15;
} /* Mode */
namespace SafetyStatus {
inline constexpr uint32_t Id = 0x001A;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 4095;
} /* SafetyStatus */
} /* attribute */

namespace command {
namespace UpOrOpen {
inline constexpr uint32_t Id = 0x00;
} /* UpOrOpen */
namespace DownOrClose {
inline constexpr uint32_t Id = 0x01;
} /* DownOrClose */
namespace StopMotion {
inline constexpr uint32_t Id = 0x02;
} /* StopMotion */
namespace GoToLiftPercentage {
inline constexpr uint32_t Id = 0x05;
} /* GoToLiftPercentage */
namespace GoToTiltPercentage {
inline constexpr uint32_t Id = 0x08;
} /* GoToTiltPercentage */
} /* command */

} /* window_covering */
} /* cluster */
} /* esp_matter */
