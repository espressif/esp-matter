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
namespace tls_client_management {

inline constexpr uint32_t Id = 0x0802;

namespace attribute {
namespace MaxProvisioned {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint8_t Min = 5;
inline constexpr uint8_t Max = 254;
} /* MaxProvisioned */
namespace ProvisionedEndpoints {
inline constexpr uint32_t Id = 0x0001;
} /* ProvisionedEndpoints */
} /* attribute */

namespace command {
namespace ProvisionEndpoint {
inline constexpr uint32_t Id = 0x00;
} /* ProvisionEndpoint */
namespace ProvisionEndpointResponse {
inline constexpr uint32_t Id = 0x01;
} /* ProvisionEndpointResponse */
namespace FindEndpoint {
inline constexpr uint32_t Id = 0x02;
} /* FindEndpoint */
namespace FindEndpointResponse {
inline constexpr uint32_t Id = 0x03;
} /* FindEndpointResponse */
namespace RemoveEndpoint {
inline constexpr uint32_t Id = 0x04;
} /* RemoveEndpoint */
} /* command */

} /* tls_client_management */
} /* cluster */
} /* esp_matter */
