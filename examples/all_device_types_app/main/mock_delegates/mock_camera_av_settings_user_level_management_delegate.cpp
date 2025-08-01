/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_camera_av_settings_user_level_management_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace CameraAvSettingsUserLevelManagement {

void MockCameraAVSettingsUserLevelManagementDelegate::ShutdownApp()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

bool MockCameraAVSettingsUserLevelManagementDelegate::CanChangeMPTZ()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

void MockCameraAVSettingsUserLevelManagementDelegate::VideoStreamAllocated(uint16_t aStreamID)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

void MockCameraAVSettingsUserLevelManagementDelegate::VideoStreamDeallocated(uint16_t aStreamID)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

void MockCameraAVSettingsUserLevelManagementDelegate::DefaultViewportUpdated(Globals::Structs::ViewportStruct::Type aViewport)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

Protocols::InteractionModel::Status MockCameraAVSettingsUserLevelManagementDelegate::MPTZSetPosition(Optional<int16_t> aPan, Optional<int16_t> aTilt,
                                                                                                     Optional<uint8_t> aZoom, PhysicalPTZCallback * callback)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    if (callback) {
        callback->OnPhysicalMovementComplete(Protocols::InteractionModel::Status::Success);
    }
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockCameraAVSettingsUserLevelManagementDelegate::MPTZRelativeMove(Optional<int16_t> aPan, Optional<int16_t> aTilt,
                                                                                                      Optional<uint8_t> aZoom, PhysicalPTZCallback * callback)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    if (callback) {
        callback->OnPhysicalMovementComplete(Protocols::InteractionModel::Status::Success);
    }
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockCameraAVSettingsUserLevelManagementDelegate::MPTZMoveToPreset(uint8_t aPreset, Optional<int16_t> aPan, Optional<int16_t> aTilt,
                                                                                                      Optional<uint8_t> aZoom, PhysicalPTZCallback * callback)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    if (callback) {
        callback->OnPhysicalMovementComplete(Protocols::InteractionModel::Status::Success);
    }
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockCameraAVSettingsUserLevelManagementDelegate::MPTZSavePreset(uint8_t aPreset)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockCameraAVSettingsUserLevelManagementDelegate::MPTZRemovePreset(uint8_t aPreset)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockCameraAVSettingsUserLevelManagementDelegate::DPTZSetViewport(uint16_t aVideoStreamID,
                                                                                                     Globals::Structs::ViewportStruct::Type aViewport)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockCameraAVSettingsUserLevelManagementDelegate::DPTZRelativeMove(uint16_t aVideoStreamID, Optional<int16_t> aDeltaX,
                                                                                                      Optional<int16_t> aDeltaY, Optional<int8_t> aZoomDelta,
                                                                                                      Globals::Structs::ViewportStruct::Type  &aViewport)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

CHIP_ERROR MockCameraAVSettingsUserLevelManagementDelegate::PersistentAttributesLoadedCallback()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockCameraAVSettingsUserLevelManagementDelegate::LoadMPTZPresets(std::vector<MPTZPresetHelper>  &mptzPresetHelpers)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    mptzPresetHelpers.clear();
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockCameraAVSettingsUserLevelManagementDelegate::LoadDPTZStreams(std::vector<DPTZStruct>  &dptzStreams)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    dptzStreams.clear();
    return CHIP_NO_ERROR;
}

} // namespace CameraAvSettingsUserLevelManagement
} // namespace Clusters
} // namespace app
} // namespace chip
