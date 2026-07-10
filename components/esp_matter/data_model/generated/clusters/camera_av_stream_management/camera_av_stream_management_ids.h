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

/* THIS IS A GENERATED FILE, DO NOT EDIT */

#pragma once
#include <esp_matter_data_model.h>

namespace esp_matter {
namespace cluster {
namespace camera_av_stream_management {

inline constexpr uint32_t Id = 0x0551;

namespace feature {
namespace Audio {
inline constexpr uint32_t Id = 0x1;
} /* Audio */
namespace Video {
inline constexpr uint32_t Id = 0x2;
} /* Video */
namespace Snapshot {
inline constexpr uint32_t Id = 0x4;
} /* Snapshot */
namespace Privacy {
inline constexpr uint32_t Id = 0x8;
} /* Privacy */
namespace Speaker {
inline constexpr uint32_t Id = 0x10;
} /* Speaker */
namespace ImageControl {
inline constexpr uint32_t Id = 0x20;
} /* ImageControl */
namespace Watermark {
inline constexpr uint32_t Id = 0x40;
} /* Watermark */
namespace OnScreenDisplay {
inline constexpr uint32_t Id = 0x80;
} /* OnScreenDisplay */
namespace LocalStorage {
inline constexpr uint32_t Id = 0x100;
} /* LocalStorage */
namespace HighDynamicRange {
inline constexpr uint32_t Id = 0x200;
} /* HighDynamicRange */
namespace NightVision {
inline constexpr uint32_t Id = 0x400;
} /* NightVision */
} /* feature */

namespace attribute {
namespace MaxConcurrentEncoders {
inline constexpr uint32_t Id = 0x0000;
} /* MaxConcurrentEncoders */
namespace MaxEncodedPixelRate {
inline constexpr uint32_t Id = 0x0001;
} /* MaxEncodedPixelRate */
namespace VideoSensorParams {
inline constexpr uint32_t Id = 0x0002;
} /* VideoSensorParams */
namespace NightVisionUsesInfrared {
inline constexpr uint32_t Id = 0x0003;
} /* NightVisionUsesInfrared */
namespace MinViewportResolution {
inline constexpr uint32_t Id = 0x0004;
} /* MinViewportResolution */
namespace RateDistortionTradeOffPoints {
inline constexpr uint32_t Id = 0x0005;
} /* RateDistortionTradeOffPoints */
namespace MaxContentBufferSize {
inline constexpr uint32_t Id = 0x0006;
} /* MaxContentBufferSize */
namespace MicrophoneCapabilities {
inline constexpr uint32_t Id = 0x0007;
} /* MicrophoneCapabilities */
namespace SpeakerCapabilities {
inline constexpr uint32_t Id = 0x0008;
} /* SpeakerCapabilities */
namespace TwoWayTalkSupport {
inline constexpr uint32_t Id = 0x0009;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* TwoWayTalkSupport */
namespace SnapshotCapabilities {
inline constexpr uint32_t Id = 0x000A;
} /* SnapshotCapabilities */
namespace MaxNetworkBandwidth {
inline constexpr uint32_t Id = 0x000B;
} /* MaxNetworkBandwidth */
namespace CurrentFrameRate {
inline constexpr uint32_t Id = 0x000C;
} /* CurrentFrameRate */
namespace HDRModeEnabled {
inline constexpr uint32_t Id = 0x000D;
} /* HDRModeEnabled */
namespace SupportedStreamUsages {
inline constexpr uint32_t Id = 0x000E;
} /* SupportedStreamUsages */
namespace AllocatedVideoStreams {
inline constexpr uint32_t Id = 0x000F;
} /* AllocatedVideoStreams */
namespace AllocatedAudioStreams {
inline constexpr uint32_t Id = 0x0010;
} /* AllocatedAudioStreams */
namespace AllocatedSnapshotStreams {
inline constexpr uint32_t Id = 0x0011;
} /* AllocatedSnapshotStreams */
namespace StreamUsagePriorities {
inline constexpr uint32_t Id = 0x0012;
} /* StreamUsagePriorities */
namespace SoftRecordingPrivacyModeEnabled {
inline constexpr uint32_t Id = 0x0013;
} /* SoftRecordingPrivacyModeEnabled */
namespace SoftLivestreamPrivacyModeEnabled {
inline constexpr uint32_t Id = 0x0014;
} /* SoftLivestreamPrivacyModeEnabled */
namespace HardPrivacyModeOn {
inline constexpr uint32_t Id = 0x0015;
} /* HardPrivacyModeOn */
namespace NightVision {
inline constexpr uint32_t Id = 0x0016;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* NightVision */
namespace NightVisionIllum {
inline constexpr uint32_t Id = 0x0017;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 2;
} /* NightVisionIllum */
namespace Viewport {
inline constexpr uint32_t Id = 0x0018;
} /* Viewport */
namespace SpeakerMuted {
inline constexpr uint32_t Id = 0x0019;
} /* SpeakerMuted */
namespace SpeakerVolumeLevel {
inline constexpr uint32_t Id = 0x001A;
} /* SpeakerVolumeLevel */
namespace SpeakerMaxLevel {
inline constexpr uint32_t Id = 0x001B;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 254;
} /* SpeakerMaxLevel */
namespace SpeakerMinLevel {
inline constexpr uint32_t Id = 0x001C;
} /* SpeakerMinLevel */
namespace MicrophoneMuted {
inline constexpr uint32_t Id = 0x001D;
} /* MicrophoneMuted */
namespace MicrophoneVolumeLevel {
inline constexpr uint32_t Id = 0x001E;
} /* MicrophoneVolumeLevel */
namespace MicrophoneMaxLevel {
inline constexpr uint32_t Id = 0x001F;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 254;
} /* MicrophoneMaxLevel */
namespace MicrophoneMinLevel {
inline constexpr uint32_t Id = 0x0020;
} /* MicrophoneMinLevel */
namespace MicrophoneAGCEnabled {
inline constexpr uint32_t Id = 0x0021;
} /* MicrophoneAGCEnabled */
namespace ImageRotation {
inline constexpr uint32_t Id = 0x0022;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 359;
} /* ImageRotation */
namespace ImageFlipHorizontal {
inline constexpr uint32_t Id = 0x0023;
} /* ImageFlipHorizontal */
namespace ImageFlipVertical {
inline constexpr uint32_t Id = 0x0024;
} /* ImageFlipVertical */
namespace LocalVideoRecordingEnabled {
inline constexpr uint32_t Id = 0x0025;
} /* LocalVideoRecordingEnabled */
namespace LocalSnapshotRecordingEnabled {
inline constexpr uint32_t Id = 0x0026;
} /* LocalSnapshotRecordingEnabled */
namespace StatusLightEnabled {
inline constexpr uint32_t Id = 0x0027;
} /* StatusLightEnabled */
namespace StatusLightBrightness {
inline constexpr uint32_t Id = 0x0028;
} /* StatusLightBrightness */
} /* attribute */

namespace command {
namespace AudioStreamAllocate {
inline constexpr uint32_t Id = 0x00;
} /* AudioStreamAllocate */
namespace AudioStreamAllocateResponse {
inline constexpr uint32_t Id = 0x01;
} /* AudioStreamAllocateResponse */
namespace AudioStreamDeallocate {
inline constexpr uint32_t Id = 0x02;
} /* AudioStreamDeallocate */
namespace VideoStreamAllocate {
inline constexpr uint32_t Id = 0x03;
} /* VideoStreamAllocate */
namespace VideoStreamAllocateResponse {
inline constexpr uint32_t Id = 0x04;
} /* VideoStreamAllocateResponse */
namespace VideoStreamModify {
inline constexpr uint32_t Id = 0x05;
} /* VideoStreamModify */
namespace VideoStreamDeallocate {
inline constexpr uint32_t Id = 0x06;
} /* VideoStreamDeallocate */
namespace SnapshotStreamAllocate {
inline constexpr uint32_t Id = 0x07;
} /* SnapshotStreamAllocate */
namespace SnapshotStreamAllocateResponse {
inline constexpr uint32_t Id = 0x08;
} /* SnapshotStreamAllocateResponse */
namespace SnapshotStreamModify {
inline constexpr uint32_t Id = 0x09;
} /* SnapshotStreamModify */
namespace SnapshotStreamDeallocate {
inline constexpr uint32_t Id = 0x0A;
} /* SnapshotStreamDeallocate */
namespace SetStreamPriorities {
inline constexpr uint32_t Id = 0x0B;
} /* SetStreamPriorities */
namespace CaptureSnapshot {
inline constexpr uint32_t Id = 0x0C;
} /* CaptureSnapshot */
namespace CaptureSnapshotResponse {
inline constexpr uint32_t Id = 0x0D;
} /* CaptureSnapshotResponse */
} /* command */

} /* camera_av_stream_management */
} /* cluster */
} /* esp_matter */
