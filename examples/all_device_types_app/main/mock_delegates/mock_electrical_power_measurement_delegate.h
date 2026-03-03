/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/electrical-power-measurement-server/ElectricalPowerMeasurementCluster.h>

/*
 * Mock ElectricalPowerMeasurement Delegate Implementation
 * This file provides a mock implementation of the ElectricalPowerMeasurement::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/electrical-power-measurement-server/electrical-power-measurement-server.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/energy-management-app/energy-management-common/energy-reporting/include/ElectricalPowerMeasurementDelegate.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/energy-management-app/energy-management-common/energy-reporting/src/ElectricalPowerMeasurementDelegate.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace ElectricalPowerMeasurement {

class MockElectricalPowerMeasurementDelegate : public Delegate {
public:
    MockElectricalPowerMeasurementDelegate() = default;
    virtual ~MockElectricalPowerMeasurementDelegate() = default;

    // Delegate interface
    PowerModeEnum GetPowerMode() override;
    uint8_t GetNumberOfMeasurementTypes() override;

    CHIP_ERROR StartAccuracyRead() override;
    CHIP_ERROR GetAccuracyByIndex(uint8_t index, Structs::MeasurementAccuracyStruct::Type  &accuracy) override;
    CHIP_ERROR EndAccuracyRead() override;

    CHIP_ERROR StartRangesRead() override;
    CHIP_ERROR GetRangeByIndex(uint8_t index, Structs::MeasurementRangeStruct::Type  &range) override;
    CHIP_ERROR EndRangesRead() override;

    CHIP_ERROR StartHarmonicCurrentsRead() override;
    CHIP_ERROR GetHarmonicCurrentsByIndex(uint8_t index, Structs::HarmonicMeasurementStruct::Type  &harmonic) override;
    CHIP_ERROR EndHarmonicCurrentsRead() override;

    CHIP_ERROR StartHarmonicPhasesRead() override;
    CHIP_ERROR GetHarmonicPhasesByIndex(uint8_t index, Structs::HarmonicMeasurementStruct::Type  &harmonic) override;
    CHIP_ERROR EndHarmonicPhasesRead() override;

    DataModel::Nullable<int64_t> GetVoltage() override;
    DataModel::Nullable<int64_t> GetActiveCurrent() override;
    DataModel::Nullable<int64_t> GetReactiveCurrent() override;
    DataModel::Nullable<int64_t> GetApparentCurrent() override;
    DataModel::Nullable<int64_t> GetActivePower() override;
    DataModel::Nullable<int64_t> GetReactivePower() override;
    DataModel::Nullable<int64_t> GetApparentPower() override;
    DataModel::Nullable<int64_t> GetRMSVoltage() override;
    DataModel::Nullable<int64_t> GetRMSCurrent() override;
    DataModel::Nullable<int64_t> GetRMSPower() override;
    DataModel::Nullable<int64_t> GetFrequency() override;
    DataModel::Nullable<int64_t> GetPowerFactor() override;
    DataModel::Nullable<int64_t> GetNeutralCurrent() override;
private:
    static constexpr const char * LOG_TAG = "MockElectricalPowerMeasurementDelegate";
};

} // namespace ElectricalPowerMeasurement
} // namespace Clusters
} // namespace app
} // namespace chip