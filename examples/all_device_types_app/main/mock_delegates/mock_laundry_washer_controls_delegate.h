/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/laundry-washer-controls-server/laundry-washer-controls-delegate.h>

/*
 * Mock LaundryWasherControls Delegate Implementation
 * This file provides a mock implementation of the LaundryWasherControls::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/laundry-washer-controls-server/laundry-washer-controls-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/laundry-washer-controls-delegate-impl.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/src/laundry-washer-controls-delegate-impl.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace LaundryWasherControls {

class MockLaundryWasherControlsDelegate : public Delegate {
public:
    MockLaundryWasherControlsDelegate() : Delegate() {}
    virtual ~MockLaundryWasherControlsDelegate() = default;

    // Laundry washer controls management
    CHIP_ERROR GetSpinSpeedAtIndex(size_t index, MutableCharSpan  &spinSpeed) override;
    CHIP_ERROR GetSupportedRinseAtIndex(size_t index, NumberOfRinsesEnum  &supportedRinse) override;

private:
    static constexpr const char * LOG_TAG = "MockLaundryWasherControlsDelegate";
};

} // namespace LaundryWasherControls
} // namespace Clusters
} // namespace app
} // namespace chip