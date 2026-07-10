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
namespace joint_fabric_administrator {

inline constexpr uint32_t Id = 0x0753;

namespace attribute {
namespace AdministratorFabricIndex {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint8_t Min = 1;
inline constexpr uint8_t Max = 254;
} /* AdministratorFabricIndex */
} /* attribute */

namespace command {
namespace ICACCSRRequest {
inline constexpr uint32_t Id = 0x00;
} /* ICACCSRRequest */
namespace ICACCSRResponse {
inline constexpr uint32_t Id = 0x01;
} /* ICACCSRResponse */
namespace AddICAC {
inline constexpr uint32_t Id = 0x02;
} /* AddICAC */
namespace ICACResponse {
inline constexpr uint32_t Id = 0x03;
} /* ICACResponse */
namespace OpenJointCommissioningWindow {
inline constexpr uint32_t Id = 0x04;
} /* OpenJointCommissioningWindow */
namespace TransferAnchorRequest {
inline constexpr uint32_t Id = 0x05;
} /* TransferAnchorRequest */
namespace TransferAnchorResponse {
inline constexpr uint32_t Id = 0x06;
} /* TransferAnchorResponse */
namespace TransferAnchorComplete {
inline constexpr uint32_t Id = 0x07;
} /* TransferAnchorComplete */
namespace AnnounceJointFabricAdministrator {
inline constexpr uint32_t Id = 0x08;
} /* AnnounceJointFabricAdministrator */
} /* command */

} /* joint_fabric_administrator */
} /* cluster */
} /* esp_matter */
