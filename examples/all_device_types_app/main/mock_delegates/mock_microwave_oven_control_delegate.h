/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/microwave-oven-control-server/microwave-oven-control-server.h>

/*
 * Mock MicrowaveOvenControl Delegate Implementation
 * This file provides a mock implementation of the MicrowaveOvenControl::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/microwave-oven-control-server/microwave-oven-control-server.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/microwave-oven-app/microwave-oven-common/include/microwave-oven-device.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/microwave-oven-app/microwave-oven-common/src/microwave-oven-device.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace MicrowaveOvenControl {

class MockMicrowaveOvenControlDelegate : public Delegate {
public:
    MockMicrowaveOvenControlDelegate() = default;
    virtual ~MockMicrowaveOvenControlDelegate() = default;

    // Delegate interface
    Protocols::InteractionModel::Status HandleSetCookingParametersCallback(uint8_t cookMode, uint32_t cookTimeSec,
                                                                           bool startAfterSetting,
                                                                           Optional<uint8_t> powerSettingNum,
                                                                           Optional<uint8_t> wattSettingIndex) override;

    Protocols::InteractionModel::Status HandleModifyCookTimeSecondsCallback(uint32_t finalCookTimeSec) override;

    CHIP_ERROR GetWattSettingByIndex(uint8_t index, uint16_t  &wattSetting) override;

    uint32_t GetMaxCookTimeSec() const override;
    uint8_t GetPowerSettingNum() const override;
    uint8_t GetMinPowerNum() const override;
    uint8_t GetMaxPowerNum() const override;
    uint8_t GetPowerStepNum() const override;
    uint8_t GetCurrentWattIndex() const override;
    uint16_t GetWattRating() const override;
private:
    static constexpr const char * LOG_TAG = "MockMicrowaveOvenControlDelegate";
};

} // namespace MicrowaveOvenControl
} // namespace Clusters
} // namespace app
} // namespace chip