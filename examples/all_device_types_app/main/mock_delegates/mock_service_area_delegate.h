/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/service-area-server/service-area-delegate.h>

/*
 * Mock ServiceArea Delegate Implementation
 * This file provides a mock implementation of the ServiceArea::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/service-area-server/service-area-server.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/rvc-app/rvc-common/include/rvc-service-area-delegate.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/rvc-app/rvc-common/src/rvc-service-area-delegate.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace ServiceArea {

class MockServiceAreaDelegate : public Delegate {
public:
    MockServiceAreaDelegate() : Delegate() {}
    virtual ~MockServiceAreaDelegate() = default;

    // Service area management
    CHIP_ERROR Init() override;
    bool IsSetSelectedAreasAllowed(MutableCharSpan  &statusText) override;
    bool IsValidSelectAreasSet(const Span<const uint32_t>  &selectedAreas, SelectAreasStatus  &locationStatus,
                               MutableCharSpan  &statusText) override;
    bool HandleSkipArea(uint32_t skippedArea, MutableCharSpan  &skipStatusText) override;

private:
    static constexpr const char * LOG_TAG = "MockServiceAreaDelegate";
};

} // namespace ServiceArea
} // namespace Clusters
} // namespace app
} // namespace chip