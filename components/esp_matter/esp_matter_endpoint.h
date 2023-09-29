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

/* Replace these with IDs from submodule whenever they are implemented */
#define ESP_MATTER_ROOT_NODE_DEVICE_TYPE_ID 0x0016
#define ESP_MATTER_ROOT_NODE_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_POWER_SOURCE_DEVICE_TYPE_ID 0x0011
#define ESP_MATTER_POWER_SOURCE_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_AGGREGATOR_DEVICE_TYPE_ID 0x000E
#define ESP_MATTER_AGGREGATOR_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_BRIDGED_NODE_DEVICE_TYPE_ID 0x0013
#define ESP_MATTER_BRIDGED_NODE_DEVICE_TYPE_VERSION 1

#define ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID 0x0100
#define ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_ID 0x0101
#define ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_ID 0x010C
#define ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_ID 0x010D
#define ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_VERSION 2

#define ESP_MATTER_ON_OFF_SWITCH_DEVICE_TYPE_ID 0x0103
#define ESP_MATTER_ON_OFF_SWITCH_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_DIMMER_SWITCH_DEVICE_TYPE_ID 0x0104
#define ESP_MATTER_DIMMER_SWITCH_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_COLOR_DIMMER_SWITCH_DEVICE_TYPE_ID 0x0105
#define ESP_MATTER_COLOR_DIMMER_SWITCH_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_GENERIC_SWITCH_DEVICE_TYPE_ID 0x000F
#define ESP_MATTER_GENERIC_SWITCH_DEVICE_TYPE_VERSION 1

#define ESP_MATTER_ON_OFF_PLUGIN_UNIT_DEVICE_TYPE_ID 0x010A
#define ESP_MATTER_ON_OFF_PLUGIN_UNIT_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_DIMMABLE_PLUGIN_UNIT_DEVICE_TYPE_ID 0x010B
#define ESP_MATTER_DIMMABLE_PLUGIN_UNIT_DEVICE_TYPE_VERSION 2

#define ESP_MATTER_TEMPERATURE_SENSOR_DEVICE_TYPE_ID 0x0302
#define ESP_MATTER_TEMPERATURE_SENSOR_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_OCCUPANCY_SENSOR_DEVICE_TYPE_ID 0x0107
#define ESP_MATTER_OCCUPANCY_SENSOR_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_CONTACT_SENSOR_DEVICE_TYPE_ID 0x0015
#define ESP_MATTER_CONTACT_SENSOR_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_LIGHT_SENSOR_DEVICE_TYPE_ID 0x0106
#define ESP_MATTER_LIGHT_SENSOR_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_PRESSURE_SENSOR_DEVICE_TYPE_ID 0x0305
#define ESP_MATTER_PRESSURE_SENSOR_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_FLOW_SENSOR_DEVICE_TYPE_ID 0x0306
#define ESP_MATTER_FLOW_SENSOR_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_HUMIDITY_SENSOR_DEVICE_TYPE_ID 0x0307
#define ESP_MATTER_HUMIDITY_SENSOR_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_ROOM_AIR_CONDITIONER_DEVICE_TYPE_ID 0x0072
#define ESP_MATTER_ROOM_AIR_CONDITIONER_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_REFRIGERATOR_DEVICE_TYPE_ID 0x0070
#define ESP_MATTER_REFRIGERATOR_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_TEMPERATURE_CONTROLLED_CABINET_DEVICE_TYPE_ID 0x0071
#define ESP_MATTER_TEMPERATURE_CONTROLLED_CABINET_DEVICE_TYPE_VERSION 1

#define ESP_MATTER_FAN_DEVICE_TYPE_ID 0x002B
#define ESP_MATTER_FAN_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_THERMOSTAT_DEVICE_TYPE_ID 0x0301
#define ESP_MATTER_THERMOSTAT_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_DOOR_LOCK_DEVICE_TYPE_ID 0x000A
#define ESP_MATTER_DOOR_LOCK_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_WINDOW_COVERING_DEVICE_TYPE_ID 0x0202
#define ESP_MATTER_WINDOW_COVERING_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_PUMP_DEVICE_TYPE_ID 0x0303
#define ESP_MATTER_PUMP_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_MODE_SELECT_DEVICE_TYPE_ID 0x0027
#define ESP_MATTER_MODE_SELECT_DEVICE_TYPE_VERSION 1

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
    cluster::descriptor::config_t descriptor;
    cluster::access_control::config_t access_control;
    cluster::basic_information::config_t basic_information;
    cluster::general_commissioning::config_t general_commissioning;
    cluster::network_commissioning::config_t network_commissioning;
    cluster::general_diagnostics::config_t general_diagnostics;
    cluster::administrator_commissioning::config_t administrator_commissioning;
    cluster::operational_credentials::config_t operational_credentials;
    cluster::diagnostics_network_wifi::config_t diagnostics_network_wifi;
    cluster::diagnostics_network_thread::config_t diagnostics_network_thread;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* root_node */

namespace power_source_device{
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::power_source::config_t power_source;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* power_source_device */

namespace on_off_light {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::groups::config_t groups;
    cluster::scenes::config_t scenes;
    cluster::on_off::config_t on_off;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* on_off_light */

namespace dimmable_light {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::groups::config_t groups;
    cluster::scenes::config_t scenes;
    cluster::on_off::config_t on_off;
    cluster::level_control::config_t level_control;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* dimmable_light */

namespace color_temperature_light {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::groups::config_t groups;
    cluster::scenes::config_t scenes;
    cluster::on_off::config_t on_off;
    cluster::level_control::config_t level_control;
    cluster::color_control::config_t color_control;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* color_temperature_light */

namespace extended_color_light {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::groups::config_t groups;
    cluster::scenes::config_t scenes;
    cluster::on_off::config_t on_off;
    cluster::level_control::config_t level_control;
    cluster::color_control::config_t color_control;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* extended_color_light */

namespace on_off_switch {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::binding::config_t binding;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* on_off_switch */

namespace dimmer_switch {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::binding::config_t binding;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* dimmer_switch */

namespace color_dimmer_switch {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::binding::config_t binding;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* color_dimmer_switch */

namespace generic_switch {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::switch_cluster::config_t switch_cluster;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* generic_switch */

namespace on_off_plugin_unit {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::groups::config_t groups;
    cluster::scenes::config_t scenes;
    cluster::on_off::config_t on_off;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* on_off_plugin_unit */

namespace dimmable_plugin_unit {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::groups::config_t groups;
    cluster::scenes::config_t scenes;
    cluster::on_off::config_t on_off;
    cluster::level_control::config_t level_control;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* dimmable_plugin_unit */

namespace fan {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::groups::config_t groups;
    cluster::fan_control::config_t fan_control;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* fan */

namespace thermostat {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::scenes::config_t scenes;
    cluster::groups::config_t groups;
    cluster::thermostat::config_t thermostat;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* thermostat */

namespace aggregator {
typedef struct config {
    cluster::descriptor::config_t descriptor;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* aggregator */

namespace bridged_node {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::bridged_device_basic_information::config_t bridged_device_basic_information;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
endpoint_t *resume(node_t *node, config_t *config, uint8_t flags, uint16_t endpoint_id, void *priv_data);
} /* bridged_node */

namespace door_lock {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::door_lock::config_t door_lock;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* door_lock */

namespace window_covering_device {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::groups::config_t groups;
    cluster::scenes::config_t scenes;
    cluster::window_covering::config_t window_covering;
    config(uint8_t end_product_type = 0) : window_covering(end_product_type) {}
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* window_covering */

namespace temperature_sensor {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::temperature_measurement::config_t temperature_measurement;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* temperature_sensor */

namespace humidity_sensor {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::relative_humidity_measurement::config_t relative_humidity_measurement;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* humidity_sensor */

namespace occupancy_sensor {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::occupancy_sensing::config_t occupancy_sensing;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* occupancy_sensor */

namespace contact_sensor {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::boolean_state::config_t boolean_state;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* contact_sensor */

namespace light_sensor {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::illuminance_measurement::config_t illuminance_measurement;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* light_sensor */

namespace pressure_sensor {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::pressure_measurement::config_t pressure_measurement;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* pressure_sensor */

namespace flow_sensor {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::flow_measurement::config_t flow_measurement;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /* flow_sensor */

namespace pump{
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::on_off::config_t on_off;
    cluster::pump_configuration_and_control::config_t pump_configuration_and_control;
    config(
        nullable<int16_t> max_pressure = nullable<int16_t>(),
        nullable<uint16_t> max_speed = nullable<uint16_t>(),
        nullable<uint16_t> max_flow = nullable<uint16_t>()
    ) : pump_configuration_and_control(max_pressure, max_speed, max_flow) {}
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /** pump **/

namespace mode_select_device {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::mode_select::config_t mode_select;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /** mode_select_device **/

namespace room_air_conditioner{
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify:: config_t identify;
    cluster::on_off::config_t on_off;
    cluster::thermostat::config_t thermostat;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /** room air conditioner **/

namespace temperature_controlled_cabinet{
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::temperature_control::config_t temperature_control;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /** temperature_controlled_cabinet **/

namespace refrigerator{
typedef struct config {
    cluster::descriptor::config_t descriptor;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
endpoint_t *add(endpoint_t *endpoint, config_t *config);
} /** refrigerator **/

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

/**
 * @param[in] config             Configuration of the root node, a pointer to an object of type `node::config_t`.
 * @param[in] attribute_callback This callback is called for every attribute update. The callback implementation shall
 *                               handle the desired attributes and return an appropriate error code. If the attribute
 *                               is not of your interest, please do not return an error code and strictly return ESP_OK.
 * @param[in] identify_callback  This callback is invoked when clients interact with the Identify Cluster.
 *                               In the callback implementation, an endpoint can identify itself.
 *                               (e.g., by flashing an LED or light).
 */
node_t *create(config_t *config, attribute::callback_t attribute_callback,
               identification::callback_t identify_callback);

} /* node */
} /* esp_matter */
