/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app-common/zap-generated/cluster-objects.h>

#include <app/clusters/content-launch-server/content-launch-delegate.h>

/*
 * Mock ContentLauncher Delegate Implementation
 * This file provides a mock implementation of the ContentLauncher::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/content-launch-server/content-launch-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/content-launcher/ContentLauncherManager.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/content-launcher/ContentLauncherManager.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace ContentLauncher {

class MockContentLauncherDelegate : public Delegate {
public:
    MockContentLauncherDelegate() = default;

    void HandleLaunchContent(CommandResponseHelper<Commands::LauncherResponse::Type>  &helper,
                             const DataModel::DecodableList<Parameter>  &parameterList, bool autoplay,
                             const CharSpan  &data, const Optional<PlaybackPreferences> playbackPreferences,
                             bool useCurrentContext) override;
    void HandleLaunchUrl(CommandResponseHelper<Commands::LauncherResponse::Type>  &helper, const CharSpan  &contentUrl,
                         const CharSpan  &displayString, const BrandingInformation  &brandingInformation) override;
    CHIP_ERROR HandleGetAcceptHeaderList(app::AttributeValueEncoder  &aEncoder) override;
    uint32_t HandleGetSupportedStreamingProtocols() override;
    uint32_t GetFeatureMap(chip::EndpointId endpoint) override;
    uint16_t GetClusterRevision(chip::EndpointId endpoint) override;

private:
    const char *LOG_TAG = "content_launcher";
};

} // namespace ContentLauncher
} // namespace Clusters
} // namespace app
} // namespace chip