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
namespace color_control {

inline constexpr uint32_t Id = 0x0300;

namespace feature {
namespace HueSaturation {
inline constexpr uint32_t Id = 0x1;
} /* HueSaturation */
namespace EnhancedHue {
inline constexpr uint32_t Id = 0x2;
} /* EnhancedHue */
namespace ColorLoop {
inline constexpr uint32_t Id = 0x4;
} /* ColorLoop */
namespace XY {
inline constexpr uint32_t Id = 0x8;
} /* XY */
namespace ColorTemperature {
inline constexpr uint32_t Id = 0x10;
} /* ColorTemperature */
} /* feature */

namespace attribute {
namespace CurrentHue {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 254;
} /* CurrentHue */
namespace CurrentSaturation {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 254;
} /* CurrentSaturation */
namespace RemainingTime {
inline constexpr uint32_t Id = 0x0002;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65534;
} /* RemainingTime */
namespace CurrentX {
inline constexpr uint32_t Id = 0x0003;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65279;
} /* CurrentX */
namespace CurrentY {
inline constexpr uint32_t Id = 0x0004;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65279;
} /* CurrentY */
namespace DriftCompensation {
inline constexpr uint32_t Id = 0x0005;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 4;
} /* DriftCompensation */
namespace CompensationText {
inline constexpr uint32_t Id = 0x0006;
} /* CompensationText */
namespace ColorTemperatureMireds {
inline constexpr uint32_t Id = 0x0007;
inline constexpr uint16_t Min = 1;
inline constexpr uint16_t Max = 65279;
} /* ColorTemperatureMireds */
namespace ColorMode {
inline constexpr uint32_t Id = 0x0008;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* ColorMode */
namespace Options {
inline constexpr uint32_t Id = 0x000F;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 1;
} /* Options */
namespace NumberOfPrimaries {
inline constexpr uint32_t Id = 0x0010;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 6;
} /* NumberOfPrimaries */
namespace Primary1X {
inline constexpr uint32_t Id = 0x0011;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65279;
} /* Primary1X */
namespace Primary1Y {
inline constexpr uint32_t Id = 0x0012;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65279;
} /* Primary1Y */
namespace Primary1Intensity {
inline constexpr uint32_t Id = 0x0013;
} /* Primary1Intensity */
namespace Primary2X {
inline constexpr uint32_t Id = 0x0015;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65279;
} /* Primary2X */
namespace Primary2Y {
inline constexpr uint32_t Id = 0x0016;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65279;
} /* Primary2Y */
namespace Primary2Intensity {
inline constexpr uint32_t Id = 0x0017;
} /* Primary2Intensity */
namespace Primary3X {
inline constexpr uint32_t Id = 0x0019;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65279;
} /* Primary3X */
namespace Primary3Y {
inline constexpr uint32_t Id = 0x001A;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65279;
} /* Primary3Y */
namespace Primary3Intensity {
inline constexpr uint32_t Id = 0x001B;
} /* Primary3Intensity */
namespace Primary4X {
inline constexpr uint32_t Id = 0x0020;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65279;
} /* Primary4X */
namespace Primary4Y {
inline constexpr uint32_t Id = 0x0021;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65279;
} /* Primary4Y */
namespace Primary4Intensity {
inline constexpr uint32_t Id = 0x0022;
} /* Primary4Intensity */
namespace Primary5X {
inline constexpr uint32_t Id = 0x0024;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65279;
} /* Primary5X */
namespace Primary5Y {
inline constexpr uint32_t Id = 0x0025;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65279;
} /* Primary5Y */
namespace Primary5Intensity {
inline constexpr uint32_t Id = 0x0026;
} /* Primary5Intensity */
namespace Primary6X {
inline constexpr uint32_t Id = 0x0028;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65279;
} /* Primary6X */
namespace Primary6Y {
inline constexpr uint32_t Id = 0x0029;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65279;
} /* Primary6Y */
namespace Primary6Intensity {
inline constexpr uint32_t Id = 0x002A;
} /* Primary6Intensity */
namespace WhitePointX {
inline constexpr uint32_t Id = 0x0030;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65279;
} /* WhitePointX */
namespace WhitePointY {
inline constexpr uint32_t Id = 0x0031;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65279;
} /* WhitePointY */
namespace ColorPointRX {
inline constexpr uint32_t Id = 0x0032;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65279;
} /* ColorPointRX */
namespace ColorPointRY {
inline constexpr uint32_t Id = 0x0033;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65279;
} /* ColorPointRY */
namespace ColorPointRIntensity {
inline constexpr uint32_t Id = 0x0034;
} /* ColorPointRIntensity */
namespace ColorPointGX {
inline constexpr uint32_t Id = 0x0036;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65279;
} /* ColorPointGX */
namespace ColorPointGY {
inline constexpr uint32_t Id = 0x0037;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65279;
} /* ColorPointGY */
namespace ColorPointGIntensity {
inline constexpr uint32_t Id = 0x0038;
} /* ColorPointGIntensity */
namespace ColorPointBX {
inline constexpr uint32_t Id = 0x003A;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65279;
} /* ColorPointBX */
namespace ColorPointBY {
inline constexpr uint32_t Id = 0x003B;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65279;
} /* ColorPointBY */
namespace ColorPointBIntensity {
inline constexpr uint32_t Id = 0x003C;
} /* ColorPointBIntensity */
namespace EnhancedCurrentHue {
inline constexpr uint32_t Id = 0x4000;
} /* EnhancedCurrentHue */
namespace EnhancedColorMode {
inline constexpr uint32_t Id = 0x4001;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* EnhancedColorMode */
namespace ColorLoopActive {
inline constexpr uint32_t Id = 0x4002;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 1;
} /* ColorLoopActive */
namespace ColorLoopDirection {
inline constexpr uint32_t Id = 0x4003;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 1;
} /* ColorLoopDirection */
namespace ColorLoopTime {
inline constexpr uint32_t Id = 0x4004;
} /* ColorLoopTime */
namespace ColorLoopStartEnhancedHue {
inline constexpr uint32_t Id = 0x4005;
} /* ColorLoopStartEnhancedHue */
namespace ColorLoopStoredEnhancedHue {
inline constexpr uint32_t Id = 0x4006;
} /* ColorLoopStoredEnhancedHue */
namespace ColorCapabilities {
inline constexpr uint32_t Id = 0x400A;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65535;
} /* ColorCapabilities */
namespace ColorTempPhysicalMinMireds {
inline constexpr uint32_t Id = 0x400B;
inline constexpr uint16_t Min = 1;
inline constexpr uint16_t Max = 65279;
} /* ColorTempPhysicalMinMireds */
namespace ColorTempPhysicalMaxMireds {
inline constexpr uint32_t Id = 0x400C;
inline constexpr uint16_t Min = 1;
inline constexpr uint16_t Max = 65279;
} /* ColorTempPhysicalMaxMireds */
namespace CoupleColorTempToLevelMinMireds {
inline constexpr uint32_t Id = 0x400D;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 65534;
} /* CoupleColorTempToLevelMinMireds */
namespace StartUpColorTemperatureMireds {
inline constexpr uint32_t Id = 0x4010;
inline constexpr uint16_t Min = 1;
inline constexpr uint16_t Max = 65279;
} /* StartUpColorTemperatureMireds */
} /* attribute */

namespace command {
namespace MoveToHue {
inline constexpr uint32_t Id = 0x00;
} /* MoveToHue */
namespace MoveHue {
inline constexpr uint32_t Id = 0x01;
} /* MoveHue */
namespace StepHue {
inline constexpr uint32_t Id = 0x02;
} /* StepHue */
namespace MoveToSaturation {
inline constexpr uint32_t Id = 0x03;
} /* MoveToSaturation */
namespace MoveSaturation {
inline constexpr uint32_t Id = 0x04;
} /* MoveSaturation */
namespace StepSaturation {
inline constexpr uint32_t Id = 0x05;
} /* StepSaturation */
namespace MoveToHueAndSaturation {
inline constexpr uint32_t Id = 0x06;
} /* MoveToHueAndSaturation */
namespace MoveToColor {
inline constexpr uint32_t Id = 0x07;
} /* MoveToColor */
namespace MoveColor {
inline constexpr uint32_t Id = 0x08;
} /* MoveColor */
namespace StepColor {
inline constexpr uint32_t Id = 0x09;
} /* StepColor */
namespace MoveToColorTemperature {
inline constexpr uint32_t Id = 0x0A;
} /* MoveToColorTemperature */
namespace EnhancedMoveToHue {
inline constexpr uint32_t Id = 0x40;
} /* EnhancedMoveToHue */
namespace EnhancedMoveHue {
inline constexpr uint32_t Id = 0x41;
} /* EnhancedMoveHue */
namespace EnhancedStepHue {
inline constexpr uint32_t Id = 0x42;
} /* EnhancedStepHue */
namespace EnhancedMoveToHueAndSaturation {
inline constexpr uint32_t Id = 0x43;
} /* EnhancedMoveToHueAndSaturation */
namespace ColorLoopSet {
inline constexpr uint32_t Id = 0x44;
} /* ColorLoopSet */
namespace StopMoveStep {
inline constexpr uint32_t Id = 0x47;
} /* StopMoveStep */
namespace MoveColorTemperature {
inline constexpr uint32_t Id = 0x4B;
} /* MoveColorTemperature */
namespace StepColorTemperature {
inline constexpr uint32_t Id = 0x4C;
} /* StepColorTemperature */
} /* command */

} /* color_control */
} /* cluster */
} /* esp_matter */
