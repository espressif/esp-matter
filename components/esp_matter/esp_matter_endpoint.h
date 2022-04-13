// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_matter_cluster.h>
#include <esp_matter_core.h>

namespace esp_matter {
using namespace cluster;

namespace endpoint {
namespace root_node {
typedef struct config {
    basic::config_t basic;
    ota_provider::config_t ota_provider;
    ota_requestor::config_t ota_requestor;
    general_commissioning::config_t general_commissioning;
    network_commissioning::config_t network_commissioning;
    general_diagnostics::config_t general_diagnostics;
    administrator_commissioning::config_t administrator_commissioning;
    operational_credentials::config_t operational_credentials;
} config_t;

int get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags);
} /* root_node */

namespace on_off_light {
typedef struct config {
    identify::config_t identify;
    groups::config_t groups;
    scenes::config_t scenes;
    on_off::config_t on_off;
    basic::config_t basic;
} config_t;

int get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags);
} /* on_off_light */

namespace dimmable_light {
typedef struct config {
    identify::config_t identify;
    groups::config_t groups;
    scenes::config_t scenes;
    on_off::config_t on_off;
    level_control::config_t level_control;
    basic::config_t basic;
} config_t;

int get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags);
} /* dimmable_light */

namespace color_dimmable_light {
typedef struct config {
    identify::config_t identify;
    groups::config_t groups;
    scenes::config_t scenes;
    on_off::config_t on_off;
    level_control::config_t level_control;
    basic::config_t basic;
    color_control::config_t color_control;
} config_t;

int get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags);
} /* color_dimmable_light */

namespace on_off_switch {
typedef struct config {
    identify::config_t identify;
    groups::config_t groups;
    scenes::config_t scenes;
    on_off::config_t on_off;
    basic::config_t basic;
    binding::config_t binding;
} config_t;

int get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags);
} /* on_off_switch */

namespace fan {
typedef struct config {
    identify::config_t identify;
    groups::config_t groups;
    fan_control::config_t fan_control;
} config_t;

int get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags);
} /* fan */

namespace thermostat {
typedef struct config {
    identify::config_t identify;
    groups::config_t groups;
    scenes::config_t scenes;
    basic::config_t basic;
    cluster::thermostat::config_t thermostat;
} config_t;

int get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags);
} /* thermostat */

namespace bridged_node {
typedef struct config {
    bridged_device_basic::config_t bridged_device_basic;
    fixed_label::config_t fixed_label;
} config_t;

int get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags);
} /* bridged_node */

namespace door_lock {
typedef struct config {
    identify::config_t identify;
    cluster::door_lock::config_t door_lock;
    time_synchronization::config_t time_synchronization;
} config_t;

int get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags);
} /* door_lock */

namespace temperature_sensor {
typedef struct config {
    identify::config_t identify;
    groups::config_t groups;
    temperature_measurement::config_t temperature_measurement;
} config_t;

int get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags);
} /* temperature_sensor */
} /* endpoint */

namespace node {
typedef struct config {
    endpoint::root_node::config_t root_node;
} config_t;

node_t *create(config_t *config, attribute::callback_t callback, void *priv_data);

} /* node */
} /* esp_matter */
