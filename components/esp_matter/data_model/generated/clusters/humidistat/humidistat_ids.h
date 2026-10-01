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
namespace humidistat {

inline constexpr uint32_t Id = 0x0205;

namespace feature {
namespace Humidifier {
inline constexpr uint32_t Id = 0x1;
} /* Humidifier */
namespace Dehumidifier {
inline constexpr uint32_t Id = 0x2;
} /* Dehumidifier */
namespace Continuous {
inline constexpr uint32_t Id = 0x4;
} /* Continuous */
namespace Sensor {
inline constexpr uint32_t Id = 0x8;
} /* Sensor */
namespace FanAuto {
inline constexpr uint32_t Id = 0x10;
} /* FanAuto */
namespace FanOnly {
inline constexpr uint32_t Id = 0x20;
} /* FanOnly */
namespace Optimal {
inline constexpr uint32_t Id = 0x40;
} /* Optimal */
namespace WarmMist {
inline constexpr uint32_t Id = 0x80;
} /* WarmMist */
namespace ColdMist {
inline constexpr uint32_t Id = 0x100;
} /* ColdMist */
namespace CondPump {
inline constexpr uint32_t Id = 0x200;
} /* CondPump */
} /* feature */

namespace attribute {
namespace SupportedModes {
inline constexpr uint32_t Id = 0x0000;
} /* SupportedModes */
namespace Mode {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* Mode */
namespace SystemState {
inline constexpr uint32_t Id = 0x0002;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* SystemState */
namespace UserSetpoint {
inline constexpr uint32_t Id = 0x0003;
} /* UserSetpoint */
namespace MinSetpoint {
inline constexpr uint32_t Id = 0x0004;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 99;
} /* MinSetpoint */
namespace MaxSetpoint {
inline constexpr uint32_t Id = 0x0005;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 100;
} /* MaxSetpoint */
namespace Step {
inline constexpr uint32_t Id = 0x0006;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 100;
} /* Step */
namespace TargetSetpoint {
inline constexpr uint32_t Id = 0x0007;
} /* TargetSetpoint */
namespace MistType {
inline constexpr uint32_t Id = 0x0008;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* MistType */
namespace Continuous {
inline constexpr uint32_t Id = 0x0009;
} /* Continuous */
namespace Sleep {
inline constexpr uint32_t Id = 0x000A;
} /* Sleep */
namespace Optimal {
inline constexpr uint32_t Id = 0x000B;
} /* Optimal */
namespace CondPumpEnabled {
inline constexpr uint32_t Id = 0x000C;
} /* CondPumpEnabled */
namespace CondRunCount {
inline constexpr uint32_t Id = 0x000D;
} /* CondRunCount */
} /* attribute */

namespace command {
namespace SetSettings {
inline constexpr uint32_t Id = 0x00;
} /* SetSettings */
} /* command */

} /* humidistat */
} /* cluster */
} /* esp_matter */
