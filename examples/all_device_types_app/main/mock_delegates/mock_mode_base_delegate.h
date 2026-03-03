/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/mode-base-server/mode-base-server.h>

/*
 * Mock Delegate for all clusters derived from ModeBase cluster:
 * ModeEVSE, ModeOven, ModeRVSRun, ModeRVSClean, ModeDishwasher, ModeWaterHeater, ModeRefrigerator, ModeLaundryWasher and ModeMicrowaveOven clusters.
 * This file provides a mock implementation of the ModeBase Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface:
 *  https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/mode-base-server/mode-base-server.h
 * 2. Delegate Implementation:
 *  ModeEVSE:
 *      https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/energy-management-app/energy-management-common/energy-evse/include/energy-evse-modes.h
 *      https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/energy-management-app/energy-management-common/energy-evse/src/energy-evse-mode.cpp
 *
 *  ModeOven:
 *      https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/oven-modes.h
 *      https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/src/oven-modes.cpp
 *
 *  ModeRVSRun, ModeRVSClean:
 *      https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/rvc-app/rvc-common/include/rvc-mode-delegates.h
 *      https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/rvc-app/rvc-common/src/rvc-mode-delegates.cpp
 *
 *  ModeDishwasher:
 *      https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/dishwasher-mode.h
 *      https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/src/dishwasher-mode.cpp
 *
 *  ModeWaterHeater:
 *      https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/energy-management-app/energy-management-common/water-heater/include/water-heater-mode.h
 *      https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/energy-management-app/energy-management-common/water-heater/src/water-heater-mode.cpp
 *
 *  ModeRefrigerator:
 *      https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/tcc-mode.h
 *      https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/src/tcc-mode.cpp
 *
 *  ModeLaundryWasher:
 *      https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/laundry-washer-mode.h
 *      https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/src/laundry-washer-mode.cpp
 *
 *  ModeRefrigerator:
 *      https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/microwave-oven-mode.h
 *      https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/src/microwave-oven-mode.cpp
 *
 *  ModeDeviceEnergyManagement:
 *      https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/energy-management-app/energy-management-common/device-energy-management/include/device-energy-management-modes.h
 *      https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/energy-management-app/energy-management-common/device-energy-management/src/device-energy-management-mode.cpp
 *
 */

namespace chip {
namespace app {
namespace Clusters {
namespace ModeBase {

class MockModeBaseDelegate : public Delegate {
public:
    MockModeBaseDelegate() = default;

    CHIP_ERROR Init() override;
    CHIP_ERROR GetModeLabelByIndex(uint8_t modeIndex, MutableCharSpan  &label) override;
    CHIP_ERROR GetModeValueByIndex(uint8_t modeIndex, uint8_t  &value) override;
    CHIP_ERROR GetModeTagsByIndex(uint8_t modeIndex, DataModel::List<detail::Structs::ModeTagStruct::Type>  &modeTags) override;
    void HandleChangeToMode(uint8_t NewMode, ModeBase::Commands::ChangeToModeResponse::Type  &response) override;

private:
    const char *LOG_TAG = "mode_base";
};

} // namespace ModeBase
} // namespace Clusters
} // namespace app
} // namespace chip
