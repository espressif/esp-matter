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
#include <identify.h>
#include <groups.h>
#include <on_off.h>
#include <scenes_management.h>

#include <esp_matter_core.h>

#define ESP_MATTER_MOUNTED_ON_OFF_CONTROL_DEVICE_TYPE_ID 0x010F
#define ESP_MATTER_MOUNTED_ON_OFF_CONTROL_DEVICE_TYPE_VERSION 2

namespace esp_matter {
namespace endpoint {
namespace mounted_on_off_control {

typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::groups::config_t groups;
    cluster::on_off::config_t on_off;
    cluster::on_off::feature::lighting::config_t on_off_lighting;
    cluster::scenes_management::config_t scenes_management;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* mounted_on_off_control */
} /* endpoint */
} /* esp_matter */
