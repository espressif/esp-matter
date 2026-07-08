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
namespace water_heater_management {

inline constexpr uint32_t Id = 0x0094;

namespace feature {
namespace EnergyManagement {
inline constexpr uint32_t Id = 0x1;
} /* EnergyManagement */
namespace TankPercent {
inline constexpr uint32_t Id = 0x2;
} /* TankPercent */
} /* feature */

namespace attribute {
namespace HeaterTypes {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 31;
} /* HeaterTypes */
namespace HeatDemand {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 31;
} /* HeatDemand */
namespace TankVolume {
inline constexpr uint32_t Id = 0x0002;
} /* TankVolume */
namespace EstimatedHeatRequired {
inline constexpr uint32_t Id = 0x0003;
inline constexpr int64_t Min = 0;
inline constexpr int64_t Max = 2147483647;
} /* EstimatedHeatRequired */
namespace TankPercentage {
inline constexpr uint32_t Id = 0x0004;
} /* TankPercentage */
namespace BoostState {
inline constexpr uint32_t Id = 0x0005;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 1;
} /* BoostState */
} /* attribute */

namespace command {
namespace Boost {
inline constexpr uint32_t Id = 0x00;
} /* Boost */
namespace CancelBoost {
inline constexpr uint32_t Id = 0x01;
} /* CancelBoost */
} /* command */

namespace event {
namespace BoostStarted {
inline constexpr uint32_t Id = 0x00;
} /* BoostStarted */
namespace BoostEnded {
inline constexpr uint32_t Id = 0x01;
} /* BoostEnded */
} /* event */

} /* water_heater_management */
} /* cluster */
} /* esp_matter */
