/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_target_navigator_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace TargetNavigator {

CHIP_ERROR MockTargetNavigatorDelegate::HandleGetTargetList(app::AttributeValueEncoder  &aEncoder)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return aEncoder.EncodeEmptyList();
}

uint8_t MockTargetNavigatorDelegate::HandleGetCurrentTarget()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

void MockTargetNavigatorDelegate::HandleNavigateTarget(CommandResponseHelper<Commands::NavigateTargetResponse::Type>  &helper,
                                                       const uint64_t  &target, const CharSpan  &data)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    Commands::NavigateTargetResponse::Type response;
    response.status = TargetNavigatorStatusEnum::kSuccess;
    helper.Success(response);
}

uint16_t MockTargetNavigatorDelegate::GetClusterRevision(chip::EndpointId endpoint)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

} // namespace TargetNavigator
} // namespace Clusters
} // namespace app
} // namespace chip