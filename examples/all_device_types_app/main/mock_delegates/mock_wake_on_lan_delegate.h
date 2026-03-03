/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/wake-on-lan-server/wake-on-lan-delegate.h>

/*
 * Mock WakeOnLan Delegate Implementation
 * This file provides a mock implementation of the WakeOnLan::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/wake-on-lan-server/wake-on-lan-server.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/wake-on-lan/WakeOnLanManager.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/wake-on-lan/WakeOnLanManager.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace WakeOnLan {

class MockWakeOnLanDelegate : public Delegate {
public:
    MockWakeOnLanDelegate() = default;

    CHIP_ERROR HandleGetMacAddress(app::AttributeValueEncoder  &aEncoder) override;

private:
    const char *LOG_TAG = "wake_on_lan";
};

} // namespace WakeOnLan
} // namespace Clusters
} // namespace app
} // namespace chip