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
namespace group_key_management {

inline constexpr uint32_t Id = 0x003F;

namespace attribute {
namespace GroupKeyMap {
inline constexpr uint32_t Id = 0x0000;
} /* GroupKeyMap */
namespace GroupTable {
inline constexpr uint32_t Id = 0x0001;
} /* GroupTable */
namespace MaxGroupsPerFabric {
inline constexpr uint32_t Id = 0x0002;
} /* MaxGroupsPerFabric */
namespace MaxGroupKeysPerFabric {
inline constexpr uint32_t Id = 0x0003;
} /* MaxGroupKeysPerFabric */
} /* attribute */

namespace command {
namespace KeySetWrite {
inline constexpr uint32_t Id = 0x00;
} /* KeySetWrite */
namespace KeySetRead {
inline constexpr uint32_t Id = 0x01;
} /* KeySetRead */
namespace KeySetReadResponse {
inline constexpr uint32_t Id = 0x02;
} /* KeySetReadResponse */
namespace KeySetRemove {
inline constexpr uint32_t Id = 0x03;
} /* KeySetRemove */
namespace KeySetReadAllIndices {
inline constexpr uint32_t Id = 0x04;
} /* KeySetReadAllIndices */
namespace KeySetReadAllIndicesResponse {
inline constexpr uint32_t Id = 0x05;
} /* KeySetReadAllIndicesResponse */
} /* command */

} /* group_key_management */
} /* cluster */
} /* esp_matter */
