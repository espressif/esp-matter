/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/time-synchronization-server/time-synchronization-delegate.h>

/*
 * Mock TimeSynchronization Delegate Implementation
 * This file provides a mock implementation of the TimeSynchronization::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/time-synchronization-server/time-synchronization-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/time-synchronization-server/DefaultTimeSyncDelegate.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/time-synchronization-server/DefaultTimeSyncDelegate.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace TimeSynchronization {

class MockTimeSynchronizationDelegate : public Delegate {
public:
    MockTimeSynchronizationDelegate() : Delegate() {}
    virtual ~MockTimeSynchronizationDelegate() = default;

    // Time synchronization management
    void TimeZoneListChanged(const Span<TimeSyncDataProvider::TimeZoneStore> timeZoneList) override;
    bool HandleUpdateDSTOffset(const CharSpan name) override;
    bool IsNTPAddressValid(const CharSpan ntp) override;
    bool IsNTPAddressDomain(const CharSpan ntp) override;
    CHIP_ERROR UpdateTimeFromPlatformSource(chip::Callback::Callback<OnTimeSyncCompletion> * callback) override;
    CHIP_ERROR UpdateTimeUsingNTPFallback(const CharSpan  &fallbackNTP,
                                          chip::Callback::Callback<OnFallbackNTPCompletion> * callback) override;
    void UTCTimeAvailabilityChanged(uint64_t time) override;
    void TrustedTimeSourceAvailabilityChanged(bool available, GranularityEnum granularity) override;
    void NotifyTimeFailure() override;

private:
    static constexpr const char * LOG_TAG = "MockTimeSynchronizationDelegate";
};

} // namespace TimeSynchronization
} // namespace Clusters
} // namespace app
} // namespace chip