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
namespace occupancy_sensing {

inline constexpr uint32_t Id = 0x0406;

namespace feature {
namespace Other {
inline constexpr uint32_t Id = 0x1;
} /* Other */
namespace PassiveInfrared {
inline constexpr uint32_t Id = 0x2;
} /* PassiveInfrared */
namespace Ultrasonic {
inline constexpr uint32_t Id = 0x4;
} /* Ultrasonic */
namespace PhysicalContact {
inline constexpr uint32_t Id = 0x8;
} /* PhysicalContact */
namespace ActiveInfrared {
inline constexpr uint32_t Id = 0x10;
} /* ActiveInfrared */
namespace Radar {
inline constexpr uint32_t Id = 0x20;
} /* Radar */
namespace RFSensing {
inline constexpr uint32_t Id = 0x40;
} /* RFSensing */
namespace Vision {
inline constexpr uint32_t Id = 0x80;
} /* Vision */
namespace OccupancyEvent {
inline constexpr uint32_t Id = 0x200;
} /* OccupancyEvent */
} /* feature */

namespace attribute {
namespace Occupancy {
inline constexpr uint32_t Id = 0x0000;
} /* Occupancy */
namespace OccupancySensorType {
inline constexpr uint32_t Id = 0x0001;
} /* OccupancySensorType */
namespace OccupancySensorTypeBitmap {
inline constexpr uint32_t Id = 0x0002;
} /* OccupancySensorTypeBitmap */
namespace HoldTime {
inline constexpr uint32_t Id = 0x0003;
} /* HoldTime */
namespace HoldTimeLimits {
inline constexpr uint32_t Id = 0x0004;
} /* HoldTimeLimits */
namespace PIROccupiedToUnoccupiedDelay {
inline constexpr uint32_t Id = 0x0010;
} /* PIROccupiedToUnoccupiedDelay */
namespace PIRUnoccupiedToOccupiedDelay {
inline constexpr uint32_t Id = 0x0011;
} /* PIRUnoccupiedToOccupiedDelay */
namespace PIRUnoccupiedToOccupiedThreshold {
inline constexpr uint32_t Id = 0x0012;
} /* PIRUnoccupiedToOccupiedThreshold */
namespace UltrasonicOccupiedToUnoccupiedDelay {
inline constexpr uint32_t Id = 0x0020;
} /* UltrasonicOccupiedToUnoccupiedDelay */
namespace UltrasonicUnoccupiedToOccupiedDelay {
inline constexpr uint32_t Id = 0x0021;
} /* UltrasonicUnoccupiedToOccupiedDelay */
namespace UltrasonicUnoccupiedToOccupiedThreshold {
inline constexpr uint32_t Id = 0x0022;
} /* UltrasonicUnoccupiedToOccupiedThreshold */
namespace PhysicalContactOccupiedToUnoccupiedDelay {
inline constexpr uint32_t Id = 0x0030;
} /* PhysicalContactOccupiedToUnoccupiedDelay */
namespace PhysicalContactUnoccupiedToOccupiedDelay {
inline constexpr uint32_t Id = 0x0031;
} /* PhysicalContactUnoccupiedToOccupiedDelay */
namespace PhysicalContactUnoccupiedToOccupiedThreshold {
inline constexpr uint32_t Id = 0x0032;
} /* PhysicalContactUnoccupiedToOccupiedThreshold */
} /* attribute */

namespace event {
namespace OccupancyChanged {
inline constexpr uint32_t Id = 0x00;
} /* OccupancyChanged */
} /* event */

} /* occupancy_sensing */
} /* cluster */
} /* esp_matter */
