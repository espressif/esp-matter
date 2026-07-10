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
namespace smoke_co_alarm {

inline constexpr uint32_t Id = 0x005C;

namespace feature {
namespace SmokeAlarm {
inline constexpr uint32_t Id = 0x1;
} /* SmokeAlarm */
namespace COAlarm {
inline constexpr uint32_t Id = 0x2;
} /* COAlarm */
} /* feature */

namespace attribute {
namespace ExpressedState {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 9;
} /* ExpressedState */
namespace SmokeState {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* SmokeState */
namespace COState {
inline constexpr uint32_t Id = 0x0002;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* COState */
namespace BatteryAlert {
inline constexpr uint32_t Id = 0x0003;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* BatteryAlert */
namespace DeviceMuted {
inline constexpr uint32_t Id = 0x0004;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 1;
} /* DeviceMuted */
namespace TestInProgress {
inline constexpr uint32_t Id = 0x0005;
} /* TestInProgress */
namespace HardwareFaultAlert {
inline constexpr uint32_t Id = 0x0006;
} /* HardwareFaultAlert */
namespace EndOfServiceAlert {
inline constexpr uint32_t Id = 0x0007;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 1;
} /* EndOfServiceAlert */
namespace InterconnectSmokeAlarm {
inline constexpr uint32_t Id = 0x0008;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* InterconnectSmokeAlarm */
namespace InterconnectCOAlarm {
inline constexpr uint32_t Id = 0x0009;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* InterconnectCOAlarm */
namespace ContaminationState {
inline constexpr uint32_t Id = 0x000A;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* ContaminationState */
namespace SmokeSensitivityLevel {
inline constexpr uint32_t Id = 0x000B;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* SmokeSensitivityLevel */
namespace ExpiryDate {
inline constexpr uint32_t Id = 0x000C;
} /* ExpiryDate */
namespace Unmounted {
inline constexpr uint32_t Id = 0x000D;
} /* Unmounted */
} /* attribute */

namespace command {
namespace SelfTestRequest {
inline constexpr uint32_t Id = 0x00;
} /* SelfTestRequest */
} /* command */

namespace event {
namespace SmokeAlarm {
inline constexpr uint32_t Id = 0x00;
} /* SmokeAlarm */
namespace COAlarm {
inline constexpr uint32_t Id = 0x01;
} /* COAlarm */
namespace LowBattery {
inline constexpr uint32_t Id = 0x02;
} /* LowBattery */
namespace HardwareFault {
inline constexpr uint32_t Id = 0x03;
} /* HardwareFault */
namespace EndOfService {
inline constexpr uint32_t Id = 0x04;
} /* EndOfService */
namespace SelfTestComplete {
inline constexpr uint32_t Id = 0x05;
} /* SelfTestComplete */
namespace AlarmMuted {
inline constexpr uint32_t Id = 0x06;
} /* AlarmMuted */
namespace MuteEnded {
inline constexpr uint32_t Id = 0x07;
} /* MuteEnded */
namespace InterconnectSmokeAlarm {
inline constexpr uint32_t Id = 0x08;
} /* InterconnectSmokeAlarm */
namespace InterconnectCOAlarm {
inline constexpr uint32_t Id = 0x09;
} /* InterconnectCOAlarm */
namespace AllClear {
inline constexpr uint32_t Id = 0x0A;
} /* AllClear */
} /* event */

} /* smoke_co_alarm */
} /* cluster */
} /* esp_matter */
