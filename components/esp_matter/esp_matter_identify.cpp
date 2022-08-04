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

static const char *TAG = "esp_matter_identify";

namespace esp_matter {
namespace identification {

static callback_t identification_callback = NULL;

esp_err_t set_callback(callback_t callback)
{
    identification_callback = callback;
    return ESP_OK;
}

static esp_err_t execute_callback(callback_type_t type, uint16_t endpoint_id, uint8_t effect_id)
{
    if (identification_callback) {
        void *priv_data = endpoint::get_priv_data(endpoint_id);
        return identification_callback(type, endpoint_id, effect_id, priv_data);
    }
    return ESP_OK;
}

static void start_cb(Identify *identify)
{
    ESP_LOGI(TAG, "Start callback");
    execute_callback(START, identify->mEndpoint, identify->mCurrentEffectIdentifier);
}

static void stop_cb(Identify *identify)
{
    ESP_LOGI(TAG, "Stop callback");
    execute_callback(STOP, identify->mEndpoint, identify->mCurrentEffectIdentifier);
}

static void effect_cb(Identify *identify)
{
    ESP_LOGI(TAG, "Effect callback");
    execute_callback(EFFECT, identify->mEndpoint, identify->mCurrentEffectIdentifier);
}

esp_err_t init(uint16_t endpoint_id, uint8_t identify_type)
{
    Identify *identify = new Identify(endpoint_id, start_cb, stop_cb, (EmberAfIdentifyIdentifyType)identify_type, effect_cb);
    if (!identify) {
        ESP_LOGE(TAG, "Fail to create identify object");
        return ESP_FAIL;
    }
    return ESP_OK;
}

} /* identification */
} /* esp_matter */
