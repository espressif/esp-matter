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
namespace energy_evse {

inline constexpr uint32_t Id = 0x0099;

namespace feature {
namespace ChargingPreferences {
inline constexpr uint32_t Id = 0x1;
} /* ChargingPreferences */
namespace SoCReporting {
inline constexpr uint32_t Id = 0x2;
} /* SoCReporting */
namespace PlugAndCharge {
inline constexpr uint32_t Id = 0x4;
} /* PlugAndCharge */
namespace RFID {
inline constexpr uint32_t Id = 0x8;
} /* RFID */
namespace V2X {
inline constexpr uint32_t Id = 0x10;
} /* V2X */
} /* feature */

namespace attribute {
namespace State {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 6;
} /* State */
namespace SupplyState {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 5;
} /* SupplyState */
namespace FaultState {
inline constexpr uint32_t Id = 0x0002;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 16;
} /* FaultState */
namespace ChargingEnabledUntil {
inline constexpr uint32_t Id = 0x0003;
} /* ChargingEnabledUntil */
namespace DischargingEnabledUntil {
inline constexpr uint32_t Id = 0x0004;
} /* DischargingEnabledUntil */
namespace CircuitCapacity {
inline constexpr uint32_t Id = 0x0005;
inline constexpr int64_t Min = 0;
inline constexpr int64_t Max = 2147483647;
} /* CircuitCapacity */
namespace MinimumChargeCurrent {
inline constexpr uint32_t Id = 0x0006;
inline constexpr int64_t Min = 0;
inline constexpr int64_t Max = 2147483647;
} /* MinimumChargeCurrent */
namespace MaximumChargeCurrent {
inline constexpr uint32_t Id = 0x0007;
inline constexpr int64_t Min = 0;
inline constexpr int64_t Max = 2147483647;
} /* MaximumChargeCurrent */
namespace MaximumDischargeCurrent {
inline constexpr uint32_t Id = 0x0008;
inline constexpr int64_t Min = 0;
inline constexpr int64_t Max = 2147483647;
} /* MaximumDischargeCurrent */
namespace UserMaximumChargeCurrent {
inline constexpr uint32_t Id = 0x0009;
} /* UserMaximumChargeCurrent */
namespace RandomizationDelayWindow {
inline constexpr uint32_t Id = 0x000A;
inline constexpr uint32_t Min = 0;
inline constexpr uint32_t Max = 86400;
} /* RandomizationDelayWindow */
namespace NextChargeStartTime {
inline constexpr uint32_t Id = 0x0023;
} /* NextChargeStartTime */
namespace NextChargeTargetTime {
inline constexpr uint32_t Id = 0x0024;
} /* NextChargeTargetTime */
namespace NextChargeRequiredEnergy {
inline constexpr uint32_t Id = 0x0025;
inline constexpr int64_t Min = 0;
inline constexpr int64_t Max = 2147483647;
} /* NextChargeRequiredEnergy */
namespace NextChargeTargetSoC {
inline constexpr uint32_t Id = 0x0026;
} /* NextChargeTargetSoC */
namespace ApproximateEVEfficiency {
inline constexpr uint32_t Id = 0x0027;
} /* ApproximateEVEfficiency */
namespace StateOfCharge {
inline constexpr uint32_t Id = 0x0030;
} /* StateOfCharge */
namespace BatteryCapacity {
inline constexpr uint32_t Id = 0x0031;
inline constexpr int64_t Min = 0;
inline constexpr int64_t Max = 2147483647;
} /* BatteryCapacity */
namespace VehicleID {
inline constexpr uint32_t Id = 0x0032;
} /* VehicleID */
namespace SessionID {
inline constexpr uint32_t Id = 0x0040;
} /* SessionID */
namespace SessionDuration {
inline constexpr uint32_t Id = 0x0041;
} /* SessionDuration */
namespace SessionEnergyCharged {
inline constexpr uint32_t Id = 0x0042;
inline constexpr int64_t Min = 0;
inline constexpr int64_t Max = 2147483647;
} /* SessionEnergyCharged */
namespace SessionEnergyDischarged {
inline constexpr uint32_t Id = 0x0043;
inline constexpr int64_t Min = 0;
inline constexpr int64_t Max = 2147483647;
} /* SessionEnergyDischarged */
} /* attribute */

namespace command {
namespace GetTargetsResponse {
inline constexpr uint32_t Id = 0x00;
} /* GetTargetsResponse */
namespace Disable {
inline constexpr uint32_t Id = 0x01;
} /* Disable */
namespace EnableCharging {
inline constexpr uint32_t Id = 0x02;
} /* EnableCharging */
namespace EnableDischarging {
inline constexpr uint32_t Id = 0x03;
} /* EnableDischarging */
namespace StartDiagnostics {
inline constexpr uint32_t Id = 0x04;
} /* StartDiagnostics */
namespace SetTargets {
inline constexpr uint32_t Id = 0x05;
} /* SetTargets */
namespace GetTargets {
inline constexpr uint32_t Id = 0x06;
} /* GetTargets */
namespace ClearTargets {
inline constexpr uint32_t Id = 0x07;
} /* ClearTargets */
} /* command */

namespace event {
namespace EVConnected {
inline constexpr uint32_t Id = 0x00;
} /* EVConnected */
namespace EVNotDetected {
inline constexpr uint32_t Id = 0x01;
} /* EVNotDetected */
namespace EnergyTransferStarted {
inline constexpr uint32_t Id = 0x02;
} /* EnergyTransferStarted */
namespace EnergyTransferStopped {
inline constexpr uint32_t Id = 0x03;
} /* EnergyTransferStopped */
namespace Fault {
inline constexpr uint32_t Id = 0x04;
} /* Fault */
namespace RFID {
inline constexpr uint32_t Id = 0x05;
} /* RFID */
} /* event */

} /* energy_evse */
} /* cluster */
} /* esp_matter */
