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

/* This is a Generated File */

#pragma once
#include <esp_matter_data_model.h>

#include <descriptor.h>
#include <binding.h>
#include <window_covering.h>

#include <esp_matter_core.h>

#define ESP_MATTER_WINDOW_COVERING_CONTROLLER_DEVICE_TYPE_ID 0x0203
#define ESP_MATTER_WINDOW_COVERING_CONTROLLER_DEVICE_TYPE_VERSION 4

namespace esp_matter {
namespace endpoint {
namespace window_covering_controller {

typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::binding::config_t binding;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* window_covering_controller */
} /* endpoint */
} /* esp_matter */
