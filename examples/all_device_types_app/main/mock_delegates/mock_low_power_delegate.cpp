/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_low_power_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace LowPower {

bool MockLowPowerDelegate::HandleSleep()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

} // namespace LowPower
} // namespace Clusters
} // namespace app
} // namespace chip