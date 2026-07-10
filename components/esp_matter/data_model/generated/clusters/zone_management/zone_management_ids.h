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
namespace zone_management {

inline constexpr uint32_t Id = 0x0550;

namespace feature {
namespace TwoDimensionalCartesianZone {
inline constexpr uint32_t Id = 0x1;
} /* TwoDimensionalCartesianZone */
namespace PerZoneSensitivity {
inline constexpr uint32_t Id = 0x2;
} /* PerZoneSensitivity */
namespace UserDefined {
inline constexpr uint32_t Id = 0x4;
} /* UserDefined */
namespace FocusZones {
inline constexpr uint32_t Id = 0x8;
} /* FocusZones */
} /* feature */

namespace attribute {
namespace MaxUserDefinedZones {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint8_t Min = 5;
inline constexpr uint8_t Max = 255;
} /* MaxUserDefinedZones */
namespace MaxZones {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint8_t Min = 1;
inline constexpr uint8_t Max = 255;
} /* MaxZones */
namespace Zones {
inline constexpr uint32_t Id = 0x0002;
} /* Zones */
namespace Triggers {
inline constexpr uint32_t Id = 0x0003;
} /* Triggers */
namespace SensitivityMax {
inline constexpr uint32_t Id = 0x0004;
inline constexpr uint8_t Min = 2;
inline constexpr uint8_t Max = 10;
} /* SensitivityMax */
namespace Sensitivity {
inline constexpr uint32_t Id = 0x0005;
inline constexpr uint8_t Min = 1;
inline constexpr uint8_t Max = 255;
} /* Sensitivity */
namespace TwoDCartesianMax {
inline constexpr uint32_t Id = 0x0006;
} /* TwoDCartesianMax */
} /* attribute */

namespace command {
namespace CreateTwoDCartesianZone {
inline constexpr uint32_t Id = 0x00;
} /* CreateTwoDCartesianZone */
namespace CreateTwoDCartesianZoneResponse {
inline constexpr uint32_t Id = 0x01;
} /* CreateTwoDCartesianZoneResponse */
namespace UpdateTwoDCartesianZone {
inline constexpr uint32_t Id = 0x02;
} /* UpdateTwoDCartesianZone */
namespace RemoveZone {
inline constexpr uint32_t Id = 0x03;
} /* RemoveZone */
namespace CreateOrUpdateTrigger {
inline constexpr uint32_t Id = 0x04;
} /* CreateOrUpdateTrigger */
namespace RemoveTrigger {
inline constexpr uint32_t Id = 0x05;
} /* RemoveTrigger */
} /* command */

namespace event {
namespace ZoneTriggered {
inline constexpr uint32_t Id = 0x00;
} /* ZoneTriggered */
namespace ZoneStopped {
inline constexpr uint32_t Id = 0x01;
} /* ZoneStopped */
} /* event */

} /* zone_management */
} /* cluster */
} /* esp_matter */
