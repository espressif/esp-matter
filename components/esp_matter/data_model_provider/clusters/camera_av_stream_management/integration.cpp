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

#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>
#include <app/SafeAttributePersistenceProvider.h>
#include <data_model/esp_matter_data_model.h>
#include <data_model/esp_matter_endpoint.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <unordered_map>
#include "integration.h"

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::CameraAvStreamManagement;
using namespace esp_matter;

namespace {
std::unordered_map<EndpointId, LazyRegisteredServerCluster<CameraAVStreamManagementCluster>> gServers;
std::unordered_map<EndpointId, CameraAvStreamManagementConfig> gConfigs;

bool IsClusterEnabled(EndpointId endpointId)
{
    cluster_t *cluster = cluster::get(endpointId, CameraAvStreamManagement::Id);
    return cluster != nullptr;
}

uint32_t GetFeatureMap(EndpointId endpointId)
{
    attribute_t *attribute = attribute::get(endpointId, CameraAvStreamManagement::Id, Globals::Attributes::FeatureMap::Id);
    VerifyOrReturnValue(attribute, 0);

    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    VerifyOrReturnValue(attribute::get_val_internal(attribute, &val) == ESP_OK, 0);
    return val.val.u32;
}

BitFlags<OptionalAttribute> GetOptionalAttributes(EndpointId endpointId)
{
    BitFlags<OptionalAttribute> optionalAttrs;

    if (endpoint::is_attribute_enabled(endpointId, CameraAvStreamManagement::Id,
                                       CameraAvStreamManagement::Attributes::HardPrivacyModeOn::Id)) {
        optionalAttrs.Set(OptionalAttribute::kHardPrivacyModeOn);
    }
    if (endpoint::is_attribute_enabled(endpointId, CameraAvStreamManagement::Id,
                                       CameraAvStreamManagement::Attributes::NightVisionIllum::Id)) {
        optionalAttrs.Set(OptionalAttribute::kNightVisionIllum);
    }
    if (endpoint::is_attribute_enabled(endpointId, CameraAvStreamManagement::Id,
                                       CameraAvStreamManagement::Attributes::MicrophoneAGCEnabled::Id)) {
        optionalAttrs.Set(OptionalAttribute::kMicrophoneAGCEnabled);
    }
    if (endpoint::is_attribute_enabled(endpointId, CameraAvStreamManagement::Id,
                                       CameraAvStreamManagement::Attributes::ImageRotation::Id)) {
        optionalAttrs.Set(OptionalAttribute::kImageRotation);
    }
    if (endpoint::is_attribute_enabled(endpointId, CameraAvStreamManagement::Id,
                                       CameraAvStreamManagement::Attributes::ImageFlipHorizontal::Id)) {
        optionalAttrs.Set(OptionalAttribute::kImageFlipHorizontal);
    }
    if (endpoint::is_attribute_enabled(endpointId, CameraAvStreamManagement::Id,
                                       CameraAvStreamManagement::Attributes::ImageFlipVertical::Id)) {
        optionalAttrs.Set(OptionalAttribute::kImageFlipVertical);
    }
    if (endpoint::is_attribute_enabled(endpointId, CameraAvStreamManagement::Id,
                                       CameraAvStreamManagement::Attributes::StatusLightEnabled::Id)) {
        optionalAttrs.Set(OptionalAttribute::kStatusLightEnabled);
    }
    if (endpoint::is_attribute_enabled(endpointId, CameraAvStreamManagement::Id,
                                       CameraAvStreamManagement::Attributes::StatusLightBrightness::Id)) {
        optionalAttrs.Set(OptionalAttribute::kStatusLightBrightness);
    }
    return optionalAttrs;
}
} // namespace

namespace chip::app::Clusters::CameraAvStreamManagement {

void SetConfig(EndpointId endpointId, const CameraAvStreamManagementConfig  &config)
{
    gConfigs[endpointId] = config;
}

const CameraAvStreamManagementConfig * GetConfig(EndpointId endpointId)
{
    auto it = gConfigs.find(endpointId);
    return (it == gConfigs.end()) ? nullptr : &it->second;
}

void SetDelegate(EndpointId endpointId, CameraAVStreamManagementDelegate * delegate)
{
    if (gConfigs.find(endpointId) == gConfigs.end()) {
        ChipLogError(AppServer, "Camera AV Stream Management config not found for endpoint %u", endpointId);
        return;
    }
    gConfigs[endpointId].delegate = delegate;
}

CameraAVStreamManagementCluster * GetServer(EndpointId endpointId)
{
    auto it = gServers.find(endpointId);
    VerifyOrReturnValue(it != gServers.end(), nullptr);
    VerifyOrReturnValue(it->second.IsConstructed(), nullptr);
    return &it->second.Cluster();
}

} // namespace chip::app::Clusters::CameraAvStreamManagement

void ESPMatterCameraAvStreamManagementClusterServerInitCallback(EndpointId endpointId)
{
    if (!IsClusterEnabled(endpointId)) {
        return;
    }

    if (!gServers[endpointId].IsConstructed()) {
        const CameraAvStreamManagementConfig *config = CameraAvStreamManagement::GetConfig(endpointId);
        if (config == nullptr || config->delegate == nullptr) {
            ChipLogError(AppServer, "Camera AV Stream Management config/delegate missing for endpoint %u", endpointId);
            return;
        }

        SafeAttributePersistenceProvider * provider = GetSafeAttributePersistenceProvider();
        if (provider == nullptr) {
            ChipLogError(AppServer, "SafeAttributePersistenceProvider not available for endpoint %u", endpointId);
            return;
        }

        ChipLogProgress(AppServer, "Registering Camera AV Stream Management on endpoint %u", endpointId);
        BitFlags<Feature> features(GetFeatureMap(endpointId));
        BitFlags<OptionalAttribute> optionalAttrs = GetOptionalAttributes(endpointId);

        gServers[endpointId].Create(CameraAVStreamManagementCluster::Context{ *provider }, *config->delegate, endpointId, features,
                                    optionalAttrs,
                                    config->maxConcurrentEncoders, config->maxEncodedPixelRate, config->videoSensorParams,
                                    config->nightVisionUsesInfrared, config->minViewPortRes, config->rateDistortionTradeOffPoints,
                                    config->maxContentBufferSize, config->microphoneCapabilities, config->speakerCapabilities,
                                    config->twoWayTalkSupport, config->snapshotCapabilities, config->maxNetworkBandwidth,
                                    config->supportedStreamUsages, config->streamUsagePriorities);
    }

    CHIP_ERROR err = data_model::provider::get_instance().registry().Register(gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer,
                     "Failed to register Camera AV Stream Management on endpoint %u - Error: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }
}

void ESPMatterCameraAvStreamManagementClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    auto serverIt = gServers.find(endpointId);
    VerifyOrReturn(serverIt != gServers.end());
    VerifyOrReturn(serverIt->second.IsConstructed());

    CHIP_ERROR err = data_model::provider::get_instance().registry().Unregister(&serverIt->second.Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer,
                     "Failed to unregister Camera AV Stream Management on endpoint %u - Error: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        serverIt->second.Destroy();
        gServers.erase(serverIt);
        gConfigs.erase(endpointId);
    }
}

void MatterCameraAvStreamManagementPluginServerInitCallback() {}

void MatterCameraAvStreamManagementPluginServerShutdownCallback() {}
