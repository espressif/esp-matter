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
#include <esp_matter_identify.h>

namespace esp_matter {

/** Specific endpoint (device type) create APIs
 *
 * These APIs also create the mandatory clusters and the mandatory attributes and commands for the clusters.
 * The configs has the cluster configs for the mandatory clusters, if it exists.
 *
 * If some standard endpoint (device type) is not present here, it can be added.
 * If a custom endpoint needs to be created, the low level esp_matter::endpoint::create() API can be used.
 */

namespace endpoint {
namespace root_node {
typedef struct config {
    cluster::basic::config_t basic;
    cluster::general_commissioning::config_t general_commissioning;
    cluster::network_commissioning::config_t network_commissioning;
    cluster::general_diagnostics::config_t general_diagnostics;
    cluster::administrator_commissioning::config_t administrator_commissioning;
    cluster::operational_credentials::config_t operational_credentials;
    cluster::diagnostics_network_wifi::config_t diagnostics_network_wifi;
    cluster::diagnostics_network_thread::config_t diagnostics_network_thread;
} config_t;

uint32_t get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
} /* root_node */

namespace on_off_light {
typedef struct config {
    cluster::identify::config_t identify;
    cluster::groups::config_t groups;
    cluster::scenes::config_t scenes;
    cluster::on_off::config_t on_off;
} config_t;

uint32_t get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
} /* on_off_light */

namespace dimmable_light {
typedef struct config {
    cluster::identify::config_t identify;
    cluster::groups::config_t groups;
    cluster::scenes::config_t scenes;
    cluster::on_off::config_t on_off;
    cluster::level_control::config_t level_control;
} config_t;

uint32_t get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
} /* dimmable_light */

namespace color_temperature_light {
typedef struct config {
    cluster::identify::config_t identify;
    cluster::groups::config_t groups;
    cluster::scenes::config_t scenes;
    cluster::on_off::config_t on_off;
    cluster::level_control::config_t level_control;
    cluster::color_control::config_t color_control;
} config_t;

uint32_t get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
} /* color_temperature_light */

namespace extended_color_light {
typedef struct config {
    cluster::identify::config_t identify;
    cluster::groups::config_t groups;
    cluster::scenes::config_t scenes;
    cluster::on_off::config_t on_off;
    cluster::level_control::config_t level_control;
    cluster::color_control::config_t color_control;
} config_t;

uint32_t get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
} /* extended_color_light */

namespace on_off_switch {
typedef struct config {
    cluster::identify::config_t identify;
    cluster::binding::config_t binding;
} config_t;

uint32_t get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
} /* on_off_switch */

namespace dimmer_switch {
typedef struct config {
    cluster::identify::config_t identify;
    cluster::binding::config_t binding;
} config_t;

uint32_t get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
} /* dimmer_switch */

namespace color_dimmer_switch {
typedef struct config {
    cluster::identify::config_t identify;
    cluster::binding::config_t binding;
} config_t;

uint32_t get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
} /* color_dimmer_switch */

namespace on_off_plugin_unit {
typedef struct config {
    cluster::identify::config_t identify;
    cluster::groups::config_t groups;
    cluster::scenes::config_t scenes;
    cluster::on_off::config_t on_off;
} config_t;

uint32_t get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
} /* on_off_plugin_unit */

namespace dimmable_plugin_unit {
typedef struct config {
    cluster::identify::config_t identify;
    cluster::groups::config_t groups;
    cluster::scenes::config_t scenes;
    cluster::on_off::config_t on_off;
    cluster::level_control::config_t level_control;
} config_t;

uint32_t get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
} /* dimmable_plugin_unit */

namespace fan {
typedef struct config {
    cluster::identify::config_t identify;
    cluster::groups::config_t groups;
    cluster::fan_control::config_t fan_control;
} config_t;

uint32_t get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
} /* fan */

namespace thermostat {
typedef struct config {
    cluster::identify::config_t identify;
    cluster::scenes::config_t scenes;
    cluster::thermostat::config_t thermostat;
} config_t;

uint32_t get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
} /* thermostat */

namespace bridge {
uint32_t get_device_type_id();
} /* bridge */

namespace bridged_node {
typedef struct config {
    cluster::bridged_device_basic::config_t bridged_device_basic;
} config_t;

uint32_t get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
} /* bridged_node */

namespace door_lock {
typedef struct config {
    cluster::identify::config_t identify;
    cluster::door_lock::config_t door_lock;
} config_t;

uint32_t get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
} /* door_lock */

namespace temperature_sensor {
typedef struct config {
    cluster::identify::config_t identify;
    cluster::temperature_measurement::config_t temperature_measurement;
} config_t;

uint32_t get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
} /* temperature_sensor */

namespace occupancy_sensor {
typedef struct config {
    cluster::identify::config_t identify;
    cluster::occupancy_sensing::config_t occupancy_sensing;
} config_t;

uint32_t get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
} /* occupancy_sensor */

namespace contact_sensor {
typedef struct config {
    cluster::identify::config_t identify;
    cluster::boolean_state::config_t boolean_state;
} config_t;

uint32_t get_device_type_id();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
} /* contact_sensor */

} /* endpoint */

namespace node {
/** Standard node create
 *
 * This creates the node, sets the attribute and the identification callbacks and also adds the root node device type, which
 * is by default added to endpoint 0 (since this is the first endpoint which is created).
 */

typedef struct config {
    endpoint::root_node::config_t root_node;
} config_t;

node_t *create(config_t *config, attribute::callback_t attribute_callback,
               identification::callback_t identify_callback);

} /* node */
} /* esp_matter */
