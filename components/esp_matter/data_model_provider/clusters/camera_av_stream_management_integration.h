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

#pragma once

#include <app/clusters/camera-av-stream-management-server/CameraAVStreamManagementCluster.h>
#include <lib/core/DataModelTypes.h>
#include <lib/support/Span.h>
#include <vector>

namespace chip {
namespace app {
namespace Clusters {
namespace CameraAvStreamManagement {

struct CameraAvStreamManagementConfig
{
    CameraAVStreamManagementDelegate * delegate = nullptr;
    uint8_t maxConcurrentEncoders = 0;
    uint32_t maxEncodedPixelRate  = 0;
    VideoSensorParamsStruct videoSensorParams{};
    bool nightVisionUsesInfrared = false;
    VideoResolutionStruct minViewPortRes{};
    std::vector<Structs::RateDistortionTradeOffPointsStruct::Type> rateDistortionTradeOffPoints;
    uint32_t maxContentBufferSize = 0;
    AudioCapabilitiesStruct microphoneCapabilities{};
    AudioCapabilitiesStruct speakerCapabilities{};
    TwoWayTalkSupportTypeEnum twoWayTalkSupport = TwoWayTalkSupportTypeEnum::kNotSupported;
    std::vector<Structs::SnapshotCapabilitiesStruct::Type> snapshotCapabilities;
    uint32_t maxNetworkBandwidth = 0;
    std::vector<Globals::StreamUsageEnum> supportedStreamUsages;
    std::vector<Globals::StreamUsageEnum> streamUsagePriorities;
};

void SetConfig(EndpointId endpointId, const CameraAvStreamManagementConfig & config);
const CameraAvStreamManagementConfig * GetConfig(EndpointId endpointId);
void SetDelegate(EndpointId endpointId, CameraAVStreamManagementDelegate * delegate);

} // namespace CameraAvStreamManagement
} // namespace Clusters
} // namespace app
} // namespace chip
