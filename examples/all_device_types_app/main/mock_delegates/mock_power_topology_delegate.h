/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/power-topology-server/power-topology-server.h>

/*
 * Mock PowerTopology Delegate Implementation
 * This file provides a mock implementation of the PowerTopology::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/power-topology-server/power-topology-server.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/src/power-topology-stub.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace PowerTopology {

class MockPowerTopologyDelegate : public Delegate {
public:
    MockPowerTopologyDelegate() = default;
    virtual ~MockPowerTopologyDelegate() = default;

    // Delegate interface
    CHIP_ERROR GetAvailableEndpointAtIndex(size_t index, EndpointId  &endpointId) override;
    CHIP_ERROR GetActiveEndpointAtIndex(size_t index, EndpointId  &endpointId) override;

private:
    static constexpr const char * LOG_TAG = "MockPowerTopologyDelegate";
};

} // namespace PowerTopology
} // namespace Clusters
} // namespace app
} // namespace chip