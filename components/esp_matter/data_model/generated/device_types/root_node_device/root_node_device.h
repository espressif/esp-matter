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
#include <access_control.h>
#include <basic_information.h>
#include <general_commissioning.h>
#include <network_commissioning.h>
#include <general_diagnostics.h>
#include <administrator_commissioning.h>
#include <operational_credentials.h>
#include <group_key_management.h>
#include <icd_management.h>
#if CHIP_DEVICE_CONFIG_ENABLE_WIFI && defined(CONFIG_SUPPORT_WIFI_NETWORK_DIAGNOSTICS_CLUSTER)
#include <wi_fi_network_diagnostics.h>
#endif
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD && defined(CONFIG_SUPPORT_THREAD_NETWORK_DIAGNOSTICS_CLUSTER)
#include <thread_network_diagnostics.h>
#endif

#include <esp_matter_core.h>

#define ESP_MATTER_ROOT_NODE_DEVICE_TYPE_ID 0x0016
#define ESP_MATTER_ROOT_NODE_DEVICE_TYPE_VERSION 4

namespace esp_matter {
namespace endpoint {
namespace root_node {

typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::access_control::config_t access_control;
    cluster::basic_information::config_t basic_information;
    cluster::general_commissioning::config_t general_commissioning;
    cluster::network_commissioning::config_t network_commissioning;
    cluster::general_diagnostics::config_t general_diagnostics;
    cluster::administrator_commissioning::config_t administrator_commissioning;
    cluster::operational_credentials::config_t operational_credentials;
    cluster::group_key_management::config_t group_key_management;
    cluster::icd_management::config_t icd_management;
#if CHIP_DEVICE_CONFIG_ENABLE_WIFI && defined(CONFIG_SUPPORT_WIFI_NETWORK_DIAGNOSTICS_CLUSTER)
    cluster::wi_fi_network_diagnostics::config_t wi_fi_network_diagnostics;
#endif
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD && defined(CONFIG_SUPPORT_THREAD_NETWORK_DIAGNOSTICS_CLUSTER)
    cluster::thread_network_diagnostics::config_t thread_network_diagnostics;
#endif
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* root_node */
} /* endpoint */
} /* esp_matter */
