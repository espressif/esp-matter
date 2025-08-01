/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/laundry-dryer-controls-server/laundry-dryer-controls-delegate.h>

/*
 * Mock LaundryDryerControls Delegate Implementation
 * This file provides a mock implementation of the LaundryDryerControls::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/laundry-dryer-controls-server/laundry-dryer-controls-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/laundry-dryer-controls-delegate-impl.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/src/laundry-dryer-controls-delegate-impl.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace LaundryDryerControls {

class MockLaundryDryerControlsDelegate : public Delegate {
public:
    MockLaundryDryerControlsDelegate() : Delegate() {}
    virtual ~MockLaundryDryerControlsDelegate() = default;

    // Laundry dryer controls management
    CHIP_ERROR GetSupportedDrynessLevelAtIndex(size_t index, DrynessLevelEnum  &supportedDryness) override;

private:
    static constexpr const char * LOG_TAG = "MockLaundryDryerControlsDelegate";
};

} // namespace LaundryDryerControls
} // namespace Clusters
} // namespace app
} // namespace chip