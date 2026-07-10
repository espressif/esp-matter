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
namespace level_control {

inline constexpr uint32_t Id = 0x0008;

namespace feature {
namespace OnOff {
inline constexpr uint32_t Id = 0x1;
} /* OnOff */
namespace Lighting {
inline constexpr uint32_t Id = 0x2;
} /* Lighting */
} /* feature */

namespace attribute {
namespace CurrentLevel {
inline constexpr uint32_t Id = 0x0000;
} /* CurrentLevel */
namespace RemainingTime {
inline constexpr uint32_t Id = 0x0001;
} /* RemainingTime */
namespace MinLevel {
inline constexpr uint32_t Id = 0x0002;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 254;
} /* MinLevel */
namespace MaxLevel {
inline constexpr uint32_t Id = 0x0003;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 254;
} /* MaxLevel */
namespace Options {
inline constexpr uint32_t Id = 0x000F;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* Options */
namespace OnOffTransitionTime {
inline constexpr uint32_t Id = 0x0010;
} /* OnOffTransitionTime */
namespace OnLevel {
inline constexpr uint32_t Id = 0x0011;
} /* OnLevel */
namespace OnTransitionTime {
inline constexpr uint32_t Id = 0x0012;
} /* OnTransitionTime */
namespace OffTransitionTime {
inline constexpr uint32_t Id = 0x0013;
} /* OffTransitionTime */
namespace DefaultMoveRate {
inline constexpr uint32_t Id = 0x0014;
inline constexpr uint8_t Min = 1;
inline constexpr uint8_t Max = 255;
} /* DefaultMoveRate */
namespace StartUpCurrentLevel {
inline constexpr uint32_t Id = 0x4000;
} /* StartUpCurrentLevel */
} /* attribute */

namespace command {
namespace MoveToLevel {
inline constexpr uint32_t Id = 0x00;
} /* MoveToLevel */
namespace Move {
inline constexpr uint32_t Id = 0x01;
} /* Move */
namespace Step {
inline constexpr uint32_t Id = 0x02;
} /* Step */
namespace Stop {
inline constexpr uint32_t Id = 0x03;
} /* Stop */
namespace MoveToLevelWithOnOff {
inline constexpr uint32_t Id = 0x04;
} /* MoveToLevelWithOnOff */
namespace MoveWithOnOff {
inline constexpr uint32_t Id = 0x05;
} /* MoveWithOnOff */
namespace StepWithOnOff {
inline constexpr uint32_t Id = 0x06;
} /* StepWithOnOff */
namespace StopWithOnOff {
inline constexpr uint32_t Id = 0x07;
} /* StopWithOnOff */
} /* command */

} /* level_control */
} /* cluster */
} /* esp_matter */
