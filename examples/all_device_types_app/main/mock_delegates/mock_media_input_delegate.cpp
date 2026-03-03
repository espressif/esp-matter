/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_media_input_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace MediaInput {

CHIP_ERROR MockMediaInputDelegate::HandleGetInputList(app::AttributeValueEncoder  &aEncoder)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return aEncoder.EncodeEmptyList();
}

uint8_t MockMediaInputDelegate::HandleGetCurrentInput()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

bool MockMediaInputDelegate::HandleSelectInput(const uint8_t index)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

bool MockMediaInputDelegate::HandleShowInputStatus()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

bool MockMediaInputDelegate::HandleHideInputStatus()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

bool MockMediaInputDelegate::HandleRenameInput(const uint8_t index, const chip::CharSpan  &name)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

} // namespace MediaInput
} // namespace Clusters
} // namespace app
} // namespace chip