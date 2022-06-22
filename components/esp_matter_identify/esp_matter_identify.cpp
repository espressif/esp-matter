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

#include <stdio.h>
#include <esp_matter.h>
#include <esp_matter_identify.h>
#include <lib/support/CHIPMem.h>

using namespace esp_matter;
static const char *TAG = "esp_matter_identify";

esp_err_t esp_matter_init_identify(chip::EndpointId endpoint, identify_callback start_cb, identify_callback stop_cb,
                                   EmberAfIdentifyIdentifyType type, identify_callback effect_cb)
{
    endpoint_t *ep = endpoint::get(node::get(), endpoint);
    if (!ep) {
        ESP_LOGE(TAG, "Couldn't get the endpoint for endpointid: %d", endpoint);
        return ESP_ERR_INVALID_ARG;
    }

    cluster_t *cluster = cluster::get(ep, chip::app::Clusters::Identify::Id);
    // TODO: use cluster::identify::get_id() instead of chip::app::Clusters::Identify::Id
    if (!cluster) {
        ESP_LOGE(TAG, "Couldn't get the identify cluster on endpointid: %d", endpoint);
        return ESP_ERR_INVALID_ARG;
    }
    
    Identify *identify = new Identify(endpoint, start_cb, stop_cb, type, effect_cb);
    if (!identify) {
        ESP_LOGE(TAG, "Fail to create identify object");
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}
