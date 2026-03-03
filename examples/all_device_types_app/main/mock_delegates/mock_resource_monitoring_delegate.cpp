/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_resource_monitoring_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace ResourceMonitoring {

CHIP_ERROR MockResourceMonitoringDelegate::Init()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

Protocols::InteractionModel::Status MockResourceMonitoringDelegate::OnResetCondition()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockResourceMonitoringDelegate::PreResetCondition()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockResourceMonitoringDelegate::PostResetCondition()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

} // namespace ResourceMonitoring
} // namespace Clusters
} // namespace app
} // namespace chip