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
namespace general_diagnostics {

inline constexpr uint32_t Id = 0x0033;

namespace feature {
namespace DataModelTest {
inline constexpr uint32_t Id = 0x1;
} /* DataModelTest */
} /* feature */

namespace attribute {
namespace NetworkInterfaces {
inline constexpr uint32_t Id = 0x0000;
} /* NetworkInterfaces */
namespace RebootCount {
inline constexpr uint32_t Id = 0x0001;
} /* RebootCount */
namespace UpTime {
inline constexpr uint32_t Id = 0x0002;
} /* UpTime */
namespace TotalOperationalHours {
inline constexpr uint32_t Id = 0x0003;
} /* TotalOperationalHours */
namespace BootReason {
inline constexpr uint32_t Id = 0x0004;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 6;
} /* BootReason */
namespace ActiveHardwareFaults {
inline constexpr uint32_t Id = 0x0005;
} /* ActiveHardwareFaults */
namespace ActiveRadioFaults {
inline constexpr uint32_t Id = 0x0006;
} /* ActiveRadioFaults */
namespace ActiveNetworkFaults {
inline constexpr uint32_t Id = 0x0007;
} /* ActiveNetworkFaults */
namespace TestEventTriggersEnabled {
inline constexpr uint32_t Id = 0x0008;
} /* TestEventTriggersEnabled */
namespace DeviceLoadStatus {
inline constexpr uint32_t Id = 0x000A;
} /* DeviceLoadStatus */
} /* attribute */

namespace command {
namespace TestEventTrigger {
inline constexpr uint32_t Id = 0x00;
} /* TestEventTrigger */
namespace TimeSnapshot {
inline constexpr uint32_t Id = 0x01;
} /* TimeSnapshot */
namespace TimeSnapshotResponse {
inline constexpr uint32_t Id = 0x02;
} /* TimeSnapshotResponse */
namespace PayloadTestRequest {
inline constexpr uint32_t Id = 0x03;
} /* PayloadTestRequest */
namespace PayloadTestResponse {
inline constexpr uint32_t Id = 0x04;
} /* PayloadTestResponse */
} /* command */

namespace event {
namespace HardwareFaultChange {
inline constexpr uint32_t Id = 0x00;
} /* HardwareFaultChange */
namespace RadioFaultChange {
inline constexpr uint32_t Id = 0x01;
} /* RadioFaultChange */
namespace NetworkFaultChange {
inline constexpr uint32_t Id = 0x02;
} /* NetworkFaultChange */
namespace BootReason {
inline constexpr uint32_t Id = 0x03;
} /* BootReason */
} /* event */

} /* general_diagnostics */
} /* cluster */
} /* esp_matter */
