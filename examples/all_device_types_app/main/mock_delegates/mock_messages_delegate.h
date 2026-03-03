/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/messages-server/messages-delegate.h>

/*
 * Mock Messages Delegate Implementation
 * This file provides a mock implementation of the Messages::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/messages-server/messages-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/messages/MessagesManager.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/messages/MessagesManager.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace Messages {

class MockMessagesDelegate : public Delegate {
public:
    MockMessagesDelegate() = default;

    CHIP_ERROR HandlePresentMessagesRequest(
        const ByteSpan  &messageId, const MessagePriorityEnum  &priority,
        const chip::BitMask<MessageControlBitmap>  &messageControl, const DataModel::Nullable<uint32_t>  &startTime,
        const DataModel::Nullable<uint64_t>  &duration, const CharSpan  &messageText,
        const chip::Optional<DataModel::DecodableList<chip::app::Clusters::Messages::Structs::MessageResponseOptionStruct::Type>> &
        responses) override;
    CHIP_ERROR HandleCancelMessagesRequest(const DataModel::DecodableList<chip::ByteSpan>  &messageIds) override;

    CHIP_ERROR HandleGetMessages(app::AttributeValueEncoder  &aEncoder) override;
    CHIP_ERROR HandleGetActiveMessageIds(app::AttributeValueEncoder  &aEncoder) override;

    uint32_t GetFeatureMap(chip::EndpointId endpoint) override;

private:
    const char *LOG_TAG = "messages";
};

} // namespace Messages
} // namespace Clusters
} // namespace app
} // namespace chip