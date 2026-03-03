/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/boolean-state-configuration-server/boolean-state-configuration-delegate.h>

/*
 * Mock BooleanStateConfiguration Delegate Implementation
 * This file provides a mock implementation of the BooleanStateConfiguration::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/src/boolcfg-stub.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace BooleanStateConfiguration {

class MockBooleanStateConfigurationDelegate : public Delegate {
public:
    MockBooleanStateConfigurationDelegate() : Delegate() {}
    virtual ~MockBooleanStateConfigurationDelegate() = default;

    // Boolean state configuration command handlers
    CHIP_ERROR HandleSuppressAlarm(BooleanStateConfiguration::AlarmModeBitmap alarmToSuppress) override;
    CHIP_ERROR HandleEnableDisableAlarms(chip::BitMask<BooleanStateConfiguration::AlarmModeBitmap> alarms) override;

private:
    static constexpr const char * LOG_TAG = "MockBooleanStateConfigurationDelegate";
};

} // namespace BooleanStateConfiguration
} // namespace Clusters
} // namespace app
} // namespace chip