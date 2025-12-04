/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_chime_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace Chime {

MockChimeDelegate::~MockChimeDelegate() = default;

CHIP_ERROR MockChimeDelegate::GetChimeSoundByIndex(uint8_t chimeIndex, uint8_t & chimeID, MutableCharSpan & name)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockChimeDelegate::GetChimeIDByIndex(uint8_t chimeIndex, uint8_t & chimeID)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

Protocols::InteractionModel::Status MockChimeDelegate::PlayChimeSound()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

} // namespace Chime
} // namespace Clusters
} // namespace app
} // namespace chip
