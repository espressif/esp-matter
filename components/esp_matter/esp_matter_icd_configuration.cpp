// Copyright 2025 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_check.h>
#include <esp_err.h>
#include <esp_matter_core.h>
#include <esp_matter_icd_configuration.h>

#include <app/icd/server/ICDConfigurationData.h>
#include <lib/core/Optional.h>
#include <system/SystemClock.h>

using namespace chip::System::Clock;
static constexpr char *TAG = "icd";

namespace chip {
namespace Test {
class ICDConfigurationDataTestAccess {
// This class is a friend class for ICDConfigurationData, so it can access private members of ICDConfigurationData.
public:
    static void SetFastPollingInterval(Milliseconds32 pollInterval)
    {
        ICDConfigurationData::GetInstance().SetFastPollingInterval(pollInterval);
    }

    static void SetSlowPollingInterval(Milliseconds32 pollInterval)
    {
        ICDConfigurationData::GetInstance().SetSlowPollingInterval(pollInterval);
        if (pollInterval >= Milliseconds32(15000)) {
            // Active Threshold SHALL NOT be smaller than 5 seconds for LIT ICD.
            ICDConfigurationData::GetInstance().SetICDMode(ICDConfigurationData::ICDMode::LIT);
            if (ICDConfigurationData::GetInstance().mActiveThreshold < Milliseconds32(5000)) {
                ICDConfigurationData::GetInstance().mActiveThreshold = Milliseconds32(5000);
            }
        }
    }

    static esp_err_t SetModeDurations(Optional<Milliseconds32> activeModeDuration, Optional<Seconds32> idleModeDuration)
    {
        Optional<Milliseconds32> idleModeDurationMs = chip::NullOptional;
        if (idleModeDuration.HasValue()) {
            idleModeDurationMs = MakeOptional(std::chrono::duration_cast<Milliseconds32>(idleModeDuration.Value()));
        }
        CHIP_ERROR err = ICDConfigurationData::GetInstance().SetModeDurations(activeModeDuration, idleModeDurationMs);
        return err == CHIP_NO_ERROR ? ESP_OK : ESP_ERR_INVALID_ARG;
    }

    static esp_err_t SetActiveThreshold(Milliseconds32 activeThreshold)
    {
        if (ICDConfigurationData::GetInstance().GetICDMode() == ICDConfigurationData::ICDMode::LIT &&
            activeThreshold < Milliseconds32(5000)) {
            return ESP_ERR_INVALID_ARG;
        }
        ICDConfigurationData::GetInstance().mActiveThreshold = activeThreshold;
        return ESP_OK;
    }
};
} // namespace Test
} // namespace chip

namespace esp_matter {
namespace icd {

static esp_err_t set_polling_intervals(std::optional<uint32_t> fast_interval_ms,
                                       std::optional<uint32_t> slow_interval_ms)
{
    if (slow_interval_ms.has_value()) {
#if !CHIP_CONFIG_ENABLE_ICD_LIT
        if (slow_interval_ms.value() > 15000) {
            return ESP_ERR_INVALID_ARG;
        }
#endif
        chip::Test::ICDConfigurationDataTestAccess::SetSlowPollingInterval(Milliseconds32(slow_interval_ms.value()));
    }
    if (fast_interval_ms.has_value()) {
        chip::Test::ICDConfigurationDataTestAccess::SetFastPollingInterval(Milliseconds32(fast_interval_ms.value()));
    }
    return ESP_OK;
}

static esp_err_t set_mode_durations(std::optional<uint32_t> active_mode_duration_ms,
                                    std::optional<uint32_t> idle_mode_duration_s)
{
    Milliseconds32 active_mode_duration = active_mode_duration_ms.has_value()
        ? Milliseconds32(active_mode_duration_ms.value())
        : chip::ICDConfigurationData::GetInstance().GetActiveModeDuration();
    Seconds32 idle_mode_duration = idle_mode_duration_s.has_value()
        ? Seconds32(idle_mode_duration_s.value())
        : chip::ICDConfigurationData::GetInstance().GetIdleModeDuration();
    return chip::Test::ICDConfigurationDataTestAccess::SetModeDurations(chip::MakeOptional(active_mode_duration),
                                                                        chip::MakeOptional(idle_mode_duration));
}

static esp_err_t set_active_threshold(uint32_t active_threshold_ms)
{
    return chip::Test::ICDConfigurationDataTestAccess::SetActiveThreshold(Milliseconds32(active_threshold_ms));
}

static bool s_enable_icd_server = true;

bool get_icd_server_enabled()
{
    return s_enable_icd_server;
}

esp_err_t set_configuration_data(config_t *config)
{
    ESP_RETURN_ON_FALSE(config, ESP_ERR_INVALID_ARG, TAG, "config cannot be NULL");
    if (!config->enable_icd_server) {
        ESP_RETURN_ON_FALSE(!node::get(), ESP_ERR_INVALID_STATE, TAG,
                            "Could not disable ICD server after data model is created");
    }
    s_enable_icd_server = config->enable_icd_server;
    ESP_RETURN_ON_FALSE(!is_started(), ESP_ERR_INVALID_STATE, TAG,
                        "Could not change ICD configuration data after Matter is started");
    ESP_RETURN_ON_ERROR(set_polling_intervals(config->fast_interval_ms, config->slow_interval_ms), TAG,
                        "Failed to set polling intervals");
    ESP_RETURN_ON_ERROR(set_mode_durations(config->active_mode_duration_ms, config->idle_mode_duration_s), TAG,
                        "Failed to set mode durations");
    if (config->active_threshold_ms.has_value()) {
        ESP_RETURN_ON_ERROR(set_active_threshold(config->active_threshold_ms.value()), TAG,
                            "Failed to set active threshold");
    }
    return ESP_OK;
}

} // namespace icd
} // namespace esp_matter
