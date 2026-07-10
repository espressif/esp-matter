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
namespace device_energy_management {

inline constexpr uint32_t Id = 0x0098;

namespace feature {
namespace PowerAdjustment {
inline constexpr uint32_t Id = 0x1;
} /* PowerAdjustment */
namespace PowerForecastReporting {
inline constexpr uint32_t Id = 0x2;
} /* PowerForecastReporting */
namespace StateForecastReporting {
inline constexpr uint32_t Id = 0x4;
} /* StateForecastReporting */
namespace StartTimeAdjustment {
inline constexpr uint32_t Id = 0x8;
} /* StartTimeAdjustment */
namespace Pausable {
inline constexpr uint32_t Id = 0x10;
} /* Pausable */
namespace ForecastAdjustment {
inline constexpr uint32_t Id = 0x20;
} /* ForecastAdjustment */
namespace ConstraintBasedAdjustment {
inline constexpr uint32_t Id = 0x40;
} /* ConstraintBasedAdjustment */
} /* feature */

namespace attribute {
namespace ESAType {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 14;
} /* ESAType */
namespace ESACanGenerate {
inline constexpr uint32_t Id = 0x0001;
} /* ESACanGenerate */
namespace ESAState {
inline constexpr uint32_t Id = 0x0002;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 4;
} /* ESAState */
namespace AbsMinPower {
inline constexpr uint32_t Id = 0x0003;
} /* AbsMinPower */
namespace AbsMaxPower {
inline constexpr uint32_t Id = 0x0004;
} /* AbsMaxPower */
namespace PowerAdjustmentCapability {
inline constexpr uint32_t Id = 0x0005;
} /* PowerAdjustmentCapability */
namespace Forecast {
inline constexpr uint32_t Id = 0x0006;
} /* Forecast */
namespace OptOutState {
inline constexpr uint32_t Id = 0x0007;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* OptOutState */
} /* attribute */

namespace command {
namespace PowerAdjustRequest {
inline constexpr uint32_t Id = 0x00;
} /* PowerAdjustRequest */
namespace CancelPowerAdjustRequest {
inline constexpr uint32_t Id = 0x01;
} /* CancelPowerAdjustRequest */
namespace StartTimeAdjustRequest {
inline constexpr uint32_t Id = 0x02;
} /* StartTimeAdjustRequest */
namespace PauseRequest {
inline constexpr uint32_t Id = 0x03;
} /* PauseRequest */
namespace ResumeRequest {
inline constexpr uint32_t Id = 0x04;
} /* ResumeRequest */
namespace ModifyForecastRequest {
inline constexpr uint32_t Id = 0x05;
} /* ModifyForecastRequest */
namespace RequestConstraintBasedForecast {
inline constexpr uint32_t Id = 0x06;
} /* RequestConstraintBasedForecast */
namespace CancelRequest {
inline constexpr uint32_t Id = 0x07;
} /* CancelRequest */
} /* command */

namespace event {
namespace PowerAdjustStart {
inline constexpr uint32_t Id = 0x00;
} /* PowerAdjustStart */
namespace PowerAdjustEnd {
inline constexpr uint32_t Id = 0x01;
} /* PowerAdjustEnd */
namespace Paused {
inline constexpr uint32_t Id = 0x02;
} /* Paused */
namespace Resumed {
inline constexpr uint32_t Id = 0x03;
} /* Resumed */
} /* event */

} /* device_energy_management */
} /* cluster */
} /* esp_matter */
