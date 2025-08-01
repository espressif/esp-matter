/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/media-playback-server/media-playback-delegate.h>

/*
 * Mock MediaPlayback Delegate Implementation
 * This file provides a mock implementation of the MediaPlayback::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/media-playback-server/media-playback-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/media-playback/MediaPlaybackManager.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/media-playback/MediaPlaybackManager.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace MediaPlayback {

class MockMediaPlaybackDelegate : public Delegate {
public:
    MockMediaPlaybackDelegate() = default;

    PlaybackStateEnum HandleGetCurrentState() override;
    uint64_t HandleGetStartTime() override;
    uint64_t HandleGetDuration() override;
    CHIP_ERROR HandleGetSampledPosition(app::AttributeValueEncoder  &aEncoder) override;
    float HandleGetPlaybackSpeed() override;
    uint64_t HandleGetSeekRangeStart() override;
    uint64_t HandleGetSeekRangeEnd() override;
    CHIP_ERROR HandleGetActiveAudioTrack(app::AttributeValueEncoder  &aEncoder) override;
    CHIP_ERROR HandleGetAvailableAudioTracks(app::AttributeValueEncoder  &aEncoder) override;
    CHIP_ERROR HandleGetActiveTextTrack(app::AttributeValueEncoder  &aEncoder) override;
    CHIP_ERROR HandleGetAvailableTextTracks(app::AttributeValueEncoder  &aEncoder) override;

    void HandlePlay(CommandResponseHelper<Commands::PlaybackResponse::Type>  &helper) override;
    void HandlePause(CommandResponseHelper<Commands::PlaybackResponse::Type>  &helper) override;
    void HandleStop(CommandResponseHelper<Commands::PlaybackResponse::Type>  &helper) override;
    void HandleFastForward(CommandResponseHelper<Commands::PlaybackResponse::Type>  &helper,
                           const chip::Optional<bool>  &audioAdvanceUnmuted) override;
    void HandlePrevious(CommandResponseHelper<Commands::PlaybackResponse::Type>  &helper) override;
    void HandleRewind(CommandResponseHelper<Commands::PlaybackResponse::Type>  &helper,
                      const chip::Optional<bool>  &audioAdvanceUnmuted) override;
    void HandleSkipBackward(CommandResponseHelper<Commands::PlaybackResponse::Type>  &helper,
                            const uint64_t  &deltaPositionMilliseconds) override;
    void HandleSkipForward(CommandResponseHelper<Commands::PlaybackResponse::Type>  &helper,
                           const uint64_t  &deltaPositionMilliseconds) override;
    void HandleSeek(CommandResponseHelper<Commands::PlaybackResponse::Type>  &helper,
                    const uint64_t  &positionMilliseconds) override;
    void HandleNext(CommandResponseHelper<Commands::PlaybackResponse::Type>  &helper) override;
    void HandleStartOver(CommandResponseHelper<Commands::PlaybackResponse::Type>  &helper) override;
    bool HandleActivateAudioTrack(const chip::CharSpan  &trackId, const uint8_t  &audioOutputIndex) override;
    bool HandleActivateTextTrack(const chip::CharSpan  &trackId) override;
    bool HandleDeactivateTextTrack() override;

    uint32_t GetFeatureMap(chip::EndpointId endpoint) override;
    uint16_t GetClusterRevision(chip::EndpointId endpoint) override;

private:
    const char *LOG_TAG = "media_playback";
};

} // namespace MediaPlayback
} // namespace Clusters
} // namespace app
} // namespace chip