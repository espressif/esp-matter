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
namespace camera_av_settings_user_level_management {

inline constexpr uint32_t Id = 0x0552;

namespace feature {
namespace DigitalPTZ {
inline constexpr uint32_t Id = 0x1;
} /* DigitalPTZ */
namespace MechanicalPan {
inline constexpr uint32_t Id = 0x2;
} /* MechanicalPan */
namespace MechanicalTilt {
inline constexpr uint32_t Id = 0x4;
} /* MechanicalTilt */
namespace MechanicalZoom {
inline constexpr uint32_t Id = 0x8;
} /* MechanicalZoom */
namespace MechanicalPresets {
inline constexpr uint32_t Id = 0x10;
} /* MechanicalPresets */
} /* feature */

namespace attribute {
namespace MPTZPosition {
inline constexpr uint32_t Id = 0x0000;
} /* MPTZPosition */
namespace MaxPresets {
inline constexpr uint32_t Id = 0x0001;
} /* MaxPresets */
namespace MPTZPresets {
inline constexpr uint32_t Id = 0x0002;
} /* MPTZPresets */
namespace DPTZStreams {
inline constexpr uint32_t Id = 0x0003;
} /* DPTZStreams */
namespace ZoomMax {
inline constexpr uint32_t Id = 0x0004;
inline constexpr uint8_t Min = 2;
inline constexpr uint8_t Max = 100;
} /* ZoomMax */
namespace TiltMin {
inline constexpr uint32_t Id = 0x0005;
inline constexpr int16_t Min = -180;
inline constexpr int16_t Max = 0;
} /* TiltMin */
namespace TiltMax {
inline constexpr uint32_t Id = 0x0006;
inline constexpr int16_t Min = 1;
inline constexpr int16_t Max = 180;
} /* TiltMax */
namespace PanMin {
inline constexpr uint32_t Id = 0x0007;
inline constexpr int16_t Min = -180;
inline constexpr int16_t Max = 0;
} /* PanMin */
namespace PanMax {
inline constexpr uint32_t Id = 0x0008;
inline constexpr int16_t Min = 1;
inline constexpr int16_t Max = 180;
} /* PanMax */
namespace MovementState {
inline constexpr uint32_t Id = 0x0009;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 1;
} /* MovementState */
} /* attribute */

namespace command {
namespace MPTZSetPosition {
inline constexpr uint32_t Id = 0x00;
} /* MPTZSetPosition */
namespace MPTZRelativeMove {
inline constexpr uint32_t Id = 0x01;
} /* MPTZRelativeMove */
namespace MPTZMoveToPreset {
inline constexpr uint32_t Id = 0x02;
} /* MPTZMoveToPreset */
namespace MPTZSavePreset {
inline constexpr uint32_t Id = 0x03;
} /* MPTZSavePreset */
namespace MPTZRemovePreset {
inline constexpr uint32_t Id = 0x04;
} /* MPTZRemovePreset */
namespace DPTZSetViewport {
inline constexpr uint32_t Id = 0x05;
} /* DPTZSetViewport */
namespace DPTZRelativeMove {
inline constexpr uint32_t Id = 0x06;
} /* DPTZRelativeMove */
} /* command */

} /* camera_av_settings_user_level_management */
} /* cluster */
} /* esp_matter */
