// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "camera-app.h"
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <esp_matter.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::Chime;
using namespace chip::app::Clusters::WebRTCTransportProvider;
using namespace chip::app::Clusters::CameraAvStreamManagement;
using namespace esp_matter;
using namespace esp_matter::cluster;

static constexpr uint32_t kBitsPerMegabit = 1000000;
extern uint16_t camera_endpoint_id;
template <typename T>
using List   = chip::app::DataModel::List<T>;
using Status = Protocols::InteractionModel::Status;

CameraApp::CameraApp(chip::EndpointId aClustersEndpoint, CameraDeviceInterface * aCameraDevice)
{
    mEndpoint     = aClustersEndpoint;
    mCameraDevice = aCameraDevice;
}

CHIP_ERROR CameraApp::InitializeCameraAVStreamMgmt()
{
    CameraAvStreamManagement::CameraAVStreamManagementCluster * avsm_server = CameraAvStreamManagement::GetServer(camera_endpoint_id);
    if (avsm_server == nullptr) {
        ChipLogError(Camera, "CameraAVStreamManagementCluster not found for endpoint %u", camera_endpoint_id);
        return CHIP_ERROR_INCORRECT_STATE;
    }

    // Set the attribute defaults
    if (mCameraDevice->GetCameraHALInterface().GetCameraSupportsHDR()) {
        ReturnErrorOnFailure(avsm_server->SetHDRModeEnabled(mCameraDevice->GetCameraHALInterface().GetHDRMode()));
    }

    if (mCameraDevice->GetCameraHALInterface().GetCameraSupportsSoftPrivacy()) {
        ReturnErrorOnFailure(avsm_server->SetSoftRecordingPrivacyModeEnabled(
                                 mCameraDevice->GetCameraHALInterface().GetSoftRecordingPrivacyModeEnabled()));
        ReturnErrorOnFailure(avsm_server->SetSoftLivestreamPrivacyModeEnabled(
                                 mCameraDevice->GetCameraHALInterface().GetSoftLivestreamPrivacyModeEnabled()));
    }

    if (mCameraDevice->GetCameraHALInterface().HasHardPrivacySwitch()) {
        ReturnErrorOnFailure(
            avsm_server->SetHardPrivacyModeOn(mCameraDevice->GetCameraHALInterface().GetHardPrivacyMode()));
    }

    if (mCameraDevice->GetCameraHALInterface().GetCameraSupportsNightVision()) {
        ReturnErrorOnFailure(avsm_server->SetNightVision(mCameraDevice->GetCameraHALInterface().GetNightVision()));
    }

    ReturnErrorOnFailure(avsm_server->SetViewport(mCameraDevice->GetCameraHALInterface().GetViewport()));

    if (mCameraDevice->GetCameraHALInterface().HasSpeaker()) {
        ReturnErrorOnFailure(
            avsm_server->SetSpeakerMuted(mCameraDevice->GetCameraHALInterface().GetSpeakerMuted()));
        ReturnErrorOnFailure(
            avsm_server->SetSpeakerVolumeLevel(mCameraDevice->GetCameraHALInterface().GetSpeakerVolume()));
        ReturnErrorOnFailure(
            avsm_server->SetSpeakerMaxLevel(mCameraDevice->GetCameraHALInterface().GetSpeakerMaxLevel()));
        ReturnErrorOnFailure(
            avsm_server->SetSpeakerMinLevel(mCameraDevice->GetCameraHALInterface().GetSpeakerMinLevel()));
    }

    if (mCameraDevice->GetCameraHALInterface().HasMicrophone()) {
        ReturnErrorOnFailure(
            avsm_server->SetMicrophoneMuted(mCameraDevice->GetCameraHALInterface().GetMicrophoneMuted()));
        ReturnErrorOnFailure(
            avsm_server->SetMicrophoneVolumeLevel(mCameraDevice->GetCameraHALInterface().GetMicrophoneVolume()));
        ReturnErrorOnFailure(
            avsm_server->SetMicrophoneMaxLevel(mCameraDevice->GetCameraHALInterface().GetMicrophoneMaxLevel()));
        ReturnErrorOnFailure(
            avsm_server->SetMicrophoneMinLevel(mCameraDevice->GetCameraHALInterface().GetMicrophoneMinLevel()));
    }

    // Video and Snapshot features are already enabled.
    if (mCameraDevice->GetCameraHALInterface().HasLocalStorage()) {
        ReturnErrorOnFailure(avsm_server->SetLocalVideoRecordingEnabled(
                                 mCameraDevice->GetCameraHALInterface().GetLocalVideoRecordingEnabled()));
        ReturnErrorOnFailure(avsm_server->SetLocalSnapshotRecordingEnabled(
                                 mCameraDevice->GetCameraHALInterface().GetLocalSnapshotRecordingEnabled()));
    }

    if (mCameraDevice->GetCameraHALInterface().HasStatusLight()) {
        ReturnErrorOnFailure(
            avsm_server->SetStatusLightEnabled(mCameraDevice->GetCameraHALInterface().GetStatusLightEnabled()));
    }

    if (mCameraDevice->GetCameraHALInterface().GetCameraSupportsImageControl()) {
        ReturnErrorOnFailure(
            avsm_server->SetImageRotation(mCameraDevice->GetCameraHALInterface().GetImageRotation()));
        ReturnErrorOnFailure(
            avsm_server->SetImageFlipVertical(mCameraDevice->GetCameraHALInterface().GetImageFlipVertical()));
        ReturnErrorOnFailure(
            avsm_server->SetImageFlipHorizontal(mCameraDevice->GetCameraHALInterface().GetImageFlipHorizontal()));
    }

    return avsm_server->Init();
}

void CameraApp::CreateAndInitializeCameraAVStreamMgmt(CameraAvStreamManagement::CameraAvStreamManagementConfig  &config)
{

    cluster_t * avsm_cluster = cluster::get(camera_endpoint_id, CameraAvStreamManagement::Id);
    if (avsm_cluster == nullptr) {
        ChipLogError(Camera, "CameraAVStreamManagement cluster not found for endpoint %u", camera_endpoint_id);
        return;
    }

    // Enable the Watermark and OSD features if camera supports
    if (mCameraDevice->GetCameraHALInterface().GetCameraSupportsWatermark()) {
        camera_av_stream_management::feature::watermark::add(avsm_cluster);
    }

    if (mCameraDevice->GetCameraHALInterface().GetCameraSupportsOSD()) {
        camera_av_stream_management::feature::on_screen_display::add(avsm_cluster);
    }

    if (mCameraDevice->GetCameraHALInterface().GetCameraSupportsSoftPrivacy()) {
        camera_av_stream_management::feature::privacy::add(avsm_cluster);
    }

    // Check microphone support to set Audio feature
    if (mCameraDevice->GetCameraHALInterface().HasMicrophone()) {
        camera_av_stream_management::feature::audio::add(avsm_cluster);
        camera_av_stream_management::attribute::create_microphone_agc_enabled(avsm_cluster, true);

    }

    if (mCameraDevice->GetCameraHALInterface().HasLocalStorage()) {
        camera_av_stream_management::feature::local_storage::add(avsm_cluster);
    }

    // Check if camera has speaker
    if (mCameraDevice->GetCameraHALInterface().HasSpeaker()) {
        camera_av_stream_management::feature::speaker::add(avsm_cluster);
    }

    if (mCameraDevice->GetCameraHALInterface().GetCameraSupportsHDR()) {
        camera_av_stream_management::feature::high_dynamic_range::add(avsm_cluster);
    }

    if (mCameraDevice->GetCameraHALInterface().GetCameraSupportsNightVision()) {
        camera_av_stream_management::feature::night_vision::add(avsm_cluster);
        camera_av_stream_management::attribute::create_night_vision_illum(avsm_cluster, to_underlying(TriStateAutoEnum::kOn));
    }

    if (mCameraDevice->GetCameraHALInterface().HasHardPrivacySwitch()) {
        camera_av_stream_management::attribute::create_hard_privacy_mode_on(avsm_cluster, true);
    }

    if (mCameraDevice->GetCameraHALInterface().HasStatusLight()) {
        camera_av_stream_management::attribute::create_status_light_enabled(avsm_cluster, true);
        camera_av_stream_management::attribute::create_status_light_brightness(avsm_cluster, 100);
    }

    if (mCameraDevice->GetCameraHALInterface().GetCameraSupportsImageControl()) {
        camera_av_stream_management::feature::image_control::add(avsm_cluster);
        camera_av_stream_management::attribute::create_image_flip_vertical(avsm_cluster, true);
        camera_av_stream_management::attribute::create_image_flip_horizontal(avsm_cluster, true);
        camera_av_stream_management::attribute::create_image_rotation(avsm_cluster, 180);
    }

    config.maxConcurrentEncoders  = mCameraDevice->GetCameraHALInterface().GetMaxConcurrentEncoders();
    config.maxEncodedPixelRate         = mCameraDevice->GetCameraHALInterface().GetMaxEncodedPixelRate();
    config.videoSensorParams = mCameraDevice->GetCameraHALInterface().GetVideoSensorParams();
    config.nightVisionUsesInfrared = mCameraDevice->GetCameraHALInterface().GetNightVisionUsesInfrared();
    config.minViewPortRes = mCameraDevice->GetCameraHALInterface().GetMinViewport();
    config.rateDistortionTradeOffPoints = mCameraDevice->GetCameraHALInterface().GetRateDistortionTradeOffPoints();
    config.maxContentBufferSize = mCameraDevice->GetCameraHALInterface().GetMaxContentBufferSize();
    config.microphoneCapabilities = mCameraDevice->GetCameraHALInterface().GetMicrophoneCapabilities();
    config.speakerCapabilities = mCameraDevice->GetCameraHALInterface().GetSpeakerCapabilities();
    config.twoWayTalkSupport = mCameraDevice->GetCameraHALInterface().HasMicrophone() && mCameraDevice->GetCameraHALInterface().HasSpeaker()
                               ? TwoWayTalkSupportTypeEnum::kFullDuplex
                               : TwoWayTalkSupportTypeEnum::kNotSupported;
    config.snapshotCapabilities = mCameraDevice->GetCameraHALInterface().GetSnapshotCapabilities();
    config.maxNetworkBandwidth = mCameraDevice->GetCameraHALInterface().GetMaxNetworkBandwidth() * kBitsPerMegabit;
    config.supportedStreamUsages = mCameraDevice->GetCameraHALInterface().GetSupportedStreamUsages();
    config.streamUsagePriorities = mCameraDevice->GetCameraHALInterface().GetStreamUsagePriorities();
}

void CameraApp::InitCameraDeviceClusters(CameraAvStreamManagement::CameraAvStreamManagementConfig  &config)
{
    // Initialize Cluster Servers
    CreateAndInitializeCameraAVStreamMgmt(config);
}

void CameraApp::ShutdownCameraDeviceClusters()
{
    ChipLogDetail(Camera, "CameraAppShutdown: Shutting down Camera device clusters");
}

std::unique_ptr<CameraApp> gCameraApp;

void CameraAppInit(CameraDeviceInterface * cameraDevice, CameraAvStreamManagement::CameraAvStreamManagementConfig  &config)
{

    gCameraApp = std::make_unique<CameraApp>(camera_endpoint_id, cameraDevice);

    gCameraApp.get()->InitCameraDeviceClusters(config);

    ChipLogDetail(Camera, "CameraAppInit: Initialized Camera clusters");
}

void CameraAppShutdown()
{
    ChipLogDetail(Camera, "CameraAppShutdown: Shutting down Camera app");
    gCameraApp.get()->ShutdownCameraDeviceClusters();
    gCameraApp = nullptr;
}