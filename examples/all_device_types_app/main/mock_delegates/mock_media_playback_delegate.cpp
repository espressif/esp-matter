/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_media_playback_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace MediaPlayback {

PlaybackStateEnum MockMediaPlaybackDelegate::HandleGetCurrentState()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return PlaybackStateEnum::kPlaying;
}

uint64_t MockMediaPlaybackDelegate::HandleGetStartTime()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

uint64_t MockMediaPlaybackDelegate::HandleGetDuration()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

CHIP_ERROR MockMediaPlaybackDelegate::HandleGetSampledPosition(app::AttributeValueEncoder  &aEncoder)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return aEncoder.EncodeNull();
}

float MockMediaPlaybackDelegate::HandleGetPlaybackSpeed()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 1.0f;
}

uint64_t MockMediaPlaybackDelegate::HandleGetSeekRangeStart()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

uint64_t MockMediaPlaybackDelegate::HandleGetSeekRangeEnd()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

CHIP_ERROR MockMediaPlaybackDelegate::HandleGetActiveAudioTrack(app::AttributeValueEncoder  &aEncoder)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return aEncoder.EncodeNull();
}

CHIP_ERROR MockMediaPlaybackDelegate::HandleGetAvailableAudioTracks(app::AttributeValueEncoder  &aEncoder)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return aEncoder.EncodeEmptyList();
}

CHIP_ERROR MockMediaPlaybackDelegate::HandleGetActiveTextTrack(app::AttributeValueEncoder  &aEncoder)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return aEncoder.EncodeNull();
}

CHIP_ERROR MockMediaPlaybackDelegate::HandleGetAvailableTextTracks(app::AttributeValueEncoder  &aEncoder)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return aEncoder.EncodeEmptyList();
}

void MockMediaPlaybackDelegate::HandlePlay(CommandResponseHelper<Commands::PlaybackResponse::Type>  &helper)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    Commands::PlaybackResponse::Type response;
    response.status = MediaPlaybackStatusEnum::kSuccess;
    helper.Success(response);
}

void MockMediaPlaybackDelegate::HandlePause(CommandResponseHelper<Commands::PlaybackResponse::Type>  &helper)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    Commands::PlaybackResponse::Type response;
    response.status = MediaPlaybackStatusEnum::kSuccess;
    helper.Success(response);
}

void MockMediaPlaybackDelegate::HandleStop(CommandResponseHelper<Commands::PlaybackResponse::Type>  &helper)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    Commands::PlaybackResponse::Type response;
    response.status = MediaPlaybackStatusEnum::kSuccess;
    helper.Success(response);
}

void MockMediaPlaybackDelegate::HandleFastForward(CommandResponseHelper<Commands::PlaybackResponse::Type>  &helper,
                                                  const chip::Optional<bool>  &audioAdvanceUnmuted)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    Commands::PlaybackResponse::Type response;
    response.status = MediaPlaybackStatusEnum::kSuccess;
    helper.Success(response);
}

void MockMediaPlaybackDelegate::HandlePrevious(CommandResponseHelper<Commands::PlaybackResponse::Type>  &helper)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    Commands::PlaybackResponse::Type response;
    response.status = MediaPlaybackStatusEnum::kSuccess;
    helper.Success(response);
}

void MockMediaPlaybackDelegate::HandleRewind(CommandResponseHelper<Commands::PlaybackResponse::Type>  &helper,
                                             const chip::Optional<bool>  &audioAdvanceUnmuted)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    Commands::PlaybackResponse::Type response;
    response.status = MediaPlaybackStatusEnum::kSuccess;
    helper.Success(response);
}

void MockMediaPlaybackDelegate::HandleSkipBackward(CommandResponseHelper<Commands::PlaybackResponse::Type>  &helper,
                                                   const uint64_t  &deltaPositionMilliseconds)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    Commands::PlaybackResponse::Type response;
    response.status = MediaPlaybackStatusEnum::kSuccess;
    helper.Success(response);
}

void MockMediaPlaybackDelegate::HandleSkipForward(CommandResponseHelper<Commands::PlaybackResponse::Type>  &helper,
                                                  const uint64_t  &deltaPositionMilliseconds)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    Commands::PlaybackResponse::Type response;
    response.status = MediaPlaybackStatusEnum::kSuccess;
    helper.Success(response);
}

void MockMediaPlaybackDelegate::HandleSeek(CommandResponseHelper<Commands::PlaybackResponse::Type>  &helper,
                                           const uint64_t  &positionMilliseconds)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    Commands::PlaybackResponse::Type response;
    response.status = MediaPlaybackStatusEnum::kSuccess;
    helper.Success(response);
}

void MockMediaPlaybackDelegate::HandleNext(CommandResponseHelper<Commands::PlaybackResponse::Type>  &helper)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    Commands::PlaybackResponse::Type response;
    response.status = MediaPlaybackStatusEnum::kSuccess;
    helper.Success(response);
}

void MockMediaPlaybackDelegate::HandleStartOver(CommandResponseHelper<Commands::PlaybackResponse::Type>  &helper)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    Commands::PlaybackResponse::Type response;
    response.status = MediaPlaybackStatusEnum::kSuccess;
    helper.Success(response);
}

bool MockMediaPlaybackDelegate::HandleActivateAudioTrack(const chip::CharSpan  &trackId, const uint8_t  &audioOutputIndex)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

bool MockMediaPlaybackDelegate::HandleActivateTextTrack(const chip::CharSpan  &trackId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

bool MockMediaPlaybackDelegate::HandleDeactivateTextTrack()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

uint32_t MockMediaPlaybackDelegate::GetFeatureMap(chip::EndpointId endpoint)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

uint16_t MockMediaPlaybackDelegate::GetClusterRevision(chip::EndpointId endpoint)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

} // namespace MediaPlayback
} // namespace Clusters
} // namespace app
} // namespace chip