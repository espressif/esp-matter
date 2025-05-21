/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "electrical_measurement.h"
#include <esp_log.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app-common/zap-generated/ids/Events.h>
#include <app/clusters/electrical-energy-measurement-server/electrical-energy-measurement-server.h>
#include <app/clusters/electrical-power-measurement-server/electrical-power-measurement-server.h>
#include <app/reporting/reporting.h>
#include <app/data-model/Nullable.h>
#include <system/SystemClock.h>
#include <app/data-model/List.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::DataModel;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ElectricalEnergyMeasurement;
using namespace chip::app::Clusters::ElectricalEnergyMeasurement::Attributes;
using namespace chip::app::Clusters::ElectricalEnergyMeasurement::Structs;
using namespace chip::app::Clusters::ElectricalPowerMeasurement;
using namespace chip::app::Clusters::ElectricalPowerMeasurement::Attributes;
using namespace chip::app::Clusters::ElectricalPowerMeasurement::Structs;

static const char *TAG = "electrical_measurement";

// A pointer to store our attribute access object
static std::unique_ptr<ElectricalEnergyMeasurementAttrAccess> gEEMAttrAccess;

// Global pointer to our ElectricalPowerMeasurementDelegate
static std::unique_ptr<ElectricalPowerMeasurementDelegate> gEPMDelegate;
// Global pointer to our ElectricalPowerMeasurementInstance
static std::unique_ptr<ElectricalPowerMeasurementInstance> gEPMInstance;

// Example of how to create and send energy measurement events
esp_err_t send_energy_measurement_events(uint16_t endpoint_id)
{
    ESP_LOGI(TAG, "Sending energy measurement events examples");
    
    // Example 1: Reporting periodic energy measurement
    ESP_LOGI(TAG, "Example 1: Reporting periodic energy measurement");
    
    // Create energy measurement struct for imported energy (0.5 kWh = 500000 mWh)
    EnergyMeasurementStruct::Type energy_imported = {
        .energy        = 500000,
        .startTimestamp = MakeOptional(static_cast<uint32_t>(0)),
        .endTimestamp   = MakeOptional(static_cast<uint32_t>(0)),
        .startSystime   = MakeOptional(static_cast<uint64_t>(0)),
        .endSystime     = MakeOptional(static_cast<uint64_t>(System::SystemClock().GetMonotonicTimestamp().count()))
    };
    
    // Send the periodic energy measured event (just imported energy)
    Optional<EnergyMeasurementStruct::Type> optional_energy_imported(energy_imported);
    Optional<EnergyMeasurementStruct::Type> optional_energy_exported; // No exported energy
    
    bool success = NotifyPeriodicEnergyMeasured(endpoint_id, optional_energy_imported, optional_energy_exported);
    if (!success) {
        ESP_LOGE(TAG, "Failed to report periodic energy");
        return ESP_FAIL;
    }
    
    // Example 2: Report cumulative energy measurement with both imported and exported energy
    ESP_LOGI(TAG, "Example 2: Reporting cumulative energy measurement");
    
    // Create energy measurement struct for imported energy (10 kWh = 10000000 mWh)
    EnergyMeasurementStruct::Type cumulative_imported = {
        .energy        = 10000000,
        .startTimestamp = MakeOptional(static_cast<uint32_t>(0)),
        .endTimestamp   = MakeOptional(static_cast<uint32_t>(0)),
        .startSystime   = MakeOptional(static_cast<uint64_t>(0)),
        .endSystime     = MakeOptional(static_cast<uint64_t>(System::SystemClock().GetMonotonicTimestamp().count()))
    };
    
    // Create energy measurement struct for exported energy (1 kWh = 1000000 mWh)
    EnergyMeasurementStruct::Type cumulative_exported = {
        .energy        = 1000000,
        .startTimestamp = MakeOptional(static_cast<uint32_t>(0)),
        .endTimestamp   = MakeOptional(static_cast<uint32_t>(0)),
        .startSystime   = MakeOptional(static_cast<uint64_t>(0)),
        .endSystime     = MakeOptional(static_cast<uint64_t>(System::SystemClock().GetMonotonicTimestamp().count()))
    };
    
    // Set the optional values for both imported and exported
    Optional<EnergyMeasurementStruct::Type> optional_cumulative_imported(cumulative_imported);
    Optional<EnergyMeasurementStruct::Type> optional_cumulative_exported(cumulative_exported);
    
    // Send the cumulative energy measured event
    success = NotifyCumulativeEnergyMeasured(endpoint_id, optional_cumulative_imported, optional_cumulative_exported);
    if (!success) {
        ESP_LOGE(TAG, "Failed to report cumulative energy");
        return ESP_FAIL;
    }
    
    ESP_LOGI(TAG, "Energy measurement events sent successfully");
    return ESP_OK;
}

esp_err_t electrical_measurement_example(uint16_t endpoint_id)
{
    ESP_LOGI(TAG, "Initializing Electrical Energy Measurement cluster for endpoint %d", endpoint_id);
    
    // First ensure we don't initialize it twice
    if (gEEMAttrAccess) {
        ESP_LOGI(TAG, "Electrical Energy Measurement cluster already initialized");
        return send_energy_measurement_events(endpoint_id);
    }

    // Create the attribute access instance with the desired features
    gEEMAttrAccess = std::make_unique<ElectricalEnergyMeasurementAttrAccess>(
        BitMask<ElectricalEnergyMeasurement::Feature, uint32_t>(
            ElectricalEnergyMeasurement::Feature::kImportedEnergy, 
            ElectricalEnergyMeasurement::Feature::kExportedEnergy,
            ElectricalEnergyMeasurement::Feature::kCumulativeEnergy, 
            ElectricalEnergyMeasurement::Feature::kPeriodicEnergy),
        BitMask<ElectricalEnergyMeasurement::OptionalAttributes, uint32_t>(
            ElectricalEnergyMeasurement::OptionalAttributes::kOptionalAttributeCumulativeEnergyReset));

    // Create accuracy ranges for energy measurements
    MeasurementAccuracyRangeStruct::Type energyAccuracyRanges[] = {
        {
            .rangeMin   = 0,
            .rangeMax   = 1000000000000000, // 1 million Mwh
            .percentMax = MakeOptional(static_cast<chip::Percent100ths>(500)),
            .percentMin = MakeOptional(static_cast<chip::Percent100ths>(50))
        }
    };

    // Create and populate accuracy struct
    MeasurementAccuracyStruct::Type accuracy = {
        .measurementType  = MeasurementTypeEnum::kElectricalEnergy,
        .measured         = true,
        .minMeasuredValue = 0,
        .maxMeasuredValue = 1000000000000000, // 1 million Mwh
        .accuracyRanges   = chip::app::DataModel::List<const MeasurementAccuracyRangeStruct::Type>(energyAccuracyRanges, 1)
    };

    // Example of setting CumulativeEnergyReset structure
    CumulativeEnergyResetStruct::Type resetStruct = {
        .importedResetTimestamp = MakeOptional(MakeNullable(static_cast<uint32_t>(0))),
        .exportedResetTimestamp = MakeOptional(MakeNullable(static_cast<uint32_t>(0))),
        .importedResetSystime   = MakeOptional(MakeNullable(static_cast<uint64_t>(0))),
        .exportedResetSystime   = MakeOptional(MakeNullable(static_cast<uint64_t>(0)))
    };

    // Initialize and set values if attribute access was created successfully
    if (gEEMAttrAccess) {
        ESP_LOGI(TAG, "Initializing electrical energy measurement attribute access");
        gEEMAttrAccess->Init();

        // Set accuracy and reset information
        CHIP_ERROR err = SetMeasurementAccuracy(endpoint_id, accuracy);
        if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Failed to set measurement accuracy: %" CHIP_ERROR_FORMAT, err.Format());
            return ESP_FAIL;
        }
        
        err = SetCumulativeReset(endpoint_id, MakeOptional(resetStruct));
        if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Failed to set cumulative reset: %" CHIP_ERROR_FORMAT, err.Format());
            return ESP_FAIL;
        }
        
        // After initializing the cluster, send example events
        esp_err_t ret = send_energy_measurement_events(endpoint_id);
        if (ret != ESP_OK) {
            return ret;
        }
        
        ESP_LOGI(TAG, "Electrical Energy Measurement cluster initialized successfully");
    } else {
        ESP_LOGE(TAG, "Failed to create electrical energy measurement attribute access");
        return ESP_FAIL;
    }

    // Initialize Electrical Power Measurement cluster.
    ESP_LOGI(TAG, "Initializing Electrical Power Measurement cluster for endpoint %d", endpoint_id);
    
    if (gEPMDelegate || gEPMInstance) {
        ESP_LOGI(TAG, "EPM Instance or Delegate already exist.");
        return ESP_OK;
    }

    gEPMDelegate = std::make_unique<ElectricalPowerMeasurementDelegate>();
    if (!gEPMDelegate) {
        ESP_LOGE(TAG, "Failed to allocate memory for EPM Delegate");
        return ESP_FAIL;
    }

    gEPMInstance = std::make_unique<ElectricalPowerMeasurementInstance>(
        EndpointId(endpoint_id), *gEPMDelegate,
        BitMask<ElectricalPowerMeasurement::Feature, uint32_t>(
            ElectricalPowerMeasurement::Feature::kDirectCurrent, 
            ElectricalPowerMeasurement::Feature::kAlternatingCurrent),
        BitMask<ElectricalPowerMeasurement::OptionalAttributes, uint32_t>(
            ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeVoltage,
            ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeActiveCurrent
        ));

    if (!gEPMInstance) {
        ESP_LOGE(TAG, "Failed to allocate memory for EPM Instance");
        gEPMDelegate.reset();
        return ESP_FAIL;
    }

    CHIP_ERROR err = gEPMInstance->Init(); // Register Attribute & Command handlers
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Init failed on gEPMInstance: %" CHIP_ERROR_FORMAT, err.Format());
        gEPMInstance.reset();
        gEPMDelegate.reset();
        return ESP_FAIL;
    }
    
    ESP_LOGI(TAG, "Electrical Energy and Power Measurement clusters initialized successfully");
    return ESP_OK;
}

//
// Implementation of ElectricalPowerMeasurementDelegate methods
//
namespace chip {
namespace app {
namespace Clusters {
namespace ElectricalPowerMeasurement {

// Accuracy ranges from reference implementation
const MeasurementAccuracyRangeStruct::Type activePowerAccuracyRanges[] = {
    // 2 - 5%, 3% Typ
    {
        .rangeMin       = -50'000'000, // -50kW
        .rangeMax       = -10'000'000, // -10kW
        .percentMax     = MakeOptional(static_cast<chip::Percent100ths>(5000)),
        .percentMin     = MakeOptional(static_cast<chip::Percent100ths>(2000)),
        .percentTypical = MakeOptional(static_cast<chip::Percent100ths>(3000)),
    },
    // 0.1 - 1%, 0.5% Typ
    {
        .rangeMin       = -9'999'999, // -9.999kW
        .rangeMax       = 9'999'999,  //  9.999kW
        .percentMax     = MakeOptional(static_cast<chip::Percent100ths>(1000)),
        .percentMin     = MakeOptional(static_cast<chip::Percent100ths>(100)),
        .percentTypical = MakeOptional(static_cast<chip::Percent100ths>(500)),
    },
    // 2 - 5%, 3% Typ
    {
        .rangeMin       = 10'000'000, // 10 kW
        .rangeMax       = 50'000'000, // 50 kW
        .percentMax     = MakeOptional(static_cast<chip::Percent100ths>(5000)),
        .percentMin     = MakeOptional(static_cast<chip::Percent100ths>(2000)),
        .percentTypical = MakeOptional(static_cast<chip::Percent100ths>(3000)),
    },
};

const MeasurementAccuracyRangeStruct::Type activeCurrentAccuracyRanges[] = {
    // 2 - 5%, 3% Typ
    {
        .rangeMin       = -100'000, // -100A
        .rangeMax       = -5'000,   // -5A
        .percentMax     = MakeOptional(static_cast<chip::Percent100ths>(5000)),
        .percentMin     = MakeOptional(static_cast<chip::Percent100ths>(2000)),
        .percentTypical = MakeOptional(static_cast<chip::Percent100ths>(3000)),
    },
    // 0.1 - 1%, 0.5% Typ
    {
        .rangeMin       = -4'999, // -4.999A
        .rangeMax       = 4'999,  //  4.999A
        .percentMax     = MakeOptional(static_cast<chip::Percent100ths>(1000)),
        .percentMin     = MakeOptional(static_cast<chip::Percent100ths>(100)),
        .percentTypical = MakeOptional(static_cast<chip::Percent100ths>(500)),
    },
    // 2 - 5%, 3% Typ
    {
        .rangeMin       = 5'000,   // 5A
        .rangeMax       = 100'000, // 100 A
        .percentMax     = MakeOptional(static_cast<chip::Percent100ths>(5000)),
        .percentMin     = MakeOptional(static_cast<chip::Percent100ths>(2000)),
        .percentTypical = MakeOptional(static_cast<chip::Percent100ths>(3000)),
    },
};

const MeasurementAccuracyRangeStruct::Type voltageAccuracyRanges[] = {
    // 2 - 5%, 3% Typ
    {
        .rangeMin       = -500'000, // -500V
        .rangeMax       = -100'000, // -100V
        .percentMax     = MakeOptional(static_cast<chip::Percent100ths>(5000)),
        .percentMin     = MakeOptional(static_cast<chip::Percent100ths>(2000)),
        .percentTypical = MakeOptional(static_cast<chip::Percent100ths>(3000)),
    },
    // 0.1 - 1%, 0.5% Typ
    {
        .rangeMin       = -99'999, // -99.999V
        .rangeMax       = 99'999,  //  99.999V
        .percentMax     = MakeOptional(static_cast<chip::Percent100ths>(1000)),
        .percentMin     = MakeOptional(static_cast<chip::Percent100ths>(100)),
        .percentTypical = MakeOptional(static_cast<chip::Percent100ths>(500)),
    },
    // 2 - 5%, 3% Typ
    {
        .rangeMin       = 100'000, // 100 V
        .rangeMax       = 500'000, // 500 V
        .percentMax     = MakeOptional(static_cast<chip::Percent100ths>(5000)),
        .percentMin     = MakeOptional(static_cast<chip::Percent100ths>(2000)),
        .percentTypical = MakeOptional(static_cast<chip::Percent100ths>(3000)),
    }
};

static const Structs::MeasurementAccuracyStruct::Type kMeasurementAccuracies[] = {
    {
        .measurementType  = MeasurementTypeEnum::kActivePower,
        .measured         = true,
        .minMeasuredValue = -50'000'000, // -50 kW
        .maxMeasuredValue = 50'000'000,  //  50 kW
        .accuracyRanges   = DataModel::List<const MeasurementAccuracyRangeStruct::Type>(activePowerAccuracyRanges),
    },
    {
        .measurementType  = MeasurementTypeEnum::kActiveCurrent,
        .measured         = true,
        .minMeasuredValue = -100'000, // -100A
        .maxMeasuredValue = 100'000,  //  100A
        .accuracyRanges   = DataModel::List<const MeasurementAccuracyRangeStruct::Type>(activeCurrentAccuracyRanges),
    },
    {
        .measurementType  = MeasurementTypeEnum::kVoltage,
        .measured         = true,
        .minMeasuredValue = -500'000, // -500V
        .maxMeasuredValue = 500'000,  //  500V
        .accuracyRanges   = DataModel::List<const MeasurementAccuracyRangeStruct::Type>(voltageAccuracyRanges),
    },
};

static const Structs::HarmonicMeasurementStruct::Type kHarmonicCurrentMeasurements[] = {
    { .order = 1, .measurement = MakeNullable(static_cast<int64_t>(100000)) }
};

static const Structs::HarmonicMeasurementStruct::Type kHarmonicPhaseMeasurements[] = {
    { .order = 1, .measurement = MakeNullable(static_cast<int64_t>(100000)) }
};

uint8_t ElectricalPowerMeasurementDelegate::GetNumberOfMeasurementTypes()
{
    return MATTER_ARRAY_SIZE(kMeasurementAccuracies);
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::StartAccuracyRead()
{
    /* Since we have a static array we don't need to do anything here */
    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::GetAccuracyByIndex(uint8_t index, Structs::MeasurementAccuracyStruct::Type & accuracy)
{
    if (index >= MATTER_ARRAY_SIZE(kMeasurementAccuracies)) {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }
    
    accuracy = kMeasurementAccuracies[index];
    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::EndAccuracyRead()
{
    /* Since we have a static array we don't need to do anything here */
    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::StartRangesRead()
{
    /* Since we don't an implementation here we don't need to do anything here */
    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::GetRangeByIndex(uint8_t index, Structs::MeasurementRangeStruct::Type & range)
{
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::EndRangesRead()
{
    /* Since we don't an implementation here we don't need to do anything here */
    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::StartHarmonicCurrentsRead()
{
    /* Since we have a static array we don't need to do anything here */
    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::GetHarmonicCurrentsByIndex(uint8_t index, Structs::HarmonicMeasurementStruct::Type & harmonics)
{
    if (index >= MATTER_ARRAY_SIZE(kHarmonicCurrentMeasurements)) {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }

    harmonics = kHarmonicCurrentMeasurements[index];
    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::EndHarmonicCurrentsRead()
{
    /* Since we have a static array we don't need to do anything here */
    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::StartHarmonicPhasesRead()
{
    /* Since we have a static array we don't need to do anything here */
    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::GetHarmonicPhasesByIndex(uint8_t index, Structs::HarmonicMeasurementStruct::Type & harmonics)
{
    if (index >= MATTER_ARRAY_SIZE(kHarmonicPhaseMeasurements)) {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }

    harmonics = kHarmonicPhaseMeasurements[index];
    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::EndHarmonicPhasesRead()
{
    /* Since we have a static array we don't need to do anything here */
    return CHIP_NO_ERROR;
}

// Implementation of setter methods with proper change detection and reporting
CHIP_ERROR ElectricalPowerMeasurementDelegate::SetPowerMode(PowerModeEnum newValue)
{
    PowerModeEnum oldValue = mPowerMode;

    if (EnsureKnownEnumValue(newValue) == PowerModeEnum::kUnknownEnumValue)
    {
        return CHIP_IM_GLOBAL_STATUS(ConstraintError);
    }

    mPowerMode = newValue;
    if (oldValue != newValue)
    {
        ChipLogDetail(AppServer, "mPowerMode updated to %d", static_cast<int>(mPowerMode));
        MatterReportingAttributeChangeCallback(mEndpointId, ElectricalPowerMeasurement::Id, PowerMode::Id);
    }

    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::SetVoltage(DataModel::Nullable<int64_t> newValue)
{
    DataModel::Nullable<int64_t> oldValue = mVoltage;

    mVoltage = newValue;
    if (oldValue != newValue)
    {
        // We won't log raw values since these could change frequently
        MatterReportingAttributeChangeCallback(mEndpointId, ElectricalPowerMeasurement::Id, Voltage::Id);
    }

    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::SetActiveCurrent(DataModel::Nullable<int64_t> newValue)
{
    DataModel::Nullable<int64_t> oldValue = mActiveCurrent;

    mActiveCurrent = newValue;
    if (oldValue != newValue)
    {
        // We won't log raw values since these could change frequently
        MatterReportingAttributeChangeCallback(mEndpointId, ElectricalPowerMeasurement::Id, ActiveCurrent::Id);
    }

    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::SetReactiveCurrent(DataModel::Nullable<int64_t> newValue)
{
    DataModel::Nullable<int64_t> oldValue = mReactiveCurrent;

    mReactiveCurrent = newValue;
    if (oldValue != newValue)
    {
        // We won't log raw values since these could change frequently
        MatterReportingAttributeChangeCallback(mEndpointId, ElectricalPowerMeasurement::Id, ReactiveCurrent::Id);
    }

    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::SetApparentCurrent(DataModel::Nullable<int64_t> newValue)
{
    DataModel::Nullable<int64_t> oldValue = mApparentCurrent;

    mApparentCurrent = newValue;
    if (oldValue != newValue)
    {
        // We won't log raw values since these could change frequently
        MatterReportingAttributeChangeCallback(mEndpointId, ElectricalPowerMeasurement::Id, ApparentCurrent::Id);
    }

    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::SetActivePower(DataModel::Nullable<int64_t> newValue)
{
    DataModel::Nullable<int64_t> oldValue = mActivePower;

    mActivePower = newValue;
    if (oldValue != newValue)
    {
        // We won't log raw values since these could change frequently
        MatterReportingAttributeChangeCallback(mEndpointId, ElectricalPowerMeasurement::Id, ActivePower::Id);
    }

    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::SetReactivePower(DataModel::Nullable<int64_t> newValue)
{
    DataModel::Nullable<int64_t> oldValue = mReactivePower;

    mReactivePower = newValue;
    if (oldValue != newValue)
    {
        // We won't log raw values since these could change frequently
        MatterReportingAttributeChangeCallback(mEndpointId, ElectricalPowerMeasurement::Id, ReactivePower::Id);
    }

    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::SetApparentPower(DataModel::Nullable<int64_t> newValue)
{
    DataModel::Nullable<int64_t> oldValue = mApparentPower;

    mApparentPower = newValue;
    if (oldValue != newValue)
    {
        // We won't log raw values since these could change frequently
        MatterReportingAttributeChangeCallback(mEndpointId, ElectricalPowerMeasurement::Id, ApparentPower::Id);
    }

    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::SetRMSVoltage(DataModel::Nullable<int64_t> newValue)
{
    DataModel::Nullable<int64_t> oldValue = mRMSVoltage;

    mRMSVoltage = newValue;
    if (oldValue != newValue)
    {
        // We won't log raw values since these could change frequently
        MatterReportingAttributeChangeCallback(mEndpointId, ElectricalPowerMeasurement::Id, RMSVoltage::Id);
    }

    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::SetRMSCurrent(DataModel::Nullable<int64_t> newValue)
{
    DataModel::Nullable<int64_t> oldValue = mRMSCurrent;

    mRMSCurrent = newValue;
    if (oldValue != newValue)
    {
        // We won't log raw values since these could change frequently
        MatterReportingAttributeChangeCallback(mEndpointId, ElectricalPowerMeasurement::Id, RMSCurrent::Id);
    }

    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::SetRMSPower(DataModel::Nullable<int64_t> newValue)
{
    DataModel::Nullable<int64_t> oldValue = mRMSPower;

    mRMSPower = newValue;
    if (oldValue != newValue)
    {
        // We won't log raw values since these could change frequently
        MatterReportingAttributeChangeCallback(mEndpointId, ElectricalPowerMeasurement::Id, RMSPower::Id);
    }

    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::SetFrequency(DataModel::Nullable<int64_t> newValue)
{
    DataModel::Nullable<int64_t> oldValue = mFrequency;

    mFrequency = newValue;
    if (oldValue != newValue)
    {
        // We won't log raw values since these could change frequently
        MatterReportingAttributeChangeCallback(mEndpointId, ElectricalPowerMeasurement::Id, Frequency::Id);
    }

    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::SetPowerFactor(DataModel::Nullable<int64_t> newValue)
{
    DataModel::Nullable<int64_t> oldValue = mPowerFactor;

    mPowerFactor = newValue;
    if (oldValue != newValue)
    {
        // We won't log raw values since these could change frequently
        MatterReportingAttributeChangeCallback(mEndpointId, ElectricalPowerMeasurement::Id, PowerFactor::Id);
    }

    return CHIP_NO_ERROR;
}

CHIP_ERROR ElectricalPowerMeasurementDelegate::SetNeutralCurrent(DataModel::Nullable<int64_t> newValue)
{
    DataModel::Nullable<int64_t> oldValue = mNeutralCurrent;

    mNeutralCurrent = newValue;
    if (oldValue != newValue)
    {
        // We won't log raw values since these could change frequently
        MatterReportingAttributeChangeCallback(mEndpointId, ElectricalPowerMeasurement::Id, NeutralCurrent::Id);
    }

    return CHIP_NO_ERROR;
}

// Implementation of ElectricalPowerMeasurementInstance methods
CHIP_ERROR ElectricalPowerMeasurementInstance::Init()
{
    return Instance::Init();
}

void ElectricalPowerMeasurementInstance::Shutdown()
{
    Instance::Shutdown();
}

} // namespace ElectricalPowerMeasurement
} // namespace Clusters
} // namespace app
} // namespace chip
