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
namespace pump_configuration_and_control {

inline constexpr uint32_t Id = 0x0200;

namespace feature {
namespace ConstantPressure {
inline constexpr uint32_t Id = 0x1;
} /* ConstantPressure */
namespace CompensatedPressure {
inline constexpr uint32_t Id = 0x2;
} /* CompensatedPressure */
namespace ConstantFlow {
inline constexpr uint32_t Id = 0x4;
} /* ConstantFlow */
namespace ConstantSpeed {
inline constexpr uint32_t Id = 0x8;
} /* ConstantSpeed */
namespace ConstantTemperature {
inline constexpr uint32_t Id = 0x10;
} /* ConstantTemperature */
namespace Automatic {
inline constexpr uint32_t Id = 0x20;
} /* Automatic */
namespace LocalOperation {
inline constexpr uint32_t Id = 0x40;
} /* LocalOperation */
} /* feature */

namespace attribute {
namespace MaxPressure {
inline constexpr uint32_t Id = 0x0000;
} /* MaxPressure */
namespace MaxSpeed {
inline constexpr uint32_t Id = 0x0001;
} /* MaxSpeed */
namespace MaxFlow {
inline constexpr uint32_t Id = 0x0002;
} /* MaxFlow */
namespace MinConstPressure {
inline constexpr uint32_t Id = 0x0003;
} /* MinConstPressure */
namespace MaxConstPressure {
inline constexpr uint32_t Id = 0x0004;
} /* MaxConstPressure */
namespace MinCompPressure {
inline constexpr uint32_t Id = 0x0005;
} /* MinCompPressure */
namespace MaxCompPressure {
inline constexpr uint32_t Id = 0x0006;
} /* MaxCompPressure */
namespace MinConstSpeed {
inline constexpr uint32_t Id = 0x0007;
} /* MinConstSpeed */
namespace MaxConstSpeed {
inline constexpr uint32_t Id = 0x0008;
} /* MaxConstSpeed */
namespace MinConstFlow {
inline constexpr uint32_t Id = 0x0009;
} /* MinConstFlow */
namespace MaxConstFlow {
inline constexpr uint32_t Id = 0x000A;
} /* MaxConstFlow */
namespace MinConstTemp {
inline constexpr uint32_t Id = 0x000B;
inline constexpr int16_t Min = -27315;
inline constexpr int16_t Max = 32767;
} /* MinConstTemp */
namespace MaxConstTemp {
inline constexpr uint32_t Id = 0x000C;
inline constexpr int16_t Min = -27315;
inline constexpr int16_t Max = 32767;
} /* MaxConstTemp */
namespace PumpStatus {
inline constexpr uint32_t Id = 0x0010;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 511;
} /* PumpStatus */
namespace EffectiveOperationMode {
inline constexpr uint32_t Id = 0x0011;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* EffectiveOperationMode */
namespace EffectiveControlMode {
inline constexpr uint32_t Id = 0x0012;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 5;
} /* EffectiveControlMode */
namespace Capacity {
inline constexpr uint32_t Id = 0x0013;
} /* Capacity */
namespace Speed {
inline constexpr uint32_t Id = 0x0014;
} /* Speed */
namespace LifetimeRunningHours {
inline constexpr uint32_t Id = 0x0015;
} /* LifetimeRunningHours */
namespace Power {
inline constexpr uint32_t Id = 0x0016;
} /* Power */
namespace LifetimeEnergyConsumed {
inline constexpr uint32_t Id = 0x0017;
} /* LifetimeEnergyConsumed */
namespace OperationMode {
inline constexpr uint32_t Id = 0x0020;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* OperationMode */
namespace ControlMode {
inline constexpr uint32_t Id = 0x0021;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 5;
} /* ControlMode */
} /* attribute */

namespace event {
namespace SupplyVoltageLow {
inline constexpr uint32_t Id = 0x00;
} /* SupplyVoltageLow */
namespace SupplyVoltageHigh {
inline constexpr uint32_t Id = 0x01;
} /* SupplyVoltageHigh */
namespace PowerMissingPhase {
inline constexpr uint32_t Id = 0x02;
} /* PowerMissingPhase */
namespace SystemPressureLow {
inline constexpr uint32_t Id = 0x03;
} /* SystemPressureLow */
namespace SystemPressureHigh {
inline constexpr uint32_t Id = 0x04;
} /* SystemPressureHigh */
namespace DryRunning {
inline constexpr uint32_t Id = 0x05;
} /* DryRunning */
namespace MotorTemperatureHigh {
inline constexpr uint32_t Id = 0x06;
} /* MotorTemperatureHigh */
namespace PumpMotorFatalFailure {
inline constexpr uint32_t Id = 0x07;
} /* PumpMotorFatalFailure */
namespace ElectronicTemperatureHigh {
inline constexpr uint32_t Id = 0x08;
} /* ElectronicTemperatureHigh */
namespace PumpBlocked {
inline constexpr uint32_t Id = 0x09;
} /* PumpBlocked */
namespace SensorFailure {
inline constexpr uint32_t Id = 0x0A;
} /* SensorFailure */
namespace ElectronicNonFatalFailure {
inline constexpr uint32_t Id = 0x0B;
} /* ElectronicNonFatalFailure */
namespace ElectronicFatalFailure {
inline constexpr uint32_t Id = 0x0C;
} /* ElectronicFatalFailure */
namespace GeneralFault {
inline constexpr uint32_t Id = 0x0D;
} /* GeneralFault */
namespace Leakage {
inline constexpr uint32_t Id = 0x0E;
} /* Leakage */
namespace AirDetection {
inline constexpr uint32_t Id = 0x0F;
} /* AirDetection */
namespace TurbineOperation {
inline constexpr uint32_t Id = 0x10;
} /* TurbineOperation */
} /* event */

} /* pump_configuration_and_control */
} /* cluster */
} /* esp_matter */
