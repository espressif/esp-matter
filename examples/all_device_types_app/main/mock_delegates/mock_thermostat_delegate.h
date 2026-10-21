/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/thermostat-server/Temperature.h>
#include <app/clusters/thermostat-server/ThermostatDelegate.h>
#include <credentials/FabricTable.h>

/*
 * Mock Thermostat Delegate Implementation
 * A minimal in-memory implementation of the (refactored) Thermostat::Delegate interface.
 * Upstream split the old monolithic delegate (presets/schedules/suggestions) into several
 * delegates; this mock implements only the core Thermostat::Delegate control state.
 * Reference: connectedhomeip/examples/thermostat/thermostat-common/include/thermostat-delegate-impl.h
 */

namespace chip {
namespace app {
namespace Clusters {
namespace Thermostat {

class MockThermostatDelegate : public Delegate {
public:
    MockThermostatDelegate() = default;

    FabricTable &GetFabricTable() const override;

    DataModel::Nullable<temperature> GetLocalTemperature() const override;
    Protocols::InteractionModel::Status SetLocalTemperature(DataModel::Nullable<temperature> localTemperature,
                                                            bool &changed) override;

    SystemModeEnum GetSystemMode() const override;
    Protocols::InteractionModel::Status SetSystemMode(SystemModeEnum systemMode, bool &changed) override;

    ControlSequenceOfOperationEnum GetControlSequenceOfOperation() const override;
    Protocols::InteractionModel::Status
    SetControlSequenceOfOperation(ControlSequenceOfOperationEnum controlSequenceOfOperation, bool &changed) override;

    Protocols::InteractionModel::Status GetRunningMode(ThermostatRunningModeEnum &runningMode) const override;
    Protocols::InteractionModel::Status SetRunningMode(ThermostatRunningModeEnum runningMode, bool &changed) override;

    Protocols::InteractionModel::Status GetRunningState(BitMask<RelayStateBitmap> &runningState) const override;
    Protocols::InteractionModel::Status SetRunningState(BitMask<RelayStateBitmap> runningState, bool &changed) override;

    Protocols::InteractionModel::Status SetRemoteSensing(BitMask<RemoteSensingBitmap> remoteSensing, bool &changed) override;

private:
    SystemModeEnum mSystemMode                                = SystemModeEnum::kOff;
    ThermostatRunningModeEnum mRunningMode                    = ThermostatRunningModeEnum::kOff;
    BitMask<RelayStateBitmap> mRunningState                   = BitMask<RelayStateBitmap>(0);
    ControlSequenceOfOperationEnum mControlSequenceOfOperation = ControlSequenceOfOperationEnum::kCoolingAndHeating;
    DataModel::Nullable<temperature> mLocalTemperature        = DataModel::Nullable<int16_t>();
    BitMask<RemoteSensingBitmap> mRemoteSensing              = BitMask<RemoteSensingBitmap>(0);
};

} // namespace Thermostat
} // namespace Clusters
} // namespace app
} // namespace chip
