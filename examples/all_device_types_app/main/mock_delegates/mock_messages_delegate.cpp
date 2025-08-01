/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_messages_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace Messages {

CHIP_ERROR MockMessagesDelegate::HandlePresentMessagesRequest(
    const ByteSpan  &messageId, const MessagePriorityEnum  &priority,
    const chip::BitMask<MessageControlBitmap>  &messageControl, const DataModel::Nullable<uint32_t>  &startTime,
    const DataModel::Nullable<uint64_t>  &duration, const CharSpan  &messageText,
    const chip::Optional<DataModel::DecodableList<chip::app::Clusters::Messages::Structs::MessageResponseOptionStruct::Type>> &
    responses)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockMessagesDelegate::HandleCancelMessagesRequest(const DataModel::DecodableList<chip::ByteSpan>  &messageIds)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockMessagesDelegate::HandleGetMessages(app::AttributeValueEncoder  &aEncoder)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return aEncoder.EncodeEmptyList();
}

CHIP_ERROR MockMessagesDelegate::HandleGetActiveMessageIds(app::AttributeValueEncoder  &aEncoder)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return aEncoder.EncodeEmptyList();
}

uint32_t MockMessagesDelegate::GetFeatureMap(chip::EndpointId endpoint)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

} // namespace Messages
} // namespace Clusters
} // namespace app
} // namespace chip