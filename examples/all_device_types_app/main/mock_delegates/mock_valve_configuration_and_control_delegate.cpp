/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_valve_configuration_and_control_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace ValveConfigurationAndControl {

DataModel::Nullable<chip::Percent> MockValveConfigurationAndControlDelegate::HandleOpenValve(DataModel::Nullable<chip::Percent> level)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<chip::Percent>();
}

CHIP_ERROR MockValveConfigurationAndControlDelegate::HandleCloseValve()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

void MockValveConfigurationAndControlDelegate::HandleRemainingDurationTick(uint32_t duration)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

} // namespace ValveConfigurationAndControl
} // namespace Clusters
} // namespace app
} // namespace chip