/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_time_synchronization_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace TimeSynchronization {

void MockTimeSynchronizationDelegate::TimeZoneListChanged(const Span<TimeSyncDataProvider::TimeZoneStore> timeZoneList)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

bool MockTimeSynchronizationDelegate::HandleUpdateDSTOffset(const CharSpan name)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return false;
}

bool MockTimeSynchronizationDelegate::IsNTPAddressValid(const CharSpan ntp)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

bool MockTimeSynchronizationDelegate::IsNTPAddressDomain(const CharSpan ntp)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return true;
}

CHIP_ERROR MockTimeSynchronizationDelegate::UpdateTimeFromPlatformSource(chip::Callback::Callback<OnTimeSyncCompletion> * callback)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_NOT_IMPLEMENTED;
}

CHIP_ERROR MockTimeSynchronizationDelegate::UpdateTimeUsingNTPFallback(const CharSpan  &fallbackNTP,
                                                                       chip::Callback::Callback<OnFallbackNTPCompletion> * callback)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_NOT_IMPLEMENTED;
}

void MockTimeSynchronizationDelegate::UTCTimeAvailabilityChanged(uint64_t time)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

void MockTimeSynchronizationDelegate::TrustedTimeSourceAvailabilityChanged(bool available, GranularityEnum granularity)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

void MockTimeSynchronizationDelegate::NotifyTimeFailure()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return;
}

} // namespace TimeSynchronization
} // namespace Clusters
} // namespace app
} // namespace chip