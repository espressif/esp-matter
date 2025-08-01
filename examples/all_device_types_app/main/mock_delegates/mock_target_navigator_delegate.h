/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app-common/zap-generated/cluster-objects.h>

#include <app/clusters/target-navigator-server/target-navigator-delegate.h>

/*
 * Mock TargetNavigator Delegate Implementation
 * This file provides a mock implementation of the TargetNavigator::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/target-navigator-server/target-navigator-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/target-navigator/TargetNavigatorManager.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/target-navigator/TargetNavigatorManager.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace TargetNavigator {

class MockTargetNavigatorDelegate : public Delegate {
public:
    MockTargetNavigatorDelegate() = default;

    CHIP_ERROR HandleGetTargetList(app::AttributeValueEncoder  &aEncoder) override;
    uint8_t HandleGetCurrentTarget() override;
    void HandleNavigateTarget(CommandResponseHelper<Commands::NavigateTargetResponse::Type>  &helper,
                              const uint64_t  &target, const CharSpan  &data) override;
    uint16_t GetClusterRevision(chip::EndpointId endpoint) override;

private:
    const char *LOG_TAG = "target_navigator";
};

} // namespace TargetNavigator
} // namespace Clusters
} // namespace app
} // namespace chip