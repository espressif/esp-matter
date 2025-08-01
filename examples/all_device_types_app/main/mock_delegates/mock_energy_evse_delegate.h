/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/energy-evse-server/energy-evse-server.h>

/*
 * Mock EnergyEVSE Delegate Implementation
 * This file provides a mock implementation of the EnergyEVSE::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/energy-evse-server/energy-evse-server.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/energy-management-app/energy-management-common/energy-evse/include/EnergyEvseDelegateImpl.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/energy-management-app/energy-management-common/energy-evse/src/EnergyEvseDelegateImpl.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace EnergyEvse {

class MockEnergyEVSEDelegate : public Delegate {
public:
    MockEnergyEVSEDelegate() = default;

    Protocols::InteractionModel::Status Disable() override;

    Protocols::InteractionModel::Status EnableCharging(const DataModel::Nullable<uint32_t>  &enableChargeTime,
                                                       const int64_t  &minimumChargeCurrent,
                                                       const int64_t  &maximumChargeCurrent) override;

    Protocols::InteractionModel::Status EnableDischarging(const DataModel::Nullable<uint32_t>  &enableDischargeTime,
                                                          const int64_t  &maximumDischargeCurrent) override;

    Protocols::InteractionModel::Status StartDiagnostics() override;

    Protocols::InteractionModel::Status
    SetTargets(const DataModel::DecodableList<Structs::ChargingTargetScheduleStruct::DecodableType>  &chargingTargetSchedules) override;

    Protocols::InteractionModel::Status LoadTargets() override;

    Protocols::InteractionModel::Status
    GetTargets(DataModel::List<const Structs::ChargingTargetScheduleStruct::Type>  &chargingTargetSchedules) override;

    Protocols::InteractionModel::Status ClearTargets() override;

    // ------------------------------------------------------------------
    // Get attribute methods
    StateEnum GetState() override;
    SupplyStateEnum GetSupplyState() override;
    FaultStateEnum GetFaultState() override;
    DataModel::Nullable<uint32_t> GetChargingEnabledUntil() override;
    DataModel::Nullable<uint32_t> GetDischargingEnabledUntil() override;
    int64_t GetCircuitCapacity() override;
    int64_t GetMinimumChargeCurrent() override;
    int64_t GetMaximumChargeCurrent() override;
    int64_t GetMaximumDischargeCurrent() override;
    int64_t GetUserMaximumChargeCurrent() override;
    uint32_t GetRandomizationDelayWindow() override;
    /* PREF attributes */
    DataModel::Nullable<uint32_t> GetNextChargeStartTime() override;
    DataModel::Nullable<uint32_t> GetNextChargeTargetTime() override;
    DataModel::Nullable<int64_t> GetNextChargeRequiredEnergy() override;
    DataModel::Nullable<Percent> GetNextChargeTargetSoC() override;
    DataModel::Nullable<uint16_t> GetApproximateEVEfficiency() override;

    /* SOC attributes */
    DataModel::Nullable<Percent> GetStateOfCharge() override;
    DataModel::Nullable<int64_t> GetBatteryCapacity() override;

    /* PNC attributes*/
    DataModel::Nullable<CharSpan> GetVehicleID() override;

    /* Session SESS attributes */
    DataModel::Nullable<uint32_t> GetSessionID() override;
    DataModel::Nullable<uint32_t> GetSessionDuration() override;
    DataModel::Nullable<int64_t> GetSessionEnergyCharged() override;
    DataModel::Nullable<int64_t> GetSessionEnergyDischarged() override;

    // ------------------------------------------------------------------
    // Set attribute methods
    CHIP_ERROR SetUserMaximumChargeCurrent(int64_t aNewValue) override;
    CHIP_ERROR SetRandomizationDelayWindow(uint32_t aNewValue) override;
    CHIP_ERROR SetApproximateEVEfficiency(DataModel::Nullable<uint16_t> aNewValue) override;
private:
    static constexpr const char * LOG_TAG = "energy-evse";
};

} // namespace EnergyEvse
} // namespace Clusters
} // namespace app
} // namespace chip