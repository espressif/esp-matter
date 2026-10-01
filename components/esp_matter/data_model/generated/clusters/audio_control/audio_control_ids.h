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
namespace audio_control {

inline constexpr uint32_t Id = 0x0512;

namespace feature {
namespace Decibel {
inline constexpr uint32_t Id = 0x1;
} /* Decibel */
namespace BasicEqualizer {
inline constexpr uint32_t Id = 0x2;
} /* BasicEqualizer */
} /* feature */

namespace attribute {
namespace SoftMuted {
inline constexpr uint32_t Id = 0x0000;
} /* SoftMuted */
namespace PhysicallyMuted {
inline constexpr uint32_t Id = 0x0001;
} /* PhysicallyMuted */
namespace Volume {
inline constexpr uint32_t Id = 0x0002;
} /* Volume */
namespace MinDeviceVolume {
inline constexpr uint32_t Id = 0x0003;
inline constexpr uint16_t Min = 1;
inline constexpr uint16_t Max = 65535;
} /* MinDeviceVolume */
namespace MaxDeviceVolume {
inline constexpr uint32_t Id = 0x0004;
} /* MaxDeviceVolume */
namespace MaxDeviceVolumeDB {
inline constexpr uint32_t Id = 0x0005;
} /* MaxDeviceVolumeDB */
namespace MaxUserVolume {
inline constexpr uint32_t Id = 0x0006;
} /* MaxUserVolume */
namespace DefaultStepSize {
inline constexpr uint32_t Id = 0x0007;
inline constexpr uint16_t Min = 1;
inline constexpr uint16_t Max = 65535;
} /* DefaultStepSize */
namespace SetVolumeUnmutePolicy {
inline constexpr uint32_t Id = 0x0008;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* SetVolumeUnmutePolicy */
namespace IncreaseVolumeUnmutePolicy {
inline constexpr uint32_t Id = 0x0009;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* IncreaseVolumeUnmutePolicy */
namespace IncreaseVolumeUnmuteVolume {
inline constexpr uint32_t Id = 0x000A;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* IncreaseVolumeUnmuteVolume */
namespace DecreaseVolumeUnmutePolicy {
inline constexpr uint32_t Id = 0x000B;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* DecreaseVolumeUnmutePolicy */
namespace StartUpMuted {
inline constexpr uint32_t Id = 0x000C;
} /* StartUpMuted */
namespace StartUpVolume {
inline constexpr uint32_t Id = 0x000D;
} /* StartUpVolume */
namespace Bass {
inline constexpr uint32_t Id = 0x000E;
} /* Bass */
namespace Mid {
inline constexpr uint32_t Id = 0x000F;
} /* Mid */
namespace Treble {
inline constexpr uint32_t Id = 0x0010;
} /* Treble */
namespace MinCorrection {
inline constexpr uint32_t Id = 0x0011;
inline constexpr int16_t Min = -32768;
inline constexpr int16_t Max = 0;
} /* MinCorrection */
namespace MaxCorrection {
inline constexpr uint32_t Id = 0x0012;
inline constexpr int16_t Min = 0;
inline constexpr int16_t Max = 32767;
} /* MaxCorrection */
} /* attribute */

namespace command {
namespace Mute {
inline constexpr uint32_t Id = 0x00;
} /* Mute */
namespace Unmute {
inline constexpr uint32_t Id = 0x01;
} /* Unmute */
namespace ToggleMuted {
inline constexpr uint32_t Id = 0x02;
} /* ToggleMuted */
namespace SetVolume {
inline constexpr uint32_t Id = 0x03;
} /* SetVolume */
namespace IncreaseVolume {
inline constexpr uint32_t Id = 0x04;
} /* IncreaseVolume */
namespace DecreaseVolume {
inline constexpr uint32_t Id = 0x05;
} /* DecreaseVolume */
} /* command */

} /* audio_control */
} /* cluster */
} /* esp_matter */
