/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_water_heater_management_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace WaterHeaterManagement {

Protocols::InteractionModel::Status MockWaterHeaterManagementDelegate::HandleBoost(uint32_t duration, Optional<bool> oneShot,
                                                                                   Optional<bool> emergencyBoost, Optional<int16_t> temporarySetpoint,
                                                                                   Optional<Percent> targetPercentage, Optional<Percent> targetReheat)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockWaterHeaterManagementDelegate::HandleCancelBoost()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

BitMask<WaterHeaterHeatSourceBitmap> MockWaterHeaterManagementDelegate::GetHeaterTypes()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return BitMask<WaterHeaterHeatSourceBitmap>();
}

BitMask<WaterHeaterHeatSourceBitmap> MockWaterHeaterManagementDelegate::GetHeatDemand()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return BitMask<WaterHeaterHeatSourceBitmap>();
}

uint16_t MockWaterHeaterManagementDelegate::GetTankVolume()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

Energy_mWh MockWaterHeaterManagementDelegate::GetEstimatedHeatRequired()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Energy_mWh(0);
}

Percent MockWaterHeaterManagementDelegate::GetTankPercentage()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Percent(0);
}

BoostStateEnum MockWaterHeaterManagementDelegate::GetBoostState()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return BoostStateEnum::kUnknownEnumValue;
}

} // namespace WaterHeaterManagement
} // namespace Clusters
} // namespace app
} // namespace chip