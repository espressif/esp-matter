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
namespace electrical_alarm {

inline constexpr uint32_t Id = 0x00A1;

namespace feature {
namespace Reset {
inline constexpr uint32_t Id = 0x1;
} /* Reset */
namespace AdjustableThresholds {
inline constexpr uint32_t Id = 0x100000;
} /* AdjustableThresholds */
namespace OverVoltage {
inline constexpr uint32_t Id = 0x200000;
} /* OverVoltage */
namespace UnderVoltage {
inline constexpr uint32_t Id = 0x400000;
} /* UnderVoltage */
namespace OverFrequency {
inline constexpr uint32_t Id = 0x800000;
} /* OverFrequency */
namespace UnderFrequency {
inline constexpr uint32_t Id = 0x1000000;
} /* UnderFrequency */
namespace OverPower {
inline constexpr uint32_t Id = 0x2000000;
} /* OverPower */
namespace UnderPower {
inline constexpr uint32_t Id = 0x4000000;
} /* UnderPower */
namespace OverCurrent {
inline constexpr uint32_t Id = 0x8000000;
} /* OverCurrent */
namespace UnderCurrent {
inline constexpr uint32_t Id = 0x10000000;
} /* UnderCurrent */
namespace PowerImport {
inline constexpr uint32_t Id = 0x20000000;
} /* PowerImport */
namespace PowerExport {
inline constexpr uint32_t Id = 0x40000000;
} /* PowerExport */
} /* feature */

namespace attribute {
namespace Mask {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint32_t Min = 0;
inline constexpr uint32_t Max = 4294967295;
} /* Mask */
namespace Latch {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint32_t Min = 0;
inline constexpr uint32_t Max = 4294967295;
} /* Latch */
namespace State {
inline constexpr uint32_t Id = 0x0002;
inline constexpr uint32_t Min = 0;
inline constexpr uint32_t Max = 4294967295;
} /* State */
namespace Supported {
inline constexpr uint32_t Id = 0x0003;
inline constexpr uint32_t Min = 0;
inline constexpr uint32_t Max = 4294967295;
} /* Supported */
namespace OverVoltageThreshold {
inline constexpr uint32_t Id = 0x0080;
} /* OverVoltageThreshold */
namespace UnderVoltageThreshold {
inline constexpr uint32_t Id = 0x0081;
} /* UnderVoltageThreshold */
namespace OverFrequencyThreshold {
inline constexpr uint32_t Id = 0x0082;
} /* OverFrequencyThreshold */
namespace UnderFrequencyThreshold {
inline constexpr uint32_t Id = 0x0083;
} /* UnderFrequencyThreshold */
namespace OverPowerThreshold {
inline constexpr uint32_t Id = 0x0084;
} /* OverPowerThreshold */
namespace UnderPowerThreshold {
inline constexpr uint32_t Id = 0x0085;
} /* UnderPowerThreshold */
namespace OverCurrentThreshold {
inline constexpr uint32_t Id = 0x0086;
} /* OverCurrentThreshold */
namespace UnderCurrentThreshold {
inline constexpr uint32_t Id = 0x0087;
} /* UnderCurrentThreshold */
namespace PowerImportThreshold {
inline constexpr uint32_t Id = 0x0088;
} /* PowerImportThreshold */
namespace PowerExportThreshold {
inline constexpr uint32_t Id = 0x0089;
} /* PowerExportThreshold */
} /* attribute */

namespace command {
namespace Reset {
inline constexpr uint32_t Id = 0x00;
} /* Reset */
namespace ModifyEnabledAlarms {
inline constexpr uint32_t Id = 0x01;
} /* ModifyEnabledAlarms */
namespace SetElectricalAlarmThresholds {
inline constexpr uint32_t Id = 0x80;
} /* SetElectricalAlarmThresholds */
} /* command */

namespace event {
namespace Notify {
inline constexpr uint32_t Id = 0x00;
} /* Notify */
} /* event */

} /* electrical_alarm */
} /* cluster */
} /* esp_matter */
