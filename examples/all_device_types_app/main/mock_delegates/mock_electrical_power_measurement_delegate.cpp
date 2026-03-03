/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_electrical_power_measurement_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace ElectricalPowerMeasurement {

PowerModeEnum MockElectricalPowerMeasurementDelegate::GetPowerMode()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return PowerModeEnum::kUnknownEnumValue;
}

uint8_t MockElectricalPowerMeasurementDelegate::GetNumberOfMeasurementTypes()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

CHIP_ERROR MockElectricalPowerMeasurementDelegate::StartAccuracyRead()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockElectricalPowerMeasurementDelegate::GetAccuracyByIndex(uint8_t index, Structs::MeasurementAccuracyStruct::Type  &accuracy)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

CHIP_ERROR MockElectricalPowerMeasurementDelegate::EndAccuracyRead()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockElectricalPowerMeasurementDelegate::StartRangesRead()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockElectricalPowerMeasurementDelegate::GetRangeByIndex(uint8_t index, Structs::MeasurementRangeStruct::Type  &range)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

CHIP_ERROR MockElectricalPowerMeasurementDelegate::EndRangesRead()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockElectricalPowerMeasurementDelegate::StartHarmonicCurrentsRead()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockElectricalPowerMeasurementDelegate::GetHarmonicCurrentsByIndex(uint8_t index, Structs::HarmonicMeasurementStruct::Type  &harmonic)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

CHIP_ERROR MockElectricalPowerMeasurementDelegate::EndHarmonicCurrentsRead()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockElectricalPowerMeasurementDelegate::StartHarmonicPhasesRead()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockElectricalPowerMeasurementDelegate::GetHarmonicPhasesByIndex(uint8_t index, Structs::HarmonicMeasurementStruct::Type  &harmonic)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

CHIP_ERROR MockElectricalPowerMeasurementDelegate::EndHarmonicPhasesRead()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

DataModel::Nullable<int64_t> MockElectricalPowerMeasurementDelegate::GetVoltage()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<int64_t>();
}

DataModel::Nullable<int64_t> MockElectricalPowerMeasurementDelegate::GetActiveCurrent()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<int64_t>();
}

DataModel::Nullable<int64_t> MockElectricalPowerMeasurementDelegate::GetReactiveCurrent()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<int64_t>();
}

DataModel::Nullable<int64_t> MockElectricalPowerMeasurementDelegate::GetApparentCurrent()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<int64_t>();
}

DataModel::Nullable<int64_t> MockElectricalPowerMeasurementDelegate::GetActivePower()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<int64_t>();
}

DataModel::Nullable<int64_t> MockElectricalPowerMeasurementDelegate::GetReactivePower()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<int64_t>();
}

DataModel::Nullable<int64_t> MockElectricalPowerMeasurementDelegate::GetApparentPower()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<int64_t>();
}

DataModel::Nullable<int64_t> MockElectricalPowerMeasurementDelegate::GetRMSVoltage()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<int64_t>();
}

DataModel::Nullable<int64_t> MockElectricalPowerMeasurementDelegate::GetRMSCurrent()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<int64_t>();
}

DataModel::Nullable<int64_t> MockElectricalPowerMeasurementDelegate::GetRMSPower()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<int64_t>();
}

DataModel::Nullable<int64_t> MockElectricalPowerMeasurementDelegate::GetFrequency()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<int64_t>();
}

DataModel::Nullable<int64_t> MockElectricalPowerMeasurementDelegate::GetPowerFactor()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<int64_t>();
}

DataModel::Nullable<int64_t> MockElectricalPowerMeasurementDelegate::GetNeutralCurrent()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<int64_t>();
}

} // namespace ElectricalPowerMeasurement
} // namespace Clusters
} // namespace app
} // namespace chip