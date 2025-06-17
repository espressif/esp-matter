/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <esp_err.h>
#include <app/clusters/electrical-power-measurement-server/electrical-power-measurement-server.h>
#include <app/util/af-types.h>
#include <lib/core/CHIPError.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize and configure the Electrical Energy Measurement cluster
 * 
 * This function:
 * 1. Creates and initializes an attribute access object with imported and exported energy features
 * 2. Sets up measurement accuracy configuration
 * 3. Demonstrates sending periodic and cumulative energy measurement events
 * 
 * @param endpoint_id The endpoint ID where the electrical energy measurement cluster is located
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t electrical_measurement_example(uint16_t endpoint_id);

/**
 * @brief Example of sending energy measurement events
 * 
 * Demonstrates:
 * 1. Reporting periodic energy measurement with imported energy
 * 2. Reporting cumulative energy measurement with both imported and exported energy
 * 
 * @param endpoint_id The endpoint ID where the electrical energy measurement cluster is located
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t send_energy_measurement_events(uint16_t endpoint_id);
#ifdef __cplusplus
}
#endif

/**
 * Electrical Power Measurement Delegate implementation
 */
namespace chip {
namespace app {
namespace Clusters {
namespace ElectricalPowerMeasurement {

class ElectricalPowerMeasurementDelegate : public ElectricalPowerMeasurement::Delegate
{
public:
    ~ElectricalPowerMeasurementDelegate() = default;

    static constexpr uint8_t kMaxNumberOfMeasurementTypes     = 14; // From spec
    static constexpr uint8_t kDefaultNumberOfMeasurementTypes = 1;

    // Attribute Accessors
    PowerModeEnum GetPowerMode() override { return mPowerMode; }
    uint8_t GetNumberOfMeasurementTypes() override;

    /* These functions are called by the ReadAttribute handler to iterate through lists
     * The cluster server will call Start<Type>Read to allow the delegate to create a temporary
     * lock on the data.
     * The delegate is expected to not change these values once Start<Type>Read has been called
     * until the End<Type>Read() has been called (e.g. releasing a lock on the data)
     */
    CHIP_ERROR StartAccuracyRead() override;
    CHIP_ERROR GetAccuracyByIndex(uint8_t, Structs::MeasurementAccuracyStruct::Type &) override;
    CHIP_ERROR EndAccuracyRead() override;

    CHIP_ERROR StartRangesRead() override;
    CHIP_ERROR GetRangeByIndex(uint8_t, Structs::MeasurementRangeStruct::Type &) override;
    CHIP_ERROR EndRangesRead() override;

    CHIP_ERROR StartHarmonicCurrentsRead() override;
    CHIP_ERROR GetHarmonicCurrentsByIndex(uint8_t, Structs::HarmonicMeasurementStruct::Type &) override;
    CHIP_ERROR EndHarmonicCurrentsRead() override;

    CHIP_ERROR StartHarmonicPhasesRead() override;
    CHIP_ERROR GetHarmonicPhasesByIndex(uint8_t, Structs::HarmonicMeasurementStruct::Type &) override;
    CHIP_ERROR EndHarmonicPhasesRead() override;

    DataModel::Nullable<int64_t> GetVoltage() override { return mVoltage; }
    DataModel::Nullable<int64_t> GetActiveCurrent() override { return mActiveCurrent; }
    DataModel::Nullable<int64_t> GetReactiveCurrent() override { return mReactiveCurrent; }
    DataModel::Nullable<int64_t> GetApparentCurrent() override { return mApparentCurrent; }
    DataModel::Nullable<int64_t> GetActivePower() override { return mActivePower; }
    DataModel::Nullable<int64_t> GetReactivePower() override { return mReactivePower; }
    DataModel::Nullable<int64_t> GetApparentPower() override { return mApparentPower; }
    DataModel::Nullable<int64_t> GetRMSVoltage() override { return mRMSVoltage; }
    DataModel::Nullable<int64_t> GetRMSCurrent() override { return mRMSCurrent; }
    DataModel::Nullable<int64_t> GetRMSPower() override { return mRMSPower; }
    DataModel::Nullable<int64_t> GetFrequency() override { return mFrequency; }
    DataModel::Nullable<int64_t> GetPowerFactor() override { return mPowerFactor; }
    DataModel::Nullable<int64_t> GetNeutralCurrent() override { return mNeutralCurrent; };

    // Internal Application API to set attribute values
    CHIP_ERROR SetPowerMode(PowerModeEnum);
    CHIP_ERROR SetVoltage(DataModel::Nullable<int64_t>);
    CHIP_ERROR SetActiveCurrent(DataModel::Nullable<int64_t>);
    CHIP_ERROR SetReactiveCurrent(DataModel::Nullable<int64_t>);
    CHIP_ERROR SetApparentCurrent(DataModel::Nullable<int64_t>);
    CHIP_ERROR SetActivePower(DataModel::Nullable<int64_t>);
    CHIP_ERROR SetReactivePower(DataModel::Nullable<int64_t>);
    CHIP_ERROR SetApparentPower(DataModel::Nullable<int64_t>);
    CHIP_ERROR SetRMSVoltage(DataModel::Nullable<int64_t>);
    CHIP_ERROR SetRMSCurrent(DataModel::Nullable<int64_t>);
    CHIP_ERROR SetRMSPower(DataModel::Nullable<int64_t>);
    CHIP_ERROR SetFrequency(DataModel::Nullable<int64_t>);
    CHIP_ERROR SetPowerFactor(DataModel::Nullable<int64_t>);
    CHIP_ERROR SetNeutralCurrent(DataModel::Nullable<int64_t>);

private:
    // Attribute storage
    PowerModeEnum mPowerMode = PowerModeEnum::kUnknown;
    DataModel::Nullable<int64_t> mVoltage;
    DataModel::Nullable<int64_t> mActiveCurrent;
    DataModel::Nullable<int64_t> mReactiveCurrent;
    DataModel::Nullable<int64_t> mApparentCurrent;
    DataModel::Nullable<int64_t> mActivePower;
    DataModel::Nullable<int64_t> mReactivePower;
    DataModel::Nullable<int64_t> mApparentPower;
    DataModel::Nullable<int64_t> mRMSVoltage;
    DataModel::Nullable<int64_t> mRMSCurrent;
    DataModel::Nullable<int64_t> mRMSPower;
    DataModel::Nullable<int64_t> mFrequency;
    DataModel::Nullable<int64_t> mPowerFactor;
    DataModel::Nullable<int64_t> mNeutralCurrent;
};

class ElectricalPowerMeasurementInstance : public Instance
{
public:
    ElectricalPowerMeasurementInstance(EndpointId aEndpointId, ElectricalPowerMeasurementDelegate & aDelegate, Feature aFeature,
                                      OptionalAttributes aOptionalAttributes) :
        ElectricalPowerMeasurement::Instance(aEndpointId, aDelegate, aFeature, aOptionalAttributes)
    {
        mDelegate = &aDelegate;
    }

    // Delete copy constructor and assignment operator.
    ElectricalPowerMeasurementInstance(const ElectricalPowerMeasurementInstance &)             = delete;
    ElectricalPowerMeasurementInstance(const ElectricalPowerMeasurementInstance &&)            = delete;
    ElectricalPowerMeasurementInstance & operator=(const ElectricalPowerMeasurementInstance &) = delete;

    CHIP_ERROR Init();
    void Shutdown();

    ElectricalPowerMeasurementDelegate * GetDelegate() { return mDelegate; };

private:
    ElectricalPowerMeasurementDelegate * mDelegate;
};

} // namespace ElectricalPowerMeasurement
} // namespace Clusters
} // namespace app
} // namespace chip 