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
namespace pm2_5_concentration_measurement {

inline constexpr uint32_t Id = 0x042A;

namespace feature {
namespace NumericMeasurement {
inline constexpr uint32_t Id = 0x1;
} /* NumericMeasurement */
namespace LevelIndication {
inline constexpr uint32_t Id = 0x2;
} /* LevelIndication */
namespace MediumLevel {
inline constexpr uint32_t Id = 0x4;
} /* MediumLevel */
namespace CriticalLevel {
inline constexpr uint32_t Id = 0x8;
} /* CriticalLevel */
namespace PeakMeasurement {
inline constexpr uint32_t Id = 0x10;
} /* PeakMeasurement */
namespace AverageMeasurement {
inline constexpr uint32_t Id = 0x20;
} /* AverageMeasurement */
} /* feature */

namespace attribute {
namespace MeasuredValue {
inline constexpr uint32_t Id = 0x0000;
} /* MeasuredValue */
namespace MinMeasuredValue {
inline constexpr uint32_t Id = 0x0001;
} /* MinMeasuredValue */
namespace MaxMeasuredValue {
inline constexpr uint32_t Id = 0x0002;
} /* MaxMeasuredValue */
namespace PeakMeasuredValue {
inline constexpr uint32_t Id = 0x0003;
} /* PeakMeasuredValue */
namespace PeakMeasuredValueWindow {
inline constexpr uint32_t Id = 0x0004;
inline constexpr uint32_t Min = 0;
inline constexpr uint32_t Max = 604800;
} /* PeakMeasuredValueWindow */
namespace AverageMeasuredValue {
inline constexpr uint32_t Id = 0x0005;
} /* AverageMeasuredValue */
namespace AverageMeasuredValueWindow {
inline constexpr uint32_t Id = 0x0006;
inline constexpr uint32_t Min = 0;
inline constexpr uint32_t Max = 604800;
} /* AverageMeasuredValueWindow */
namespace Uncertainty {
inline constexpr uint32_t Id = 0x0007;
} /* Uncertainty */
namespace MeasurementUnit {
inline constexpr uint32_t Id = 0x0008;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 7;
} /* MeasurementUnit */
namespace MeasurementMedium {
inline constexpr uint32_t Id = 0x0009;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* MeasurementMedium */
namespace LevelValue {
inline constexpr uint32_t Id = 0x000A;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 4;
} /* LevelValue */
} /* attribute */

} /* pm2_5_concentration_measurement */
} /* cluster */
} /* esp_matter */
