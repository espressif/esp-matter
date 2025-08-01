/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app-common/zap-generated/cluster-objects.h>

#include <app/clusters/channel-server/channel-delegate.h>

/*
 * Mock Channel Delegate Implementation
 * This file provides a mock implementation of the Channel::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/channel-server/channel-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/channel/ChannelManager.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/channel/ChannelManager.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace Channel {

class MockChannelDelegate : public Delegate {
public:
    MockChannelDelegate() = default;

    CHIP_ERROR HandleGetChannelList(app::AttributeValueEncoder  &aEncoder) override;
    CHIP_ERROR HandleGetLineup(app::AttributeValueEncoder  &aEncoder) override;
    CHIP_ERROR HandleGetCurrentChannel(app::AttributeValueEncoder  &aEncoder) override;

    void HandleChangeChannel(CommandResponseHelper<Commands::ChangeChannelResponse::Type>  &helper,
                             const chip::CharSpan  &match) override;
    bool HandleChangeChannelByNumber(const uint16_t  &majorNumber, const uint16_t  &minorNumber) override;
    bool HandleSkipChannel(const int16_t  &count) override;
    void HandleGetProgramGuide(CommandResponseHelper<Commands::ProgramGuideResponse::Type>  &helper,
                               const chip::Optional<uint32_t>  &startTime, const chip::Optional<uint32_t>  &endTime,
                               const chip::Optional<DataModel::DecodableList<ChannelInfo>>  &channelList,
                               const chip::Optional<PageToken>  &pageToken,
                               const chip::Optional<chip::BitMask<RecordingFlagBitmap>>  &recordingFlag,
                               const chip::Optional<DataModel::DecodableList<AdditionalInfo>>  &externalIdList,
                               const chip::Optional<chip::ByteSpan>  &data) override;

    bool HandleRecordProgram(const chip::CharSpan  &programIdentifier, bool shouldRecordSeries,
                             const DataModel::DecodableList<AdditionalInfo>  &externalIdList,
                             const chip::ByteSpan  &data) override;

    bool HandleCancelRecordProgram(const chip::CharSpan  &programIdentifier, bool shouldRecordSeries,
                                   const DataModel::DecodableList<AdditionalInfo>  &externalIdList,
                                   const chip::ByteSpan  &data) override;

    uint32_t GetFeatureMap(chip::EndpointId endpoint) override;
    uint16_t GetClusterRevision(chip::EndpointId endpoint) override;

private:
    const char *LOG_TAG = "channel";
};

} // namespace Channel
} // namespace Clusters
} // namespace app
} // namespace chip