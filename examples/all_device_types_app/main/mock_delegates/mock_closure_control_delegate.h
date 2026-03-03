/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/closure-control-server/closure-control-cluster-delegate.h>
#include <protocols/interaction_model/StatusCode.h>

/*
 * Mock ClosureControl Delegate Implementation
 * This file provides a mock implementation of the ClosureControl::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/closure-control-server/closure-control-cluster-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/closure-app/closure-common/include/ClosureControlEndpoint.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/closure-app/closure-common/src/ClosureControlEndpoint.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace ClosureControl {

class MockClosureControlDelegate : public DelegateBase {
public:
    MockClosureControlDelegate() : DelegateBase() {}
    virtual ~MockClosureControlDelegate() = default;

    // Closure control command handlers
    Protocols::InteractionModel::Status HandleStopCommand() override;
    Protocols::InteractionModel::Status HandleMoveToCommand(const Optional<TargetPositionEnum>  &position,
                                                            const Optional<bool>  &latch,
                                                            const Optional<Globals::ThreeLevelAutoEnum>  &speed) override;
    Protocols::InteractionModel::Status HandleCalibrateCommand() override;

    // Attribute getters
    bool IsReadyToMove() override;
    ElapsedS GetCalibrationCountdownTime() override;
    ElapsedS GetMovingCountdownTime() override;
    ElapsedS GetWaitingForMotionCountdownTime() override;

private:
    static constexpr const char * LOG_TAG = "MockClosureControlDelegate";
};

} // namespace ClosureControl
} // namespace Clusters
} // namespace app
} // namespace chip