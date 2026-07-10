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
namespace operational_credentials {

inline constexpr uint32_t Id = 0x003E;

namespace attribute {
namespace NOCs {
inline constexpr uint32_t Id = 0x0000;
} /* NOCs */
namespace Fabrics {
inline constexpr uint32_t Id = 0x0001;
} /* Fabrics */
namespace SupportedFabrics {
inline constexpr uint32_t Id = 0x0002;
inline constexpr uint8_t Min = 5;
inline constexpr uint8_t Max = 254;
} /* SupportedFabrics */
namespace CommissionedFabrics {
inline constexpr uint32_t Id = 0x0003;
} /* CommissionedFabrics */
namespace TrustedRootCertificates {
inline constexpr uint32_t Id = 0x0004;
} /* TrustedRootCertificates */
namespace CurrentFabricIndex {
inline constexpr uint32_t Id = 0x0005;
} /* CurrentFabricIndex */
} /* attribute */

namespace command {
namespace AttestationRequest {
inline constexpr uint32_t Id = 0x00;
} /* AttestationRequest */
namespace AttestationResponse {
inline constexpr uint32_t Id = 0x01;
} /* AttestationResponse */
namespace CertificateChainRequest {
inline constexpr uint32_t Id = 0x02;
} /* CertificateChainRequest */
namespace CertificateChainResponse {
inline constexpr uint32_t Id = 0x03;
} /* CertificateChainResponse */
namespace CSRRequest {
inline constexpr uint32_t Id = 0x04;
} /* CSRRequest */
namespace CSRResponse {
inline constexpr uint32_t Id = 0x05;
} /* CSRResponse */
namespace AddNOC {
inline constexpr uint32_t Id = 0x06;
} /* AddNOC */
namespace UpdateNOC {
inline constexpr uint32_t Id = 0x07;
} /* UpdateNOC */
namespace NOCResponse {
inline constexpr uint32_t Id = 0x08;
} /* NOCResponse */
namespace UpdateFabricLabel {
inline constexpr uint32_t Id = 0x09;
} /* UpdateFabricLabel */
namespace RemoveFabric {
inline constexpr uint32_t Id = 0x0A;
} /* RemoveFabric */
namespace AddTrustedRootCertificate {
inline constexpr uint32_t Id = 0x0B;
} /* AddTrustedRootCertificate */
namespace SetVIDVerificationStatement {
inline constexpr uint32_t Id = 0x0C;
} /* SetVIDVerificationStatement */
namespace SignVIDVerificationRequest {
inline constexpr uint32_t Id = 0x0D;
} /* SignVIDVerificationRequest */
namespace SignVIDVerificationResponse {
inline constexpr uint32_t Id = 0x0E;
} /* SignVIDVerificationResponse */
} /* command */

} /* operational_credentials */
} /* cluster */
} /* esp_matter */
