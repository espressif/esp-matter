/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_camera_av_stream_management_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace CameraAvStreamManagement {

Protocols::InteractionModel::Status MockCameraAVStreamManagementDelegate::VideoStreamAllocate(const VideoStreamStruct  &allocateArgs,
                                                                                              uint16_t  &outStreamID)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    outStreamID = 0;
    return Protocols::InteractionModel::Status::Success;
}

void MockCameraAVStreamManagementDelegate::OnVideoStreamAllocated(const VideoStreamStruct  &allocatedStream, StreamAllocationAction action)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

Protocols::InteractionModel::Status MockCameraAVStreamManagementDelegate::VideoStreamModify(const uint16_t streamID, const Optional<bool> waterMarkEnabled,
                                                                                            const Optional<bool> osdEnabled)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockCameraAVStreamManagementDelegate::VideoStreamDeallocate(const uint16_t streamID)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockCameraAVStreamManagementDelegate::AudioStreamAllocate(const AudioStreamStruct  &allocateArgs,
                                                                                              uint16_t  &outStreamID)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    outStreamID = 0;
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockCameraAVStreamManagementDelegate::AudioStreamDeallocate(const uint16_t streamID)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockCameraAVStreamManagementDelegate::SnapshotStreamAllocate(const SnapshotStreamAllocateArgs  &allocateArgs,
                                                                                                 uint16_t  &outStreamID)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    outStreamID = 0;
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockCameraAVStreamManagementDelegate::SnapshotStreamModify(const uint16_t streamID, const Optional<bool> waterMarkEnabled,
                                                                                               const Optional<bool> osdEnabled)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockCameraAVStreamManagementDelegate::SnapshotStreamDeallocate(const uint16_t streamID)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

void MockCameraAVStreamManagementDelegate::OnStreamUsagePrioritiesChanged()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

void MockCameraAVStreamManagementDelegate::OnAttributeChanged(AttributeId attributeId)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

Protocols::InteractionModel::Status MockCameraAVStreamManagementDelegate::CaptureSnapshot(const DataModel::Nullable<uint16_t> streamID,
                                                                                          const VideoResolutionStruct  &resolution,
                                                                                          ImageSnapshot  &outImageSnapshot)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

CHIP_ERROR MockCameraAVStreamManagementDelegate::PersistentAttributesLoadedCallback()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockCameraAVStreamManagementDelegate::OnTransportAcquireAudioVideoStreams(uint16_t audioStreamID, uint16_t videoStreamID)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockCameraAVStreamManagementDelegate::OnTransportReleaseAudioVideoStreams(uint16_t audioStreamID, uint16_t videoStreamID)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

const std::vector<VideoStreamStruct>  &MockCameraAVStreamManagementDelegate::GetAllocatedVideoStreams() const
{
    return mAllocatedVideoStreams;
}

const std::vector<AudioStreamStruct>  &MockCameraAVStreamManagementDelegate::GetAllocatedAudioStreams() const
{
    return mAllocatedAudioStreams;
}

} // namespace CameraAvStreamManagement
} // namespace Clusters
} // namespace app
} // namespace chip
