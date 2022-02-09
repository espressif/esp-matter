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

#define MAX_BRIDGED_DEVICE_COUNT CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT - 1
// There is an endpoint reserved as root endpoint

typedef struct esp_matter_bridge_device {
    esp_matter_node_t *node;
    esp_matter_endpoint_t *endpoint;
    int endpoint_id;
} esp_matter_bridge_device_t;

esp_matter_bridge_device_t *esp_matter_bridge_create_device(esp_matter_node_t *node);

esp_err_t esp_matter_bridge_remove_device(esp_matter_bridge_device_t *bridged_device);
