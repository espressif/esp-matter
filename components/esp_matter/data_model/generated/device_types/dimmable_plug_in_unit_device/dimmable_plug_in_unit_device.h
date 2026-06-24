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
#include <level_control.h>
#include <scenes_management.h>

#include <esp_matter_core.h>

#define ESP_MATTER_DIMMABLE_PLUG_IN_UNIT_DEVICE_TYPE_ID 0x010B
#define ESP_MATTER_DIMMABLE_PLUG_IN_UNIT_DEVICE_TYPE_VERSION 5

namespace esp_matter {
namespace endpoint {
namespace dimmable_plug_in_unit {

typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::groups::config_t groups;
    cluster::on_off::config_t on_off;
    cluster::on_off::feature::lighting::config_t on_off_lighting;
    cluster::level_control::config_t level_control;
    cluster::level_control::feature::lighting::config_t level_control_lighting;
    cluster::scenes_management::config_t scenes_management;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* dimmable_plug_in_unit */
} /* endpoint */
} /* esp_matter */
