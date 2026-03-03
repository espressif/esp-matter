/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_boolean_state_configuration_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace BooleanStateConfiguration {

CHIP_ERROR MockBooleanStateConfigurationDelegate::HandleSuppressAlarm(BooleanStateConfiguration::AlarmModeBitmap alarmToSuppress)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockBooleanStateConfigurationDelegate::HandleEnableDisableAlarms(chip::BitMask<BooleanStateConfiguration::AlarmModeBitmap> alarms)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

} // namespace BooleanStateConfiguration
} // namespace Clusters
} // namespace app
} // namespace chip