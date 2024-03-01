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

#pragma once
#include <sdkconfig.h>

#include <esp_err.h>
#include <esp_matter_core.h>

#define MAX_BRIDGED_DEVICE_COUNT \
    CONFIG_ESP_MATTER_MAX_DYNAMIC_ENDPOINT_COUNT - 1 - CONFIG_ESP_MATTER_AGGREGATOR_ENDPOINT_COUNT
// There is an endpoint reserved as root endpoint

namespace esp_matter_bridge {

typedef struct device_persistent_info {
    uint16_t parent_endpoint_id;
    uint16_t device_endpoint_id;
    uint32_t device_type_id;
} device_persistent_info_t;

typedef struct device {
    esp_matter::node_t *node;
    esp_matter::endpoint_t *endpoint;
    device_persistent_info_t persistent_info;
} device_t;

typedef esp_err_t (*bridge_device_type_callback_t)(esp_matter::endpoint_t *ep, uint32_t device_type_id, void *priv_data);

esp_err_t get_bridged_endpoint_ids(uint16_t *matter_endpoint_id_array);

esp_err_t erase_bridged_device_info(uint16_t matter_endpoint_id);

device_t *create_device(esp_matter::node_t *node, uint16_t parent_endpoint_id, uint32_t device_type_id,
                        void *priv_data);

device_t *resume_device(esp_matter::node_t *node, uint16_t device_endpoint_id, void *priv_data);

esp_err_t set_device_type(device_t *bridged_device, uint32_t device_type_id, void *priv_data);

esp_err_t remove_device(device_t *bridged_device);

esp_err_t initialize(esp_matter::node_t *node, bridge_device_type_callback_t device_type_cb);

esp_err_t factory_reset();
} // namespace esp_matter_bridge
