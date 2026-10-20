/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "time_sync.h"

#include <sys/time.h>

#include <esp_log.h>
#include <esp_netif_sntp.h>
#include <esp_sntp.h>
#include <app/clusters/time-synchronization-server/DefaultTimeSyncDelegate.h>
#include <freertos/FreeRTOS.h>
#include <lib/support/TimeUtils.h>
#include <system/SystemClock.h>

using namespace chip::DeviceLayer;

namespace {

const char *TAG = "time_sync";

portMUX_TYPE sMutex = portMUX_INITIALIZER_UNLOCKED;
bool sConnected = false;
bool sReady = false;
uint32_t sGeneration = 1;
int64_t sSyncMonotonic = 0;
int64_t sSyncUtc = 0;

void RecordSynchronization(int64_t utcMicroseconds)
{
    const int64_t monotonic = static_cast<int64_t>(chip::System::SystemClock().GetMonotonicMicroseconds64().count());

    portENTER_CRITICAL(&sMutex);
    ++sGeneration;
    sReady = sConnected;
    sSyncMonotonic = monotonic;
    sSyncUtc = utcMicroseconds;
    portEXIT_CRITICAL(&sMutex);
}

void SntpSynchronized(struct timeval *tv)
{
    RecordSynchronization(static_cast<int64_t>(tv->tv_sec) * 1000000 + tv->tv_usec);
    ESP_LOGI(TAG, "SNTP synchronization received");
}

class TimeSyncDelegate final : public chip::app::Clusters::TimeSynchronization::DefaultTimeSyncDelegate {
public:
    CHIP_ERROR UpdateTimeFromPlatformSource(
        chip::Callback::Callback<chip::app::Clusters::TimeSynchronization::OnTimeSyncCompletion> *callback) override
    {
        time_t utc;
        uint32_t generation;
        // A seeded RTC must not suppress a fresh query to the trusted Matter time source.
        if (!TimeSync::Now(utc, generation)) {
            return CHIP_ERROR_REAL_TIME_NOT_SYNCED;
        }
        return DefaultTimeSyncDelegate::UpdateTimeFromPlatformSource(callback);
    }

    void UTCTimeAvailabilityChanged(uint64_t matterUtcMicroseconds) override
    {
        uint64_t unixUtcMicroseconds;
        if (!chip::ChipEpochToUnixEpochMicros(matterUtcMicroseconds, unixUtcMicroseconds) ||
                unixUtcMicroseconds > INT64_MAX) {
            return;
        }
        RecordSynchronization(static_cast<int64_t>(unixUtcMicroseconds));
        ESP_LOGI(TAG, "Matter UTC synchronization received");
    }
};

} // namespace

chip::app::Clusters::TimeSynchronization::Delegate *TimeSync::GetDelegate()
{
    static TimeSyncDelegate delegate;
    return &delegate;
}

bool TimeSync::Now(time_t &utc, uint32_t &generation)
{
    chip::System::Clock::Microseconds64 realTime{0};
    const bool clockAvailable = chip::System::SystemClock().GetClock_RealTime(realTime) == CHIP_NO_ERROR &&
                                realTime.count() <= INT64_MAX;
    const int64_t actual = clockAvailable ? static_cast<int64_t>(realTime.count()) : 0;
    const int64_t monotonic = static_cast<int64_t>(chip::System::SystemClock().GetMonotonicMicroseconds64().count());
    const int64_t maxAge = static_cast<int64_t>(esp_sntp_get_sync_interval()) * 2000;

    portENTER_CRITICAL(&sMutex);
    const int64_t elapsed = monotonic - sSyncMonotonic;
    const int64_t difference = actual - (sSyncUtc + elapsed);
    const bool invalidated = sReady && (!clockAvailable || elapsed < 0 || elapsed > maxAge ||
                                        difference < -2000000 || difference > 2000000);
    if (invalidated) {
        sReady = false;
        ++sGeneration;
    }
    const bool ready = sReady;
    generation = sGeneration;
    portEXIT_CRITICAL(&sMutex);

    if (invalidated) {
        ESP_LOGW(TAG, "Time unavailable: clock error, stale sync or clock step");
    }
    utc = static_cast<time_t>(actual / 1000000);

    return ready;
}

void TimeSync::OnNetworkEvent(const ChipDeviceEvent *event)
{
    ConnectivityChange change = kConnectivity_NoChange;
    if (event->Type == DeviceEventType::kInternetConnectivityChange) {
        const auto &ip = event->InternetConnectivityChange;
        if (ip.IPv4 != kConnectivity_NoChange || ip.IPv6 != kConnectivity_NoChange) {
            change = (ip.IPv4 == kConnectivity_Established || ip.IPv6 == kConnectivity_Established) ?
                     kConnectivity_Established : kConnectivity_Lost;
        }
    } else if (event->Type == DeviceEventType::kThreadConnectivityChange) {
        change = event->ThreadConnectivityChange.Result;
    } else if (event->Type == DeviceEventType::kWiFiConnectivityChange &&
               event->WiFiConnectivityChange.Result == kConnectivity_Lost) {
        change = kConnectivity_Lost;
    }
    if (change == kConnectivity_NoChange) {
        return;
    }
    const bool connected = change == kConnectivity_Established;

    portENTER_CRITICAL(&sMutex);
    sConnected = connected;
    if (!connected) {
        sReady = false;
        ++sGeneration;
    }
    portEXIT_CRITICAL(&sMutex);

    if (!connected) {
        return;
    }
    static bool initialized = false;
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    config.sync_cb = SntpSynchronized;
    config.wait_for_sync = false;
    const esp_err_t err = initialized ? esp_netif_sntp_start() : esp_netif_sntp_init(&config);
    if (err == ESP_OK) {
        initialized = true;
    } else {
        ESP_LOGE(TAG, "SNTP start failed: %d", err);
    }
}
