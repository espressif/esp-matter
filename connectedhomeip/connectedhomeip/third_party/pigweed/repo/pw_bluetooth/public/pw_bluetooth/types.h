// Copyright 2022 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.
#pragma once

#include <array>
#include <cstdint>
#include <string_view>

#include "pw_bluetooth/address.h"
#include "pw_bluetooth/uuid.h"

namespace pw::bluetooth {

// 64-bit unique value used by the system to identify peer devices.
using PeerId = uint64_t;

using DeviceName = std::string_view;

// A 128-bit secret key.
using Key = std::array<uint8_t, 16>;

/// Refers to the role of a Bluetooth device in a physical channel piconet.
enum class ConnectionRole : uint8_t {
  // The connection initiating device.
  kCentral,
  // The advertising device.
  kPeripheral
};

/// Possible values for the LE Appearance property which describes the external
/// appearance of a peer at a high level.
/// (See the Bluetooth assigned-numbers document:
/// https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.gap.appearance.xml)
enum class Appearance : uint16_t {
  kUnknown = 0,
  kPhone = 64,
  kComputer = 128,
  kWatch = 192,
  kWatchSports = 193,
  kClock = 256,
  kDisplay = 320,
  kRemoteControl = 384,
  kEyeGlasses = 448,
  kTag = 512,
  kKeyring = 576,
  kMediaPlayer = 640,
  kBarcodeScanner = 704,
  kThermometer = 768,
  kThermometerEar = 769,
  kHeartRateSensor = 832,
  kHeartRateSensorBelt = 833,
  kBloodPressure = 896,
  kBloodPressureArm = 897,
  kBloodPressureWrist = 898,
  kHid = 960,
  kHidKeyboard = 961,
  kHidMouse = 962,
  kHidJoystick = 963,
  kHidGamepad = 964,
  kHidDigitizerTablet = 965,
  kHidCardReader = 966,
  kHidDigitalPen = 967,
  kHidBarcodeScanner = 968,
  kGlucoseMeter = 1024,
  kRunningWalkingSensor = 1088,
  kRunningWalkingSensorInShoe = 1089,
  kRunningWalkingSensorOnShoe = 1090,
  kRunningWalkingSensorOnHip = 1091,
  kCycling = 1152,
  kCyclingComputer = 1153,
  kCyclingSpeedSensor = 1154,
  kCyclingCadenceSensor = 1155,
  kCyclingPowerSensor = 1156,
  kCyclingSpeedAndCadenceSensor = 1157,
  kPulseOximeter = 3136,
  kPulseOximeterFingertip = 3137,
  kPulseOximeterWrist = 3138,
  kWeightScale = 3200,
  kPersonalMobility = 3264,
  kPersonalMobilityWheelchair = 3265,
  kPersonalMobilityScooter = 3266,
  kGlucoseMonitor = 3328,
  kSportsActivity = 5184,
  kSportsActivityLocationDisplay = 5185,
  kSportsActivityLocationAndNavDisplay = 5186,
  kSportsActivityLocationPod = 5187,
  kSportsActivityLocationAndNavPod = 5188,
};

}  // namespace pw::bluetooth