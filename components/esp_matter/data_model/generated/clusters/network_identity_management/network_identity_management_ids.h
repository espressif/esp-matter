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
namespace network_identity_management {

inline constexpr uint32_t Id = 0x0450;

namespace attribute {
namespace ActiveNetworkIdentities {
inline constexpr uint32_t Id = 0x0000;
} /* ActiveNetworkIdentities */
namespace Clients {
inline constexpr uint32_t Id = 0x0001;
} /* Clients */
namespace ClientTableSize {
inline constexpr uint32_t Id = 0x0002;
inline constexpr uint16_t Min = 500;
inline constexpr uint16_t Max = 2047;
} /* ClientTableSize */
} /* attribute */

namespace command {
namespace AddClient {
inline constexpr uint32_t Id = 0x00;
} /* AddClient */
namespace AddClientResponse {
inline constexpr uint32_t Id = 0x01;
} /* AddClientResponse */
namespace RemoveClient {
inline constexpr uint32_t Id = 0x02;
} /* RemoveClient */
namespace QueryIdentity {
inline constexpr uint32_t Id = 0x03;
} /* QueryIdentity */
namespace QueryIdentityResponse {
inline constexpr uint32_t Id = 0x04;
} /* QueryIdentityResponse */
namespace ImportAdminSecret {
inline constexpr uint32_t Id = 0x40;
} /* ImportAdminSecret */
namespace ExportAdminSecret {
inline constexpr uint32_t Id = 0x41;
} /* ExportAdminSecret */
namespace ExportAdminSecretResponse {
inline constexpr uint32_t Id = 0x42;
} /* ExportAdminSecretResponse */
} /* command */

} /* network_identity_management */
} /* cluster */
} /* esp_matter */
