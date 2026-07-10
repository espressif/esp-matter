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
namespace groups {

inline constexpr uint32_t Id = 0x0004;

namespace feature {
namespace GroupNames {
inline constexpr uint32_t Id = 0x1;
} /* GroupNames */
} /* feature */

namespace attribute {
namespace NameSupport {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 1;
} /* NameSupport */
} /* attribute */

namespace command {
namespace AddGroup {
inline constexpr uint32_t Id = 0x00;
} /* AddGroup */
namespace AddGroupResponse {
inline constexpr uint32_t Id = 0x00;
} /* AddGroupResponse */
namespace ViewGroup {
inline constexpr uint32_t Id = 0x01;
} /* ViewGroup */
namespace ViewGroupResponse {
inline constexpr uint32_t Id = 0x01;
} /* ViewGroupResponse */
namespace GetGroupMembership {
inline constexpr uint32_t Id = 0x02;
} /* GetGroupMembership */
namespace GetGroupMembershipResponse {
inline constexpr uint32_t Id = 0x02;
} /* GetGroupMembershipResponse */
namespace RemoveGroup {
inline constexpr uint32_t Id = 0x03;
} /* RemoveGroup */
namespace RemoveGroupResponse {
inline constexpr uint32_t Id = 0x03;
} /* RemoveGroupResponse */
namespace RemoveAllGroups {
inline constexpr uint32_t Id = 0x04;
} /* RemoveAllGroups */
namespace AddGroupIfIdentifying {
inline constexpr uint32_t Id = 0x05;
} /* AddGroupIfIdentifying */
} /* command */

} /* groups */
} /* cluster */
} /* esp_matter */
