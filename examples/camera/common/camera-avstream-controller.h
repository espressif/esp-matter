/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/camera-av-stream-management-server/CameraAVStreamManagementCluster.h>

namespace chip {
namespace app {
namespace Clusters {
namespace CameraAvStreamManagement {

/**
 * The application interface to define the options & implement commands.
 */
class CameraAVStreamController {
public:
    virtual ~CameraAVStreamController() = default;

    virtual CHIP_ERROR ValidateStreamUsage(StreamUsageEnum streamUsage, Optional<std::vector<uint16_t>>  &videoStreams,
                                           Optional<std::vector<uint16_t>>  &audioStreams) = 0;

    virtual CHIP_ERROR ValidateVideoStreamID(uint16_t videoStreamId) = 0;

    virtual CHIP_ERROR ValidateAudioStreamID(uint16_t audioStreamId) = 0;

    virtual CHIP_ERROR ValidateVideoStreams(const std::vector<uint16_t>  &videoStreams) = 0;

    virtual CHIP_ERROR ValidateAudioStreams(const std::vector<uint16_t>  &audioStreams) = 0;

    virtual CHIP_ERROR IsHardPrivacyModeActive(bool &isActive) = 0;

    virtual CHIP_ERROR IsSoftRecordingPrivacyModeActive(bool &isActive) = 0;

    virtual CHIP_ERROR IsSoftLivestreamPrivacyModeActive(bool &isActive) = 0;

    virtual CHIP_ERROR SetHardPrivacyModeOn(bool hardPrivacyMode) = 0;

    virtual bool HasAllocatedVideoStreams() = 0;

    virtual bool HasAllocatedAudioStreams() = 0;

    /**
     * @brief Calculates the total bandwidth in bps for the given video and audio
     * stream IDs. It iterates through the allocated video and audio streams, and
     * if a matching stream ID is found, its bit rate (maxBitRate for video,
     * bitRate for audio) is added to the total bandwidth.
     *
     * @param videoStreamId Optional nullable ID of the video stream.
     * @param audioStreamId Optional nullable ID of the audio stream.
     * @param outBandwidthbps Output parameter for the calculated total bandwidth
     * in bps.
     */
    virtual void GetBandwidthForStreams(
        const Optional<DataModel::Nullable<uint16_t>> &videoStreamId,
        const Optional<DataModel::Nullable<uint16_t>> &audioStreamId,
        uint32_t &outBandwidthbps) = 0;

    /**
     * @brief Called by transports when they start using the corresponding audio and video streams.
     *
     */
    virtual CHIP_ERROR OnTransportAcquireAudioVideoStreams(const std::vector<uint16_t>  &audioStreams,
                                                           const std::vector<uint16_t>  &videoStreams) = 0;

    /**
    * @brief Called by transports when they release the corresponding audio and video streams.
    *
    */
    virtual CHIP_ERROR OnTransportReleaseAudioVideoStreams(const std::vector<uint16_t>  &audioStreams,
                                                           const std::vector<uint16_t>  &videoStreams) = 0;
};

} // namespace CameraAvStreamManagement
} // namespace Clusters
} // namespace app
} // namespace chip
