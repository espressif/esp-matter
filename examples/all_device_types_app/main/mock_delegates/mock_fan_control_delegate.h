/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/fan-control-server/fan-control-delegate.h>
#include <protocols/interaction_model/StatusCode.h>

/*
 * Mock FanControl Delegate Implementation
 * This file provides a mock implementation of the FanControl::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/fan-control-server/fan-control-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/src/fan-stub.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace FanControl {

class MockFanControlDelegate : public Delegate {
public:
    MockFanControlDelegate() : Delegate(kInvalidEndpointId) {}
    MockFanControlDelegate(EndpointId aEndpoint) : Delegate(aEndpoint) {}
    virtual ~MockFanControlDelegate() = default;

    // Fan control command handlers
    Protocols::InteractionModel::Status HandleStep(StepDirectionEnum aDirection, bool aWrap, bool aLowestOff) override;

private:
    static constexpr const char * LOG_TAG = "MockFanControlDelegate";
};

} // namespace FanControl
} // namespace Clusters
} // namespace app
} // namespace chip