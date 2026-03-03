/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_device_energy_management_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace DeviceEnergyManagement {

Protocols::InteractionModel::Status MockDeviceEnergyManagementDelegate::PowerAdjustRequest(const int64_t power, const uint32_t duration,
                                                                                           AdjustmentCauseEnum cause)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockDeviceEnergyManagementDelegate::CancelPowerAdjustRequest()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockDeviceEnergyManagementDelegate::StartTimeAdjustRequest(const uint32_t requestedStartTime,
                                                                                               AdjustmentCauseEnum cause)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockDeviceEnergyManagementDelegate::PauseRequest(const uint32_t duration, AdjustmentCauseEnum cause)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockDeviceEnergyManagementDelegate::ResumeRequest()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status
MockDeviceEnergyManagementDelegate::ModifyForecastRequest(const uint32_t forecastID,
                                                          const DataModel::DecodableList<Structs::SlotAdjustmentStruct::Type>  &slotAdjustments,
                                                          AdjustmentCauseEnum cause)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status
MockDeviceEnergyManagementDelegate::RequestConstraintBasedForecast(const DataModel::DecodableList<Structs::ConstraintsStruct::Type>  &constraints,
                                                                   AdjustmentCauseEnum cause)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockDeviceEnergyManagementDelegate::CancelRequest()
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

ESATypeEnum MockDeviceEnergyManagementDelegate::GetESAType()
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return ESATypeEnum::kOther;
}

bool MockDeviceEnergyManagementDelegate::GetESACanGenerate()
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return false;
}

ESAStateEnum MockDeviceEnergyManagementDelegate::GetESAState()
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return ESAStateEnum::kOffline;
}

int64_t MockDeviceEnergyManagementDelegate::GetAbsMinPower()
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

int64_t MockDeviceEnergyManagementDelegate::GetAbsMaxPower()
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

OptOutStateEnum MockDeviceEnergyManagementDelegate::GetOptOutState()
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return OptOutStateEnum::kNoOptOut;
}

const DataModel::Nullable<Structs::PowerAdjustCapabilityStruct::Type>  &MockDeviceEnergyManagementDelegate::GetPowerAdjustmentCapability()
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return mPowerAdjustCapability;
}

const DataModel::Nullable<Structs::ForecastStruct::Type>  &MockDeviceEnergyManagementDelegate::GetForecast()
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return mForecast;
}

CHIP_ERROR MockDeviceEnergyManagementDelegate::SetESAState(ESAStateEnum esaState)
{
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

} // namespace DeviceEnergyManagement
} // namespace Clusters
} // namespace app
} // namespace chip