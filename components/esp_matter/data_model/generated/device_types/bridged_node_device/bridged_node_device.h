// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
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

/* THIS IS A GENERATED FILE, DO NOT EDIT */

#pragma once
#include <esp_matter_data_model.h>

#include <descriptor.h>
#include <bridged_device_basic_information.h>

#include <esp_matter_core.h>

#define ESP_MATTER_BRIDGED_NODE_DEVICE_TYPE_ID 0x0013
#define ESP_MATTER_BRIDGED_NODE_DEVICE_TYPE_VERSION 3

namespace esp_matter {
namespace endpoint {
namespace bridged_node {

typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::bridged_device_basic_information::config_t bridged_device_basic_information;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
endpoint_t *resume(node_t *node, config_t *config, uint8_t flags, uint16_t endpoint_id, void *priv_data);
} /* bridged_node */
} /* endpoint */
} /* esp_matter */
