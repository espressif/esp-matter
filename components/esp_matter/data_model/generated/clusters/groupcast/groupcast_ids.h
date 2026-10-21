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
namespace groupcast {

inline constexpr uint32_t Id = 0x0065;

namespace feature {
namespace Listener {
inline constexpr uint32_t Id = 0x1;
} /* Listener */
namespace Sender {
inline constexpr uint32_t Id = 0x2;
} /* Sender */
namespace PerGroup {
inline constexpr uint32_t Id = 0x4;
} /* PerGroup */
} /* feature */

namespace attribute {
namespace Membership {
inline constexpr uint32_t Id = 0x0000;
} /* Membership */
namespace MaxMembershipCount {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint16_t Min = 10;
inline constexpr uint16_t Max = 65535;
} /* MaxMembershipCount */
namespace MaxMcastAddrCount {
inline constexpr uint32_t Id = 0x0002;
inline constexpr uint16_t Min = 1;
inline constexpr uint16_t Max = 65535;
} /* MaxMcastAddrCount */
namespace UsedMcastAddrCount {
inline constexpr uint32_t Id = 0x0003;
} /* UsedMcastAddrCount */
namespace FabricUnderTest {
inline constexpr uint32_t Id = 0x0004;
} /* FabricUnderTest */
} /* attribute */

namespace command {
namespace JoinGroup {
inline constexpr uint32_t Id = 0x00;
} /* JoinGroup */
namespace LeaveGroup {
inline constexpr uint32_t Id = 0x01;
} /* LeaveGroup */
namespace LeaveGroupResponse {
inline constexpr uint32_t Id = 0x02;
} /* LeaveGroupResponse */
namespace UpdateGroupKey {
inline constexpr uint32_t Id = 0x03;
} /* UpdateGroupKey */
namespace ConfigureAuxiliaryACL {
inline constexpr uint32_t Id = 0x04;
} /* ConfigureAuxiliaryACL */
namespace GroupcastTesting {
inline constexpr uint32_t Id = 0x05;
} /* GroupcastTesting */
} /* command */

namespace event {
namespace GroupcastTesting {
inline constexpr uint32_t Id = 0x00;
} /* GroupcastTesting */
} /* event */

} /* groupcast */
} /* cluster */
} /* esp_matter */
