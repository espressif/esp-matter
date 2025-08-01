/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_fan_control_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace FanControl {

Protocols::InteractionModel::Status MockFanControlDelegate::HandleStep(StepDirectionEnum aDirection, bool aWrap, bool aLowestOff)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

} // namespace FanControl
} // namespace Clusters
} // namespace app
} // namespace chip