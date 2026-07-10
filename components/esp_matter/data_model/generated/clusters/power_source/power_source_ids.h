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
namespace power_source {

inline constexpr uint32_t Id = 0x002F;

namespace feature {
namespace Wired {
inline constexpr uint32_t Id = 0x1;
} /* Wired */
namespace Battery {
inline constexpr uint32_t Id = 0x2;
} /* Battery */
namespace Rechargeable {
inline constexpr uint32_t Id = 0x4;
} /* Rechargeable */
namespace Replaceable {
inline constexpr uint32_t Id = 0x8;
} /* Replaceable */
} /* feature */

namespace attribute {
namespace Status {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* Status */
namespace Order {
inline constexpr uint32_t Id = 0x0001;
} /* Order */
namespace Description {
inline constexpr uint32_t Id = 0x0002;
} /* Description */
namespace WiredAssessedInputVoltage {
inline constexpr uint32_t Id = 0x0003;
} /* WiredAssessedInputVoltage */
namespace WiredAssessedInputFrequency {
inline constexpr uint32_t Id = 0x0004;
} /* WiredAssessedInputFrequency */
namespace WiredCurrentType {
inline constexpr uint32_t Id = 0x0005;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 1;
} /* WiredCurrentType */
namespace WiredAssessedCurrent {
inline constexpr uint32_t Id = 0x0006;
} /* WiredAssessedCurrent */
namespace WiredNominalVoltage {
inline constexpr uint32_t Id = 0x0007;
} /* WiredNominalVoltage */
namespace WiredMaximumCurrent {
inline constexpr uint32_t Id = 0x0008;
} /* WiredMaximumCurrent */
namespace WiredPresent {
inline constexpr uint32_t Id = 0x0009;
} /* WiredPresent */
namespace ActiveWiredFaults {
inline constexpr uint32_t Id = 0x000A;
} /* ActiveWiredFaults */
namespace BatVoltage {
inline constexpr uint32_t Id = 0x000B;
} /* BatVoltage */
namespace BatPercentRemaining {
inline constexpr uint32_t Id = 0x000C;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 200;
} /* BatPercentRemaining */
namespace BatTimeRemaining {
inline constexpr uint32_t Id = 0x000D;
} /* BatTimeRemaining */
namespace BatChargeLevel {
inline constexpr uint32_t Id = 0x000E;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* BatChargeLevel */
namespace BatReplacementNeeded {
inline constexpr uint32_t Id = 0x000F;
} /* BatReplacementNeeded */
namespace BatReplaceability {
inline constexpr uint32_t Id = 0x0010;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* BatReplaceability */
namespace BatPresent {
inline constexpr uint32_t Id = 0x0011;
} /* BatPresent */
namespace ActiveBatFaults {
inline constexpr uint32_t Id = 0x0012;
} /* ActiveBatFaults */
namespace BatReplacementDescription {
inline constexpr uint32_t Id = 0x0013;
} /* BatReplacementDescription */
namespace BatCommonDesignation {
inline constexpr uint32_t Id = 0x0014;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 80;
} /* BatCommonDesignation */
namespace BatANSIDesignation {
inline constexpr uint32_t Id = 0x0015;
} /* BatANSIDesignation */
namespace BatIECDesignation {
inline constexpr uint32_t Id = 0x0016;
} /* BatIECDesignation */
namespace BatApprovedChemistry {
inline constexpr uint32_t Id = 0x0017;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 32;
} /* BatApprovedChemistry */
namespace BatCapacity {
inline constexpr uint32_t Id = 0x0018;
} /* BatCapacity */
namespace BatQuantity {
inline constexpr uint32_t Id = 0x0019;
} /* BatQuantity */
namespace BatChargeState {
inline constexpr uint32_t Id = 0x001A;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* BatChargeState */
namespace BatTimeToFullCharge {
inline constexpr uint32_t Id = 0x001B;
} /* BatTimeToFullCharge */
namespace BatFunctionalWhileCharging {
inline constexpr uint32_t Id = 0x001C;
} /* BatFunctionalWhileCharging */
namespace BatChargingCurrent {
inline constexpr uint32_t Id = 0x001D;
} /* BatChargingCurrent */
namespace ActiveBatChargeFaults {
inline constexpr uint32_t Id = 0x001E;
} /* ActiveBatChargeFaults */
namespace EndpointList {
inline constexpr uint32_t Id = 0x001F;
} /* EndpointList */
} /* attribute */

namespace event {
namespace WiredFaultChange {
inline constexpr uint32_t Id = 0x00;
} /* WiredFaultChange */
namespace BatFaultChange {
inline constexpr uint32_t Id = 0x01;
} /* BatFaultChange */
namespace BatChargeFaultChange {
inline constexpr uint32_t Id = 0x02;
} /* BatChargeFaultChange */
} /* event */

} /* power_source */
} /* cluster */
} /* esp_matter */
