/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_microwave_oven_control_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace MicrowaveOvenControl {

Protocols::InteractionModel::Status MockMicrowaveOvenControlDelegate::HandleSetCookingParametersCallback(uint8_t cookMode, uint32_t cookTimeSec,
                                                                                                         bool startAfterSetting,
                                                                                                         Optional<uint8_t> powerSettingNum,
                                                                                                         Optional<uint8_t> wattSettingIndex)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockMicrowaveOvenControlDelegate::HandleModifyCookTimeSecondsCallback(uint32_t finalCookTimeSec)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

CHIP_ERROR MockMicrowaveOvenControlDelegate::GetWattSettingByIndex(uint8_t index, uint16_t  &wattSetting)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_NOT_FOUND;
}

uint32_t MockMicrowaveOvenControlDelegate::GetMaxCookTimeSec() const
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

uint8_t MockMicrowaveOvenControlDelegate::GetPowerSettingNum() const
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

uint8_t MockMicrowaveOvenControlDelegate::GetMinPowerNum() const
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

uint8_t MockMicrowaveOvenControlDelegate::GetMaxPowerNum() const
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

uint8_t MockMicrowaveOvenControlDelegate::GetPowerStepNum() const
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

uint8_t MockMicrowaveOvenControlDelegate::GetCurrentWattIndex() const
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

uint16_t MockMicrowaveOvenControlDelegate::GetWattRating() const
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

} // namespace MicrowaveOvenControl
} // namespace Clusters
} // namespace app
} // namespace chip