/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_laundry_washer_controls_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace LaundryWasherControls {

CHIP_ERROR MockLaundryWasherControlsDelegate::GetSpinSpeedAtIndex(size_t index, MutableCharSpan  &spinSpeed)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

CHIP_ERROR MockLaundryWasherControlsDelegate::GetSupportedRinseAtIndex(size_t index, NumberOfRinsesEnum  &supportedRinse)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

} // namespace LaundryWasherControls
} // namespace Clusters
} // namespace app
} // namespace chip