/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/thread-border-router-management-server/thread-br-delegate.h>
#include <protocols/interaction_model/StatusCode.h>

/*
 * Mock ThreadBorderRouterManagement Delegate Implementation
 * This file provides a mock implementation of the ThreadBorderRouterManagement::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/thread-border-router-management-server/thread-br-delegate.h
 */

namespace chip {
namespace app {
namespace Clusters {
namespace ThreadBorderRouterManagement {

class MockThreadBorderRouterManagementDelegate : public Delegate {
public:
    MockThreadBorderRouterManagementDelegate() : Delegate() {}
    virtual ~MockThreadBorderRouterManagementDelegate() = default;

    // Thread Border Router management
    CHIP_ERROR Init(AttributeChangeCallback * attributeChangeCallback) override;
    bool GetPanChangeSupported() override;
    void GetBorderRouterName(MutableCharSpan  &borderRouterName) override;
    CHIP_ERROR GetBorderAgentId(MutableByteSpan  &borderAgentId) override;
    uint16_t GetThreadVersion() override;
    bool GetInterfaceEnabled() override;
    CHIP_ERROR GetDataset(Thread::OperationalDataset  &dataset, DatasetType type) override;
    void SetActiveDataset(const Thread::OperationalDataset  &activeDataset, uint32_t sequenceNum,
                          ActivateDatasetCallback * callback) override;
    CHIP_ERROR CommitActiveDataset() override;
    CHIP_ERROR RevertActiveDataset() override;
    CHIP_ERROR SetPendingDataset(const Thread::OperationalDataset  &pendingDataset) override;

private:
    static constexpr const char * LOG_TAG = "MockThreadBRMgmtDelegate";
};

} // namespace ThreadBorderRouterManagement
} // namespace Clusters
} // namespace app
} // namespace chip
