/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/window-covering-server/window-covering-delegate.h>

/*
 * Mock WindowCovering Delegate Implementation
 * This file provides a mock implementation of the WindowCovering::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/window-covering-server/window-covering-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/chef/common/clusters/window-covering/chef-window-covering.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/chef/common/clusters/window-covering/chef-window-covering.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace WindowCovering {

class MockWindowCoveringDelegate : public Delegate {
public:
    MockWindowCoveringDelegate() = default;

    CHIP_ERROR HandleMovement(WindowCoveringType type) override;
    CHIP_ERROR HandleStopMotion() override;

private:
    const char *LOG_TAG = "window_covering";
};

} // namespace WindowCovering
} // namespace Clusters
} // namespace app
} // namespace chip