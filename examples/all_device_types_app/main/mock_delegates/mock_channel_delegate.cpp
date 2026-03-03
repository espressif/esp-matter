/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_channel_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace Channel {

CHIP_ERROR MockChannelDelegate::HandleGetChannelList(app::AttributeValueEncoder  &aEncoder)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return aEncoder.EncodeEmptyList();
}

CHIP_ERROR MockChannelDelegate::HandleGetLineup(app::AttributeValueEncoder  &aEncoder)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return aEncoder.EncodeNull();
}

CHIP_ERROR MockChannelDelegate::HandleGetCurrentChannel(app::AttributeValueEncoder  &aEncoder)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return aEncoder.EncodeNull();
}

void MockChannelDelegate::HandleChangeChannel(CommandResponseHelper<Commands::ChangeChannelResponse::Type>  &helper,
                                              const chip::CharSpan  &match)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    Commands::ChangeChannelResponse::Type response;
    response.status = ChannelStatusEnum::kSuccess;
    helper.Success(response);
}

bool MockChannelDelegate::HandleChangeChannelByNumber(const uint16_t  &majorNumber, const uint16_t  &minorNumber)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

bool MockChannelDelegate::HandleSkipChannel(const int16_t  &count)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

void MockChannelDelegate::HandleGetProgramGuide(CommandResponseHelper<Commands::ProgramGuideResponse::Type>  &helper,
                                                const chip::Optional<uint32_t>  &startTime, const chip::Optional<uint32_t>  &endTime,
                                                const chip::Optional<DataModel::DecodableList<ChannelInfo>>  &channelList,
                                                const chip::Optional<PageToken>  &pageToken,
                                                const chip::Optional<chip::BitMask<RecordingFlagBitmap>>  &recordingFlag,
                                                const chip::Optional<DataModel::DecodableList<AdditionalInfo>>  &externalIdList,
                                                const chip::Optional<chip::ByteSpan>  &data)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    Commands::ProgramGuideResponse::Type response;
    helper.Success(response);
}

bool MockChannelDelegate::HandleRecordProgram(const chip::CharSpan  &programIdentifier, bool shouldRecordSeries,
                                              const DataModel::DecodableList<AdditionalInfo>  &externalIdList,
                                              const chip::ByteSpan  &data)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

bool MockChannelDelegate::HandleCancelRecordProgram(const chip::CharSpan  &programIdentifier, bool shouldRecordSeries,
                                                    const DataModel::DecodableList<AdditionalInfo>  &externalIdList,
                                                    const chip::ByteSpan  &data)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

uint32_t MockChannelDelegate::GetFeatureMap(chip::EndpointId endpoint)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

uint16_t MockChannelDelegate::GetClusterRevision(chip::EndpointId endpoint)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

} // namespace Channel
} // namespace Clusters
} // namespace app
} // namespace chip