/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_mode_base_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace ModeBase {

CHIP_ERROR MockModeBaseDelegate::Init()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockModeBaseDelegate::GetModeLabelByIndex(uint8_t modeIndex, MutableCharSpan  &label)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

CHIP_ERROR MockModeBaseDelegate::GetModeValueByIndex(uint8_t modeIndex, uint8_t  &value)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

CHIP_ERROR MockModeBaseDelegate::GetModeTagsByIndex(uint8_t modeIndex, DataModel::List<detail::Structs::ModeTagStruct::Type>  &modeTags)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

void MockModeBaseDelegate::HandleChangeToMode(uint8_t NewMode, ModeBase::Commands::ChangeToModeResponse::Type  &response)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    response.status = to_underlying(StatusCode::kSuccess);
    return;
}

} // namespace ModeBase
} // namespace Clusters
} // namespace app
} // namespace chip
