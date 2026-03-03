/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/commissioner-control-server/commissioner-control-server.h>

/*
 * Mock CommissionerControl Delegate Implementation
 * This file provides a mock implementation of the CommissionerControl::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/commissioner-control-server/commissioner-control-server.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/fabric-bridge-app/linux/include/CommissionerControlDelegate.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/fabric-bridge-app/linux/CommissionerControlDelegate.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace CommissionerControl {

class MockCommissionerControlDelegate : public Delegate {
public:
    MockCommissionerControlDelegate() = default;
    virtual ~MockCommissionerControlDelegate() = default;

    // Commissioner control management
    CHIP_ERROR HandleCommissioningApprovalRequest(const CommissioningApprovalRequest  &request) override;
    CHIP_ERROR ValidateCommissionNodeCommand(NodeId clientNodeId, uint64_t requestId) override;
    CHIP_ERROR GetCommissioningWindowParams(CommissioningWindowParams  &outParams) override;
    CHIP_ERROR HandleCommissionNode(const CommissioningWindowParams  &params) override;

private:
    static constexpr const char * LOG_TAG = "MockCommissionerControlDelegate";
};

} // namespace CommissionerControl
} // namespace Clusters
} // namespace app
} // namespace chip