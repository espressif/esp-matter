/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "mock_thermostat_delegate.h"

#include <app/server/Server.h>

namespace chip {
namespace app {
namespace Clusters {
namespace Thermostat {

using Status = Protocols::InteractionModel::Status;

FabricTable &MockThermostatDelegate::GetFabricTable() const
{
    return Server::GetInstance().GetFabricTable();
}

DataModel::Nullable<temperature> MockThermostatDelegate::GetLocalTemperature() const
{
    return mLocalTemperature;
}

Status MockThermostatDelegate::SetLocalTemperature(DataModel::Nullable<temperature> localTemperature, bool &changed)
{
    changed = (mLocalTemperature != localTemperature);
    mLocalTemperature = localTemperature;
    return Status::Success;
}

SystemModeEnum MockThermostatDelegate::GetSystemMode() const
{
    return mSystemMode;
}

Status MockThermostatDelegate::SetSystemMode(SystemModeEnum systemMode, bool &changed)
{
    changed = (mSystemMode != systemMode);
    mSystemMode = systemMode;
    return Status::Success;
}

ControlSequenceOfOperationEnum MockThermostatDelegate::GetControlSequenceOfOperation() const
{
    return mControlSequenceOfOperation;
}

Status MockThermostatDelegate::SetControlSequenceOfOperation(ControlSequenceOfOperationEnum controlSequenceOfOperation,
                                                             bool &changed)
{
    changed = (mControlSequenceOfOperation != controlSequenceOfOperation);
    mControlSequenceOfOperation = controlSequenceOfOperation;
    return Status::Success;
}

Status MockThermostatDelegate::GetRunningMode(ThermostatRunningModeEnum &runningMode) const
{
    runningMode = mRunningMode;
    return Status::Success;
}

Status MockThermostatDelegate::SetRunningMode(ThermostatRunningModeEnum runningMode, bool &changed)
{
    changed = (mRunningMode != runningMode);
    mRunningMode = runningMode;
    return Status::Success;
}

Status MockThermostatDelegate::GetRunningState(BitMask<RelayStateBitmap> &runningState) const
{
    runningState = mRunningState;
    return Status::Success;
}

Status MockThermostatDelegate::SetRunningState(BitMask<RelayStateBitmap> runningState, bool &changed)
{
    changed = (mRunningState.Raw() != runningState.Raw());
    mRunningState = runningState;
    return Status::Success;
}

Status MockThermostatDelegate::SetRemoteSensing(BitMask<RemoteSensingBitmap> remoteSensing, bool &changed)
{
    changed = (mRemoteSensing.Raw() != remoteSensing.Raw());
    mRemoteSensing = remoteSensing;
    return Status::Success;
}

} // namespace Thermostat
} // namespace Clusters
} // namespace app
} // namespace chip
