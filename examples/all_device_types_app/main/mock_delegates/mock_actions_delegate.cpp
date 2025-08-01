/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_actions_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace Actions {

CHIP_ERROR MockActionsDelegate::ReadActionAtIndex(uint16_t index, ActionStructStorage  &action)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

CHIP_ERROR MockActionsDelegate::ReadEndpointListAtIndex(uint16_t index, EndpointListStorage  &epList)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

bool MockActionsDelegate::HaveActionWithId(uint16_t aActionId, uint16_t  &aActionIndex)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return false;
}

Protocols::InteractionModel::Status MockActionsDelegate::HandleInstantAction(uint16_t actionId, Optional<uint32_t> invokeId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockActionsDelegate::HandleInstantActionWithTransition(uint16_t actionId, uint16_t transitionTime,
                                                                                           Optional<uint32_t> invokeId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockActionsDelegate::HandleStartAction(uint16_t actionId, Optional<uint32_t> invokeId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockActionsDelegate::HandleStartActionWithDuration(uint16_t actionId, uint32_t duration,
                                                                                       Optional<uint32_t> invokeId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockActionsDelegate::HandleStopAction(uint16_t actionId, Optional<uint32_t> invokeId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockActionsDelegate::HandlePauseAction(uint16_t actionId, Optional<uint32_t> invokeId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockActionsDelegate::HandlePauseActionWithDuration(uint16_t actionId, uint32_t duration,
                                                                                       Optional<uint32_t> invokeId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockActionsDelegate::HandleResumeAction(uint16_t actionId, Optional<uint32_t> invokeId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockActionsDelegate::HandleEnableAction(uint16_t actionId, Optional<uint32_t> invokeId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockActionsDelegate::HandleEnableActionWithDuration(uint16_t actionId, uint32_t duration,
                                                                                        Optional<uint32_t> invokeId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockActionsDelegate::HandleDisableAction(uint16_t actionId, Optional<uint32_t> invokeId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockActionsDelegate::HandleDisableActionWithDuration(uint16_t actionId, uint32_t duration,
                                                                                         Optional<uint32_t> invokeId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

} // namespace Actions
} // namespace Clusters
} // namespace app
} // namespace chip