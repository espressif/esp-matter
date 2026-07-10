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
namespace general_commissioning {

inline constexpr uint32_t Id = 0x0030;

namespace feature {
namespace TermsAndConditions {
inline constexpr uint32_t Id = 0x1;
} /* TermsAndConditions */
} /* feature */

namespace attribute {
namespace Breadcrumb {
inline constexpr uint32_t Id = 0x0000;
} /* Breadcrumb */
namespace BasicCommissioningInfo {
inline constexpr uint32_t Id = 0x0001;
} /* BasicCommissioningInfo */
namespace RegulatoryConfig {
inline constexpr uint32_t Id = 0x0002;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* RegulatoryConfig */
namespace LocationCapability {
inline constexpr uint32_t Id = 0x0003;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* LocationCapability */
namespace SupportsConcurrentConnection {
inline constexpr uint32_t Id = 0x0004;
} /* SupportsConcurrentConnection */
namespace TCAcceptedVersion {
inline constexpr uint32_t Id = 0x0005;
} /* TCAcceptedVersion */
namespace TCMinRequiredVersion {
inline constexpr uint32_t Id = 0x0006;
} /* TCMinRequiredVersion */
namespace TCAcknowledgements {
inline constexpr uint32_t Id = 0x0007;
} /* TCAcknowledgements */
namespace TCAcknowledgementsRequired {
inline constexpr uint32_t Id = 0x0008;
} /* TCAcknowledgementsRequired */
namespace TCUpdateDeadline {
inline constexpr uint32_t Id = 0x0009;
} /* TCUpdateDeadline */
} /* attribute */

namespace command {
namespace ArmFailSafe {
inline constexpr uint32_t Id = 0x00;
} /* ArmFailSafe */
namespace ArmFailSafeResponse {
inline constexpr uint32_t Id = 0x01;
} /* ArmFailSafeResponse */
namespace SetRegulatoryConfig {
inline constexpr uint32_t Id = 0x02;
} /* SetRegulatoryConfig */
namespace SetRegulatoryConfigResponse {
inline constexpr uint32_t Id = 0x03;
} /* SetRegulatoryConfigResponse */
namespace CommissioningComplete {
inline constexpr uint32_t Id = 0x04;
} /* CommissioningComplete */
namespace CommissioningCompleteResponse {
inline constexpr uint32_t Id = 0x05;
} /* CommissioningCompleteResponse */
namespace SetTCAcknowledgements {
inline constexpr uint32_t Id = 0x06;
} /* SetTCAcknowledgements */
namespace SetTCAcknowledgementsResponse {
inline constexpr uint32_t Id = 0x07;
} /* SetTCAcknowledgementsResponse */
} /* command */

} /* general_commissioning */
} /* cluster */
} /* esp_matter */
