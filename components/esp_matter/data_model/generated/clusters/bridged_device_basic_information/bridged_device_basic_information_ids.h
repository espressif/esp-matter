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
namespace bridged_device_basic_information {

inline constexpr uint32_t Id = 0x0039;

namespace feature {
namespace BridgedICDSupport {
inline constexpr uint32_t Id = 0x100000;
} /* BridgedICDSupport */
} /* feature */

namespace attribute {
namespace VendorName {
inline constexpr uint32_t Id = 0x0001;
} /* VendorName */
namespace VendorID {
inline constexpr uint32_t Id = 0x0002;
} /* VendorID */
namespace ProductName {
inline constexpr uint32_t Id = 0x0003;
} /* ProductName */
namespace ProductID {
inline constexpr uint32_t Id = 0x0004;
} /* ProductID */
namespace NodeLabel {
inline constexpr uint32_t Id = 0x0005;
} /* NodeLabel */
namespace HardwareVersion {
inline constexpr uint32_t Id = 0x0007;
} /* HardwareVersion */
namespace HardwareVersionString {
inline constexpr uint32_t Id = 0x0008;
} /* HardwareVersionString */
namespace SoftwareVersion {
inline constexpr uint32_t Id = 0x0009;
} /* SoftwareVersion */
namespace SoftwareVersionString {
inline constexpr uint32_t Id = 0x000A;
} /* SoftwareVersionString */
namespace ManufacturingDate {
inline constexpr uint32_t Id = 0x000B;
} /* ManufacturingDate */
namespace PartNumber {
inline constexpr uint32_t Id = 0x000C;
} /* PartNumber */
namespace ProductURL {
inline constexpr uint32_t Id = 0x000D;
} /* ProductURL */
namespace ProductLabel {
inline constexpr uint32_t Id = 0x000E;
} /* ProductLabel */
namespace SerialNumber {
inline constexpr uint32_t Id = 0x000F;
} /* SerialNumber */
namespace Reachable {
inline constexpr uint32_t Id = 0x0011;
} /* Reachable */
namespace UniqueID {
inline constexpr uint32_t Id = 0x0012;
} /* UniqueID */
namespace ProductAppearance {
inline constexpr uint32_t Id = 0x0014;
} /* ProductAppearance */
namespace ConfigurationVersion {
inline constexpr uint32_t Id = 0x0018;
inline constexpr uint32_t Min = 1;
inline constexpr uint32_t Max = 4294967295;
} /* ConfigurationVersion */
} /* attribute */

namespace command {
namespace KeepActive {
inline constexpr uint32_t Id = 0x80;
} /* KeepActive */
} /* command */

namespace event {
namespace StartUp {
inline constexpr uint32_t Id = 0x00;
} /* StartUp */
namespace ShutDown {
inline constexpr uint32_t Id = 0x01;
} /* ShutDown */
namespace Leave {
inline constexpr uint32_t Id = 0x02;
} /* Leave */
namespace ReachableChanged {
inline constexpr uint32_t Id = 0x03;
} /* ReachableChanged */
namespace ActiveChanged {
inline constexpr uint32_t Id = 0x80;
} /* ActiveChanged */
} /* event */

} /* bridged_device_basic_information */
} /* cluster */
} /* esp_matter */
