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
namespace electrical_power_measurement {

inline constexpr uint32_t Id = 0x0090;

namespace feature {
namespace DirectCurrent {
inline constexpr uint32_t Id = 0x1;
} /* DirectCurrent */
namespace AlternatingCurrent {
inline constexpr uint32_t Id = 0x2;
} /* AlternatingCurrent */
namespace PolyphasePower {
inline constexpr uint32_t Id = 0x4;
} /* PolyphasePower */
namespace Harmonics {
inline constexpr uint32_t Id = 0x8;
} /* Harmonics */
namespace PowerQuality {
inline constexpr uint32_t Id = 0x10;
} /* PowerQuality */
} /* feature */

namespace attribute {
namespace PowerMode {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* PowerMode */
namespace NumberOfMeasurementTypes {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 32;
} /* NumberOfMeasurementTypes */
namespace Accuracy {
inline constexpr uint32_t Id = 0x0002;
} /* Accuracy */
namespace Ranges {
inline constexpr uint32_t Id = 0x0003;
} /* Ranges */
namespace Voltage {
inline constexpr uint32_t Id = 0x0004;
inline constexpr int64_t Min = -4611686018427387904;
inline constexpr int64_t Max = 4611686018427387904;
} /* Voltage */
namespace ActiveCurrent {
inline constexpr uint32_t Id = 0x0005;
inline constexpr int64_t Min = -4611686018427387904;
inline constexpr int64_t Max = 4611686018427387904;
} /* ActiveCurrent */
namespace ReactiveCurrent {
inline constexpr uint32_t Id = 0x0006;
inline constexpr int64_t Min = -4611686018427387904;
inline constexpr int64_t Max = 4611686018427387904;
} /* ReactiveCurrent */
namespace ApparentCurrent {
inline constexpr uint32_t Id = 0x0007;
inline constexpr int64_t Min = -4611686018427387904;
inline constexpr int64_t Max = 4611686018427387904;
} /* ApparentCurrent */
namespace ActivePower {
inline constexpr uint32_t Id = 0x0008;
inline constexpr int64_t Min = -4611686018427387904;
inline constexpr int64_t Max = 4611686018427387904;
} /* ActivePower */
namespace ReactivePower {
inline constexpr uint32_t Id = 0x0009;
inline constexpr int64_t Min = -4611686018427387904;
inline constexpr int64_t Max = 4611686018427387904;
} /* ReactivePower */
namespace ApparentPower {
inline constexpr uint32_t Id = 0x000A;
inline constexpr int64_t Min = -4611686018427387904;
inline constexpr int64_t Max = 4611686018427387904;
} /* ApparentPower */
namespace RMSVoltage {
inline constexpr uint32_t Id = 0x000B;
inline constexpr int64_t Min = -4611686018427387904;
inline constexpr int64_t Max = 4611686018427387904;
} /* RMSVoltage */
namespace RMSCurrent {
inline constexpr uint32_t Id = 0x000C;
inline constexpr int64_t Min = -4611686018427387904;
inline constexpr int64_t Max = 4611686018427387904;
} /* RMSCurrent */
namespace RMSPower {
inline constexpr uint32_t Id = 0x000D;
inline constexpr int64_t Min = -4611686018427387904;
inline constexpr int64_t Max = 4611686018427387904;
} /* RMSPower */
namespace Frequency {
inline constexpr uint32_t Id = 0x000E;
inline constexpr int64_t Min = 0;
inline constexpr int64_t Max = 1000000;
} /* Frequency */
namespace HarmonicCurrents {
inline constexpr uint32_t Id = 0x000F;
} /* HarmonicCurrents */
namespace HarmonicPhases {
inline constexpr uint32_t Id = 0x0010;
} /* HarmonicPhases */
namespace PowerFactor {
inline constexpr uint32_t Id = 0x0011;
inline constexpr int64_t Min = -10000;
inline constexpr int64_t Max = 10000;
} /* PowerFactor */
namespace NeutralCurrent {
inline constexpr uint32_t Id = 0x0012;
inline constexpr int64_t Min = -4611686018427387904;
inline constexpr int64_t Max = 4611686018427387904;
} /* NeutralCurrent */
} /* attribute */

namespace event {
namespace MeasurementPeriodRanges {
inline constexpr uint32_t Id = 0x00;
} /* MeasurementPeriodRanges */
} /* event */

} /* electrical_power_measurement */
} /* cluster */
} /* esp_matter */
