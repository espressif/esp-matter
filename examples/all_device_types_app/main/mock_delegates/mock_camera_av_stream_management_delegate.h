/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/camera-av-stream-management-server/CameraAVStreamManagementCluster.h>
#include <protocols/interaction_model/StatusCode.h>

/*
 * Mock CameraAVStreamManagement Delegate Implementation
 * This file provides a mock implementation of the CameraAVStreamMgmtDelegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/camera-av-stream-management-server/camera-av-stream-management-server.h
 */

namespace chip {
namespace app {
namespace Clusters {
namespace CameraAvStreamManagement {

class MockCameraAVStreamManagementDelegate : public CameraAVStreamManagementDelegate {
public:
    MockCameraAVStreamManagementDelegate() = default;
    virtual ~MockCameraAVStreamManagementDelegate() = default;

    // Video stream handlers
    Protocols::InteractionModel::Status VideoStreamAllocate(const VideoStreamStruct  &allocateArgs,
                                                            uint16_t  &outStreamID) override;
    void OnVideoStreamAllocated(const VideoStreamStruct  &allocatedStream, StreamAllocationAction action) override;
    Protocols::InteractionModel::Status VideoStreamModify(const uint16_t streamID, const Optional<bool> waterMarkEnabled,
                                                          const Optional<bool> osdEnabled) override;
    Protocols::InteractionModel::Status VideoStreamDeallocate(const uint16_t streamID) override;

    // Audio stream handlers
    Protocols::InteractionModel::Status AudioStreamAllocate(const AudioStreamStruct  &allocateArgs,
                                                            uint16_t  &outStreamID) override;
    Protocols::InteractionModel::Status AudioStreamDeallocate(const uint16_t streamID) override;

    // Snapshot stream handlers
    Protocols::InteractionModel::Status SnapshotStreamAllocate(const SnapshotStreamAllocateArgs  &allocateArgs,
                                                               uint16_t  &outStreamID) override;
    Protocols::InteractionModel::Status SnapshotStreamModify(const uint16_t streamID, const Optional<bool> waterMarkEnabled,
                                                             const Optional<bool> osdEnabled) override;
    Protocols::InteractionModel::Status SnapshotStreamDeallocate(const uint16_t streamID) override;

    // Callback handlers
    void OnStreamUsagePrioritiesChanged() override;
    void OnAttributeChanged(AttributeId attributeId) override;

    // Capture snapshot
    Protocols::InteractionModel::Status CaptureSnapshot(const DataModel::Nullable<uint16_t> streamID,
                                                        const VideoResolutionStruct  &resolution,
                                                        ImageSnapshot  &outImageSnapshot) override;

    // Persistent attributes callback
    CHIP_ERROR PersistentAttributesLoadedCallback() override;

    // Transport stream handlers
    CHIP_ERROR OnTransportAcquireAudioVideoStreams(uint16_t audioStreamID, uint16_t videoStreamID) override;
    CHIP_ERROR OnTransportReleaseAudioVideoStreams(uint16_t audioStreamID, uint16_t videoStreamID) override;

    // Stream accessors
    const std::vector<VideoStreamStruct>  &GetAllocatedVideoStreams() const override;
    const std::vector<AudioStreamStruct>  &GetAllocatedAudioStreams() const override;

private:
    static constexpr const char * LOG_TAG = "MockCameraAVStreamMgmtDelegate";
    std::vector<VideoStreamStruct> mAllocatedVideoStreams;
    std::vector<AudioStreamStruct> mAllocatedAudioStreams;
};

} // namespace CameraAvStreamManagement
} // namespace Clusters
} // namespace app
} // namespace chip
