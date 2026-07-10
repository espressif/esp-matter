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
namespace tls_certificate_management {

inline constexpr uint32_t Id = 0x0801;

namespace attribute {
namespace MaxRootCertificates {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint8_t Min = 5;
inline constexpr uint8_t Max = 254;
} /* MaxRootCertificates */
namespace ProvisionedRootCertificates {
inline constexpr uint32_t Id = 0x0001;
} /* ProvisionedRootCertificates */
namespace MaxClientCertificates {
inline constexpr uint32_t Id = 0x0002;
inline constexpr uint8_t Min = 2;
inline constexpr uint8_t Max = 254;
} /* MaxClientCertificates */
namespace ProvisionedClientCertificates {
inline constexpr uint32_t Id = 0x0003;
} /* ProvisionedClientCertificates */
} /* attribute */

namespace command {
namespace ProvisionRootCertificate {
inline constexpr uint32_t Id = 0x00;
} /* ProvisionRootCertificate */
namespace ProvisionRootCertificateResponse {
inline constexpr uint32_t Id = 0x01;
} /* ProvisionRootCertificateResponse */
namespace FindRootCertificate {
inline constexpr uint32_t Id = 0x02;
} /* FindRootCertificate */
namespace FindRootCertificateResponse {
inline constexpr uint32_t Id = 0x03;
} /* FindRootCertificateResponse */
namespace LookupRootCertificate {
inline constexpr uint32_t Id = 0x04;
} /* LookupRootCertificate */
namespace LookupRootCertificateResponse {
inline constexpr uint32_t Id = 0x05;
} /* LookupRootCertificateResponse */
namespace RemoveRootCertificate {
inline constexpr uint32_t Id = 0x06;
} /* RemoveRootCertificate */
namespace ClientCSR {
inline constexpr uint32_t Id = 0x07;
} /* ClientCSR */
namespace ClientCSRResponse {
inline constexpr uint32_t Id = 0x08;
} /* ClientCSRResponse */
namespace ProvisionClientCertificate {
inline constexpr uint32_t Id = 0x09;
} /* ProvisionClientCertificate */
namespace FindClientCertificate {
inline constexpr uint32_t Id = 0x0A;
} /* FindClientCertificate */
namespace FindClientCertificateResponse {
inline constexpr uint32_t Id = 0x0B;
} /* FindClientCertificateResponse */
namespace LookupClientCertificate {
inline constexpr uint32_t Id = 0x0C;
} /* LookupClientCertificate */
namespace LookupClientCertificateResponse {
inline constexpr uint32_t Id = 0x0D;
} /* LookupClientCertificateResponse */
namespace RemoveClientCertificate {
inline constexpr uint32_t Id = 0x0E;
} /* RemoveClientCertificate */
} /* command */

} /* tls_certificate_management */
} /* cluster */
} /* esp_matter */
