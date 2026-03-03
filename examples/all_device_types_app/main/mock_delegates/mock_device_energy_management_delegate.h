/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/device-energy-management-server/device-energy-management-server.h>

/*
 * Mock DeviceEnergyManagement Delegate Implementation
 * This file provides a mock implementation of the DeviceEnergyManagement::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/device-energy-management-server/device-energy-management-server.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/energy-management-app/energy-management-common/device-energy-management/include/DeviceEnergyManagementDelegateImpl.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/energy-management-app/energy-management-common/device-energy-management/src/DeviceEnergyManagementDelegateImpl.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace DeviceEnergyManagement {

class MockDeviceEnergyManagementDelegate : public Delegate {
public:
    MockDeviceEnergyManagementDelegate() = default;
    virtual ~MockDeviceEnergyManagementDelegate() = default;

    // Command handlers
    Protocols::InteractionModel::Status PowerAdjustRequest(const int64_t power, const uint32_t duration,
                                                           AdjustmentCauseEnum cause) override;

    Protocols::InteractionModel::Status CancelPowerAdjustRequest() override;

    Protocols::InteractionModel::Status StartTimeAdjustRequest(const uint32_t requestedStartTime,
                                                               AdjustmentCauseEnum cause) override;

    Protocols::InteractionModel::Status PauseRequest(const uint32_t duration, AdjustmentCauseEnum cause) override;

    Protocols::InteractionModel::Status ResumeRequest() override;

    Protocols::InteractionModel::Status
    ModifyForecastRequest(const uint32_t forecastID,
                          const DataModel::DecodableList<Structs::SlotAdjustmentStruct::Type>  &slotAdjustments,
                          AdjustmentCauseEnum cause) override;

    Protocols::InteractionModel::Status
    RequestConstraintBasedForecast(const DataModel::DecodableList<Structs::ConstraintsStruct::Type>  &constraints,
                                   AdjustmentCauseEnum cause) override;

    Protocols::InteractionModel::Status CancelRequest() override;

    // Attribute getters
    ESATypeEnum GetESAType() override;
    bool GetESACanGenerate() override;
    ESAStateEnum GetESAState() override;
    int64_t GetAbsMinPower() override;
    int64_t GetAbsMaxPower() override;
    OptOutStateEnum GetOptOutState() override;
    const DataModel::Nullable<Structs::PowerAdjustCapabilityStruct::Type>  &GetPowerAdjustmentCapability() override;
    const DataModel::Nullable<Structs::ForecastStruct::Type>  &GetForecast() override;

    // Attribute setter
    CHIP_ERROR SetESAState(ESAStateEnum esaState) override;

private:
    static constexpr const char * LOG_TAG = "MockDeviceEnergyManagementDelegate";
    DataModel::Nullable<Structs::PowerAdjustCapabilityStruct::Type> mPowerAdjustCapability;
    DataModel::Nullable<Structs::ForecastStruct::Type> mForecast;
};

} // namespace DeviceEnergyManagement
} // namespace Clusters
} // namespace app
} // namespace chip