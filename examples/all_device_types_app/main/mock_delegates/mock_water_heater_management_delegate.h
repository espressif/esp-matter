/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/water-heater-management-server/water-heater-management-server.h>

/*
 * Mock WaterHeaterManagement Delegate Implementation
 * This file provides a mock implementation of the WaterHeaterManagement::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/water-heater-management-server/water-heater-management-server.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/energy-management-app/energy-management-common/water-heater/include/WhmDelegate.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/energy-management-app/energy-management-common/water-heater/src/WhmDelegateImpl.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace WaterHeaterManagement {

class MockWaterHeaterManagementDelegate : public Delegate {
public:
    MockWaterHeaterManagementDelegate() = default;
    virtual ~MockWaterHeaterManagementDelegate() = default;

    // Delegate interface
    Protocols::InteractionModel::Status HandleBoost(uint32_t duration, Optional<bool> oneShot,
                                                    Optional<bool> emergencyBoost, Optional<int16_t> temporarySetpoint,
                                                    Optional<Percent> targetPercentage, Optional<Percent> targetReheat) override;

    Protocols::InteractionModel::Status HandleCancelBoost() override;

    // Get attribute methods
    BitMask<WaterHeaterHeatSourceBitmap> GetHeaterTypes() override;
    BitMask<WaterHeaterHeatSourceBitmap> GetHeatDemand() override;
    uint16_t GetTankVolume() override;
    Energy_mWh GetEstimatedHeatRequired() override;
    Percent GetTankPercentage() override;
    BoostStateEnum GetBoostState() override;

private:
    static constexpr const char * LOG_TAG = "MockWaterHeaterManagementDelegate";
};

} // namespace WaterHeaterManagement
} // namespace Clusters
} // namespace app
} // namespace chip