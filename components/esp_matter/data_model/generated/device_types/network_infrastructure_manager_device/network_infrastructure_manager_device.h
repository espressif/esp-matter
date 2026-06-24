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
#include <thread_network_diagnostics.h>
#include <wi_fi_network_management.h>
#include <thread_border_router_management.h>
#include <thread_network_directory.h>

#include <esp_matter_core.h>

#define ESP_MATTER_NETWORK_INFRASTRUCTURE_MANAGER_DEVICE_TYPE_ID 0x0090
#define ESP_MATTER_NETWORK_INFRASTRUCTURE_MANAGER_DEVICE_TYPE_VERSION 2

namespace esp_matter {
namespace endpoint {
namespace network_infrastructure_manager {

typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::thread_network_diagnostics::config_t thread_network_diagnostics;
    cluster::wi_fi_network_management::config_t wi_fi_network_management;
    cluster::thread_border_router_management::config_t thread_border_router_management;
    cluster::thread_network_directory::config_t thread_network_directory;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* network_infrastructure_manager */
} /* endpoint */
} /* esp_matter */
