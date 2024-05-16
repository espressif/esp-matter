// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#if CONFIG_ENABLE_ESP_INSIGHTS_TRACE
#include <esp_insights.h>
#include <esp_log.h>
#include <platform/PlatformManager.h>
#include <tracing/esp32_trace/esp32_tracing.h>
#include <tracing/registry.h>

#if CONFIG_ENABLE_ESP_INSIGHTS_SYSTEM_STATS
#include <tracing/esp32_trace/insights_sys_stats.h>
#define START_TIMEOUT_MS 60000
#endif

static void register_backend(intptr_t context)
{
    static chip::Tracing::Insights::ESP32Backend backend;
    chip::Tracing::Register(backend);

#if CONFIG_ENABLE_ESP_INSIGHTS_SYSTEM_STATS
    chip::System::Stats::InsightsSystemMetrics::GetInstance().RegisterAndEnable(chip::System::Clock::Timeout(START_TIMEOUT_MS));
#endif
}

void enable_insights(const char * insights_auth_key)
{
    esp_insights_config_t config = {
        .log_type = ESP_DIAG_LOG_TYPE_ERROR | ESP_DIAG_LOG_TYPE_WARNING | ESP_DIAG_LOG_TYPE_EVENT,
        .auth_key = insights_auth_key,
    };

    esp_err_t ret = esp_insights_init(&config);

    if (ret != ESP_OK)
    {
        ESP_LOGE("Insights", "Failed to initialize ESP Insights, err:0x%x", ret);
    }

    chip::DeviceLayer::PlatformMgr().ScheduleWork(register_backend, reinterpret_cast<intptr_t>(nullptr));
}
#endif // CONFIG_ENABLE_ESP_INSIGHTS_TRACE
