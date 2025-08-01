/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_laundry_dryer_controls_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace LaundryDryerControls {

CHIP_ERROR MockLaundryDryerControlsDelegate::GetSupportedDrynessLevelAtIndex(size_t index, DrynessLevelEnum  &supportedDryness)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

} // namespace LaundryDryerControls
} // namespace Clusters
} // namespace app
} // namespace chip