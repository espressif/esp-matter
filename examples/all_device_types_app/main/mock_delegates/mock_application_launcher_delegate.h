/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app-common/zap-generated/cluster-objects.h>

#include <app/clusters/application-launcher-server/application-launcher-delegate.h>

/*
 * Mock ApplicationLauncher Delegate Implementation
 * This file provides a mock implementation of the ApplicationLauncher::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/application-launcher-server/application-launcher-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/application-launcher/ApplicationLauncherManager.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/application-launcher/ApplicationLauncherManager.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace ApplicationLauncher {

class MockApplicationLauncherDelegate : public Delegate {
public:
    MockApplicationLauncherDelegate() = default;
    MockApplicationLauncherDelegate(bool featureMapContentPlatform) : Delegate(featureMapContentPlatform) {}

    CHIP_ERROR HandleGetCatalogList(app::AttributeValueEncoder  &aEncoder) override;
    void HandleLaunchApp(CommandResponseHelper<LauncherResponseType>  &helper, const ByteSpan  &data,
                         const Application  &application) override;
    void HandleStopApp(CommandResponseHelper<LauncherResponseType>  &helper, const Application  &application) override;
    void HandleHideApp(CommandResponseHelper<LauncherResponseType>  &helper, const Application  &application) override;

private:
    const char *LOG_TAG = "application_launcher";
};

} // namespace ApplicationLauncher
} // namespace Clusters
} // namespace app
} // namespace chip