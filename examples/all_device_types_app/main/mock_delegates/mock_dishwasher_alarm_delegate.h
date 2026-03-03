/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/dishwasher-alarm-server/dishwasher-alarm-delegate.h>

/*
 * Mock DishwasherAlarm Delegate Implementation
 * This file provides a mock implementation of the DishwasherAlarm::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/dishwasher-alarm-server/dishwasher-alarm-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/src/dishwasher-alarm-stub.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace DishwasherAlarm {

class MockDishwasherAlarmDelegate : public Delegate {
public:
    MockDishwasherAlarmDelegate() : Delegate() {}
    MockDishwasherAlarmDelegate(EndpointId endpoint) : Delegate(endpoint) {}
    virtual ~MockDishwasherAlarmDelegate() = default;

    // Dishwasher alarm callbacks
    bool ModifyEnabledAlarmsCallback(const BitMask<AlarmMap> mask) override;
    bool ResetAlarmsCallback(const BitMask<AlarmMap> alarms) override;

private:
    static constexpr const char * LOG_TAG = "MockDishwasherAlarmDelegate";
};

} // namespace DishwasherAlarm
} // namespace Clusters
} // namespace app
} // namespace chip