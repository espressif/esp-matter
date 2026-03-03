/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/camera-av-settings-user-level-management-server/CameraAvSettingsUserLevelManagementCluster.h>
#include <protocols/interaction_model/StatusCode.h>

/*
 * Mock CameraAVSettingsUserLevelManagement Delegate Implementation
 * This file provides a mock implementation of the CameraAVSettingsUserLevelManagement::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/camera-av-settings-user-level-management-server/camera-av-settings-user-level-management-server.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/camera-av-settings-user-level-management-instance.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/src/camera-av-settings-user-level-management-stub.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace CameraAvSettingsUserLevelManagement {

class MockCameraAVSettingsUserLevelManagementDelegate : public CameraAvSettingsUserLevelManagementDelegate {
public:
    MockCameraAVSettingsUserLevelManagementDelegate() : CameraAvSettingsUserLevelManagementDelegate() {}
    virtual ~MockCameraAVSettingsUserLevelManagementDelegate() = default;

    // Shutdown handler
    void ShutdownApp() override;

    // Camera AV settings management
    bool CanChangeMPTZ() override;

    // DPTZ stream handling
    void VideoStreamAllocated(uint16_t aStreamID) override;
    void VideoStreamDeallocated(uint16_t aStreamID) override;
    void DefaultViewportUpdated(Globals::Structs::ViewportStruct::Type aViewport) override;

    // MPTZ command handlers
    Protocols::InteractionModel::Status MPTZSetPosition(Optional<int16_t> aPan, Optional<int16_t> aTilt,
                                                        Optional<uint8_t> aZoom, PhysicalPTZCallback * callback) override;
    Protocols::InteractionModel::Status MPTZRelativeMove(Optional<int16_t> aPan, Optional<int16_t> aTilt,
                                                         Optional<uint8_t> aZoom, PhysicalPTZCallback * callback) override;
    Protocols::InteractionModel::Status MPTZMoveToPreset(uint8_t aPreset, Optional<int16_t> aPan, Optional<int16_t> aTilt,
                                                         Optional<uint8_t> aZoom, PhysicalPTZCallback * callback) override;
    Protocols::InteractionModel::Status MPTZSavePreset(uint8_t aPreset) override;
    Protocols::InteractionModel::Status MPTZRemovePreset(uint8_t aPreset) override;

    // DPTZ command handlers
    Protocols::InteractionModel::Status DPTZSetViewport(uint16_t aVideoStreamID,
                                                        Globals::Structs::ViewportStruct::Type aViewport) override;
    Protocols::InteractionModel::Status DPTZRelativeMove(uint16_t aVideoStreamID, Optional<int16_t> aDeltaX,
                                                         Optional<int16_t> aDeltaY, Optional<int8_t> aZoomDelta,
                                                         Globals::Structs::ViewportStruct::Type  &aViewport) override;

    // Persistent attributes callback
    CHIP_ERROR PersistentAttributesLoadedCallback() override;

    // Load handlers
    CHIP_ERROR LoadMPTZPresets(std::vector<MPTZPresetHelper>  &mptzPresetHelpers) override;
    CHIP_ERROR LoadDPTZStreams(std::vector<DPTZStruct>  &dptzStreams) override;

private:
    static constexpr const char * LOG_TAG = "MockCameraAVSettingsUserLevelMgmtDelegate";
};

} // namespace CameraAvSettingsUserLevelManagement
} // namespace Clusters
} // namespace app
} // namespace chip
