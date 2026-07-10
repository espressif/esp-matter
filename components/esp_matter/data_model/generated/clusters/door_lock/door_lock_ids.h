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
namespace door_lock {

inline constexpr uint32_t Id = 0x0101;

namespace feature {
namespace PINCredential {
inline constexpr uint32_t Id = 0x1;
} /* PINCredential */
namespace WeekdayAccessSchedules {
inline constexpr uint32_t Id = 0x10;
} /* WeekdayAccessSchedules */
namespace DoorPositionSensor {
inline constexpr uint32_t Id = 0x20;
} /* DoorPositionSensor */
namespace CredentialOverTheAirAccess {
inline constexpr uint32_t Id = 0x80;
} /* CredentialOverTheAirAccess */
namespace YearDayAccessSchedules {
inline constexpr uint32_t Id = 0x400;
} /* YearDayAccessSchedules */
namespace HolidaySchedules {
inline constexpr uint32_t Id = 0x800;
} /* HolidaySchedules */
namespace Unbolting {
inline constexpr uint32_t Id = 0x1000;
} /* Unbolting */
namespace AliroProvisioning {
inline constexpr uint32_t Id = 0x2000;
} /* AliroProvisioning */
namespace AliroBLEUWB {
inline constexpr uint32_t Id = 0x4000;
} /* AliroBLEUWB */
} /* feature */

namespace attribute {
namespace LockState {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* LockState */
namespace LockType {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 11;
} /* LockType */
namespace ActuatorEnabled {
inline constexpr uint32_t Id = 0x0002;
} /* ActuatorEnabled */
namespace DoorState {
inline constexpr uint32_t Id = 0x0003;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 5;
} /* DoorState */
namespace DoorOpenEvents {
inline constexpr uint32_t Id = 0x0004;
} /* DoorOpenEvents */
namespace DoorClosedEvents {
inline constexpr uint32_t Id = 0x0005;
} /* DoorClosedEvents */
namespace OpenPeriod {
inline constexpr uint32_t Id = 0x0006;
} /* OpenPeriod */
namespace NumberOfPINUsersSupported {
inline constexpr uint32_t Id = 0x0012;
} /* NumberOfPINUsersSupported */
namespace NumberOfWeekDaySchedulesSupportedPerUser {
inline constexpr uint32_t Id = 0x0014;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 253;
} /* NumberOfWeekDaySchedulesSupportedPerUser */
namespace NumberOfYearDaySchedulesSupportedPerUser {
inline constexpr uint32_t Id = 0x0015;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 253;
} /* NumberOfYearDaySchedulesSupportedPerUser */
namespace NumberOfHolidaySchedulesSupported {
inline constexpr uint32_t Id = 0x0016;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 253;
} /* NumberOfHolidaySchedulesSupported */
namespace MaxPINCodeLength {
inline constexpr uint32_t Id = 0x0017;
} /* MaxPINCodeLength */
namespace MinPINCodeLength {
inline constexpr uint32_t Id = 0x0018;
} /* MinPINCodeLength */
namespace Language {
inline constexpr uint32_t Id = 0x0021;
} /* Language */
namespace LEDSettings {
inline constexpr uint32_t Id = 0x0022;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* LEDSettings */
namespace AutoRelockTime {
inline constexpr uint32_t Id = 0x0023;
} /* AutoRelockTime */
namespace SoundVolume {
inline constexpr uint32_t Id = 0x0024;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* SoundVolume */
namespace OperatingMode {
inline constexpr uint32_t Id = 0x0025;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 4;
} /* OperatingMode */
namespace SupportedOperatingModes {
inline constexpr uint32_t Id = 0x0026;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65535;
} /* SupportedOperatingModes */
namespace DefaultConfigurationRegister {
inline constexpr uint32_t Id = 0x0027;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65535;
} /* DefaultConfigurationRegister */
namespace EnableLocalProgramming {
inline constexpr uint32_t Id = 0x0028;
} /* EnableLocalProgramming */
namespace EnableOneTouchLocking {
inline constexpr uint32_t Id = 0x0029;
} /* EnableOneTouchLocking */
namespace EnableInsideStatusLED {
inline constexpr uint32_t Id = 0x002A;
} /* EnableInsideStatusLED */
namespace EnablePrivacyModeButton {
inline constexpr uint32_t Id = 0x002B;
} /* EnablePrivacyModeButton */
namespace LocalProgrammingFeatures {
inline constexpr uint32_t Id = 0x002C;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 15;
} /* LocalProgrammingFeatures */
namespace RequirePINforRemoteOperation {
inline constexpr uint32_t Id = 0x0033;
} /* RequirePINforRemoteOperation */
namespace AliroReaderVerificationKey {
inline constexpr uint32_t Id = 0x0080;
} /* AliroReaderVerificationKey */
namespace AliroReaderGroupIdentifier {
inline constexpr uint32_t Id = 0x0081;
} /* AliroReaderGroupIdentifier */
namespace AliroReaderGroupSubIdentifier {
inline constexpr uint32_t Id = 0x0082;
} /* AliroReaderGroupSubIdentifier */
namespace AliroExpeditedTransactionSupportedProtocolVersions {
inline constexpr uint32_t Id = 0x0083;
} /* AliroExpeditedTransactionSupportedProtocolVersions */
namespace AliroGroupResolvingKey {
inline constexpr uint32_t Id = 0x0084;
} /* AliroGroupResolvingKey */
namespace AliroSupportedBLEUWBProtocolVersions {
inline constexpr uint32_t Id = 0x0085;
} /* AliroSupportedBLEUWBProtocolVersions */
namespace AliroBLEAdvertisingVersion {
inline constexpr uint32_t Id = 0x0086;
} /* AliroBLEAdvertisingVersion */
namespace NumberOfAliroCredentialIssuerKeysSupported {
inline constexpr uint32_t Id = 0x0087;
} /* NumberOfAliroCredentialIssuerKeysSupported */
namespace NumberOfAliroEndpointKeysSupported {
inline constexpr uint32_t Id = 0x0088;
} /* NumberOfAliroEndpointKeysSupported */
} /* attribute */

namespace command {
namespace LockDoor {
inline constexpr uint32_t Id = 0x00;
} /* LockDoor */
namespace UnlockDoor {
inline constexpr uint32_t Id = 0x01;
} /* UnlockDoor */
namespace UnlockWithTimeout {
inline constexpr uint32_t Id = 0x03;
} /* UnlockWithTimeout */
namespace SetWeekDaySchedule {
inline constexpr uint32_t Id = 0x0B;
} /* SetWeekDaySchedule */
namespace GetWeekDaySchedule {
inline constexpr uint32_t Id = 0x0C;
} /* GetWeekDaySchedule */
namespace GetWeekDayScheduleResponse {
inline constexpr uint32_t Id = 0x0C;
} /* GetWeekDayScheduleResponse */
namespace ClearWeekDaySchedule {
inline constexpr uint32_t Id = 0x0D;
} /* ClearWeekDaySchedule */
namespace SetYearDaySchedule {
inline constexpr uint32_t Id = 0x0E;
} /* SetYearDaySchedule */
namespace GetYearDaySchedule {
inline constexpr uint32_t Id = 0x0F;
} /* GetYearDaySchedule */
namespace GetYearDayScheduleResponse {
inline constexpr uint32_t Id = 0x0F;
} /* GetYearDayScheduleResponse */
namespace ClearYearDaySchedule {
inline constexpr uint32_t Id = 0x10;
} /* ClearYearDaySchedule */
namespace SetHolidaySchedule {
inline constexpr uint32_t Id = 0x11;
} /* SetHolidaySchedule */
namespace GetHolidaySchedule {
inline constexpr uint32_t Id = 0x12;
} /* GetHolidaySchedule */
namespace GetHolidayScheduleResponse {
inline constexpr uint32_t Id = 0x12;
} /* GetHolidayScheduleResponse */
namespace ClearHolidaySchedule {
inline constexpr uint32_t Id = 0x13;
} /* ClearHolidaySchedule */
namespace UnboltDoor {
inline constexpr uint32_t Id = 0x27;
} /* UnboltDoor */
namespace SetAliroReaderConfig {
inline constexpr uint32_t Id = 0x28;
} /* SetAliroReaderConfig */
namespace ClearAliroReaderConfig {
inline constexpr uint32_t Id = 0x29;
} /* ClearAliroReaderConfig */
} /* command */

namespace event {
namespace DoorLockAlarm {
inline constexpr uint32_t Id = 0x00;
} /* DoorLockAlarm */
namespace DoorStateChange {
inline constexpr uint32_t Id = 0x01;
} /* DoorStateChange */
namespace LockOperation {
inline constexpr uint32_t Id = 0x02;
} /* LockOperation */
namespace LockOperationError {
inline constexpr uint32_t Id = 0x03;
} /* LockOperationError */
} /* event */

} /* door_lock */
} /* cluster */
} /* esp_matter */
