/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_audio_output_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace AudioOutput {

uint8_t MockAudioOutputDelegate::HandleGetCurrentOutput()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

CHIP_ERROR MockAudioOutputDelegate::HandleGetOutputList(app::AttributeValueEncoder  &aEncoder)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return aEncoder.EncodeEmptyList();
}

bool MockAudioOutputDelegate::HandleRenameOutput(const uint8_t  &index, const chip::CharSpan  &name)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

bool MockAudioOutputDelegate::HandleSelectOutput(const uint8_t  &index)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

} // namespace AudioOutput
} // namespace Clusters
} // namespace app
} // namespace chip