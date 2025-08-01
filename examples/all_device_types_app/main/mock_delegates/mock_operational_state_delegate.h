/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/operational-state-server/operational-state-server.h>
#include <utility>

/*
 * Mock OperationalState Delegate Implementation
 * This file provides a mock implementation of the OperationalState::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/operational-state-server/operational-state-server.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/operational-state-delegate-impl.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/src/operational-state-delegate-impl.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace OperationalState {

class MockOperationalStateDelegate : public Delegate {
public:
    MockOperationalStateDelegate() = default;
    virtual ~MockOperationalStateDelegate() = default;

    // Operational state management
    app::DataModel::Nullable<uint32_t> GetCountdownTime() override;
    CHIP_ERROR GetOperationalStateAtIndex(size_t index, GenericOperationalState  &operationalState) override;
    CHIP_ERROR GetOperationalPhaseAtIndex(size_t index, MutableCharSpan  &operationalPhase) override;

    // Command callbacks
    void HandlePauseStateCallback(GenericOperationalError  &err) override;
    void HandleResumeStateCallback(GenericOperationalError  &err) override;
    void HandleStartStateCallback(GenericOperationalError  &err) override;
    void HandleStopStateCallback(GenericOperationalError  &err) override;

private:
    static constexpr const char * LOG_TAG = "MockOperationalStateDelegate";
};

} // namespace OperationalState
} // namespace Clusters
} // namespace app
} // namespace chip