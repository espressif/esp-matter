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
namespace access_control {

inline constexpr uint32_t Id = 0x001F;

namespace feature {
namespace Extension {
inline constexpr uint32_t Id = 0x1;
} /* Extension */
namespace ManagedDevice {
inline constexpr uint32_t Id = 0x2;
} /* ManagedDevice */
} /* feature */

namespace attribute {
namespace ACL {
inline constexpr uint32_t Id = 0x0000;
} /* ACL */
namespace Extension {
inline constexpr uint32_t Id = 0x0001;
} /* Extension */
namespace SubjectsPerAccessControlEntry {
inline constexpr uint32_t Id = 0x0002;
inline constexpr uint16_t Min = 4;
inline constexpr uint16_t Max = 65534;
} /* SubjectsPerAccessControlEntry */
namespace TargetsPerAccessControlEntry {
inline constexpr uint32_t Id = 0x0003;
inline constexpr uint16_t Min = 3;
inline constexpr uint16_t Max = 65534;
} /* TargetsPerAccessControlEntry */
namespace AccessControlEntriesPerFabric {
inline constexpr uint32_t Id = 0x0004;
inline constexpr uint16_t Min = 4;
inline constexpr uint16_t Max = 65534;
} /* AccessControlEntriesPerFabric */
namespace CommissioningARL {
inline constexpr uint32_t Id = 0x0005;
} /* CommissioningARL */
namespace ARL {
inline constexpr uint32_t Id = 0x0006;
} /* ARL */
} /* attribute */

namespace command {
namespace ReviewFabricRestrictions {
inline constexpr uint32_t Id = 0x00;
} /* ReviewFabricRestrictions */
namespace ReviewFabricRestrictionsResponse {
inline constexpr uint32_t Id = 0x01;
} /* ReviewFabricRestrictionsResponse */
} /* command */

namespace event {
namespace AccessControlEntryChanged {
inline constexpr uint32_t Id = 0x00;
} /* AccessControlEntryChanged */
namespace AccessControlExtensionChanged {
inline constexpr uint32_t Id = 0x01;
} /* AccessControlExtensionChanged */
namespace FabricRestrictionReviewUpdate {
inline constexpr uint32_t Id = 0x02;
} /* FabricRestrictionReviewUpdate */
} /* event */

} /* access_control */
} /* cluster */
} /* esp_matter */
