/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <protocols/interaction_model/StatusCode.h>
#include <app/clusters/closure-dimension-server/closure-dimension-delegate.h>

/*
 * Mock ClosureDimension Delegate Implementation
 * This file provides a mock implementation of the ClosureDimension::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/closure-dimension-server/closure-dimension-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/closure-app/closure-common/include/ClosureDimensionEndpoint.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/closure-app/closure-common/src/ClosureDimensionEndpoint.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace ClosureDimension {

class MockClosureDimensionDelegate : public DelegateBase {
public:
    MockClosureDimensionDelegate() : DelegateBase() {}
    virtual ~MockClosureDimensionDelegate() = default;

    // Closure dimension command handlers
    Protocols::InteractionModel::Status HandleSetTarget(const Optional<Percent100ths>  &position,
                                                        const Optional<bool>  &latch,
                                                        const Optional<Globals::ThreeLevelAutoEnum>  &speed) override;
    Protocols::InteractionModel::Status HandleStep(const StepDirectionEnum  &direction, const uint16_t  &numberOfSteps,
                                                   const Optional<Globals::ThreeLevelAutoEnum>  &speed) override;

private:
    static constexpr const char * LOG_TAG = "MockClosureDimensionDelegate";
};

} // namespace ClosureDimension
} // namespace Clusters
} // namespace app
} // namespace chip
