/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/low-power-server/low-power-delegate.h>

/*
 * Mock LowPower Delegate Implementation
 * This file provides a mock implementation of the LowPower::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/low-power-server/low-power-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/low-power/LowPowerManager.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/low-power/LowPowerManager.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace LowPower {

class MockLowPowerDelegate : public Delegate {
public:
    MockLowPowerDelegate() = default;

    bool HandleSleep() override;

private:
    const char *LOG_TAG = "low_power";
};

} // namespace LowPower
} // namespace Clusters
} // namespace app
} // namespace chip