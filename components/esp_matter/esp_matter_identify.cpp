// Copyright 2022 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_identify.h>

#include <app/clusters/identify-server/identify-server.h>

#ifndef CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
#include <app/util/attribute-storage.h>
#include <zap-generated/gen_config.h>
#if defined(MATTER_DM_IDENTIFY_CLUSTER_SERVER_ENDPOINT_COUNT) && MATTER_DM_IDENTIFY_CLUSTER_SERVER_ENDPOINT_COUNT > 0
#define USE_IDENTIFY_ARRAY
#endif
#endif // !CONFIG_ESP_MATTER_ENABLE_DATA_MODEL

static const char *TAG = "esp_matter_identify";

namespace esp_matter {
namespace identification {

static callback_t identification_callback = NULL;
#ifdef USE_IDENTIFY_ARRAY
static Identify *g_identifies[MATTER_DM_IDENTIFY_CLUSTER_SERVER_ENDPOINT_COUNT];
#endif

esp_err_t set_callback(callback_t callback)
{
    identification_callback = callback;
    return ESP_OK;
}

static esp_err_t execute_callback(callback_type_t type, uint16_t endpoint_id, uint8_t effect_id, uint8_t effect_variant)
{
    if (identification_callback) {
#ifdef CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
        void *priv_data = endpoint::get_priv_data(endpoint_id);
        return identification_callback(type, endpoint_id, effect_id, effect_variant, priv_data);
#else // CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
        void *ctx = nullptr;
#ifdef USE_IDENTIFY_ARRAY
        uint16_t index = emberAfGetClusterServerEndpointIndex(endpoint_id, chip::app::Clusters::Identify::Id,
                                                              MATTER_DM_IDENTIFY_CLUSTER_SERVER_ENDPOINT_COUNT);
        ctx = index < MATTER_ARRAY_SIZE(g_identifies) ? g_identifies[index] : nullptr;
#endif // USE_IDENTIFY_ARRAY
        return identification_callback(type, endpoint_id, effect_id, effect_variant, ctx);
#endif // !CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
    }
    return ESP_OK;
}

static void start_cb(Identify *identify)
{
    ESP_LOGI(TAG, "Start callback");
    execute_callback(START, identify->mEndpoint, static_cast<uint8_t>(identify->mCurrentEffectIdentifier),
                     static_cast<uint8_t>(identify->mEffectVariant));
}

static void stop_cb(Identify *identify)
{
    ESP_LOGI(TAG, "Stop callback");
    execute_callback(STOP, identify->mEndpoint, static_cast<uint8_t>(identify->mCurrentEffectIdentifier),
                     static_cast<uint8_t>(identify->mEffectVariant));
}

static void effect_cb(Identify *identify)
{
    ESP_LOGI(TAG, "Effect callback");
    execute_callback(EFFECT, identify->mEndpoint, static_cast<uint8_t>(identify->mCurrentEffectIdentifier),
                     static_cast<uint8_t>(identify->mEffectVariant));
}

esp_err_t init(uint16_t endpoint_id, uint8_t identify_type, uint8_t effect_identifier, uint8_t effect_variant)
{
    Identify *identify = chip::Platform::New<Identify>(
        endpoint_id, start_cb, stop_cb, (chip::app::Clusters::Identify::IdentifyTypeEnum)identify_type, effect_cb,
        static_cast<chip::app::Clusters::Identify::EffectIdentifierEnum>(effect_identifier),
        static_cast<chip::app::Clusters::Identify::EffectVariantEnum>(effect_variant));
    VerifyOrReturnError(identify, ESP_FAIL, ESP_LOGE(TAG, "Fail to create identify object"));
#ifdef CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
    endpoint::set_identify(endpoint_id, (void *)identify);
#elif defined(USE_IDENTIFY_ARRAY)
    uint16_t index = emberAfGetClusterServerEndpointIndex(endpoint_id, chip::app::Clusters::Identify::Id,
                                                          MATTER_DM_IDENTIFY_CLUSTER_SERVER_ENDPOINT_COUNT);
    if (index < MATTER_ARRAY_SIZE(g_identifies)) {
        g_identifies[index] = identify;
    }
#endif
    return ESP_OK;
}

} // namespace identification
} // namespace esp_matter
