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
#define ESP_MATTER_ROOT_NODE_DEVICE_TYPE_VERSION 3
#define ESP_MATTER_OTA_REQUESTOR_DEVICE_TYPE_ID 0x0012
#define ESP_MATTER_OTA_REQUESTOR_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_OTA_PROVIDER_DEVICE_TYPE_ID 0x0014
#define ESP_MATTER_OTA_PROVIDER_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_POWER_SOURCE_DEVICE_TYPE_ID 0x0011
#define ESP_MATTER_POWER_SOURCE_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_AGGREGATOR_DEVICE_TYPE_ID 0x000E
#define ESP_MATTER_AGGREGATOR_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_BRIDGED_NODE_DEVICE_TYPE_ID 0x0013
#define ESP_MATTER_BRIDGED_NODE_DEVICE_TYPE_VERSION 3
#define ESP_MATTER_CONTROL_BRIDGE_DEVICE_TYPE_ID 0x0840
#define ESP_MATTER_CONTROL_BRIDGE_DEVICE_TYPE_VERSION 3

#define ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID 0x0100
#define ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_VERSION 3
#define ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_ID 0x0101
#define ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_VERSION 3
#define ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_ID 0x010C
#define ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_VERSION 4
#define ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_ID 0x010D
#define ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_VERSION 4

#define ESP_MATTER_ON_OFF_SWITCH_DEVICE_TYPE_ID 0x0103
#define ESP_MATTER_ON_OFF_SWITCH_DEVICE_TYPE_VERSION 3
#define ESP_MATTER_DIMMER_SWITCH_DEVICE_TYPE_ID 0x0104
#define ESP_MATTER_DIMMER_SWITCH_DEVICE_TYPE_VERSION 3
#define ESP_MATTER_COLOR_DIMMER_SWITCH_DEVICE_TYPE_ID 0x0105
#define ESP_MATTER_COLOR_DIMMER_SWITCH_DEVICE_TYPE_VERSION 3
#define ESP_MATTER_GENERIC_SWITCH_DEVICE_TYPE_ID 0x000F
#define ESP_MATTER_GENERIC_SWITCH_DEVICE_TYPE_VERSION 3

#define ESP_MATTER_ON_OFF_PLUGIN_UNIT_DEVICE_TYPE_ID 0x010A
#define ESP_MATTER_ON_OFF_PLUGIN_UNIT_DEVICE_TYPE_VERSION 3
#define ESP_MATTER_DIMMABLE_PLUGIN_UNIT_DEVICE_TYPE_ID 0x010B
#define ESP_MATTER_DIMMABLE_PLUGIN_UNIT_DEVICE_TYPE_VERSION 4
#define ESP_MATTER_MOUNTED_ON_OFF_CONTROL_DEVICE_TYPE_ID 0x010F
#define ESP_MATTER_MOUNTED_ON_OFF_CONTROL_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_MOUNTED_DIMMABLE_LOAD_CONTROL_DEVICE_TYPE_ID 0x0110
#define ESP_MATTER_MOUNTED_DIMMABLE_LOAD_CONTROL_DEVICE_TYPE_VERSION 1

#define ESP_MATTER_TEMPERATURE_SENSOR_DEVICE_TYPE_ID 0x0302
#define ESP_MATTER_TEMPERATURE_SENSOR_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_OCCUPANCY_SENSOR_DEVICE_TYPE_ID 0x0107
#define ESP_MATTER_OCCUPANCY_SENSOR_DEVICE_TYPE_VERSION 4
#define ESP_MATTER_CONTACT_SENSOR_DEVICE_TYPE_ID 0x0015
#define ESP_MATTER_CONTACT_SENSOR_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_LIGHT_SENSOR_DEVICE_TYPE_ID 0x0106
#define ESP_MATTER_LIGHT_SENSOR_DEVICE_TYPE_VERSION 3
#define ESP_MATTER_PRESSURE_SENSOR_DEVICE_TYPE_ID 0x0305
#define ESP_MATTER_PRESSURE_SENSOR_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_FLOW_SENSOR_DEVICE_TYPE_ID 0x0306
#define ESP_MATTER_FLOW_SENSOR_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_HUMIDITY_SENSOR_DEVICE_TYPE_ID 0x0307
#define ESP_MATTER_HUMIDITY_SENSOR_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_ROOM_AIR_CONDITIONER_DEVICE_TYPE_ID 0x0072
#define ESP_MATTER_ROOM_AIR_CONDITIONER_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_REFRIGERATOR_DEVICE_TYPE_ID 0x0070
#define ESP_MATTER_REFRIGERATOR_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_TEMPERATURE_CONTROLLED_CABINET_DEVICE_TYPE_ID 0x0071
#define ESP_MATTER_TEMPERATURE_CONTROLLED_CABINET_DEVICE_TYPE_VERSION 3
#define ESP_MATTER_LAUNDRY_WASHER_DEVICE_TYPE_ID 0x0073
#define ESP_MATTER_LAUNDRY_WASHER_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_DISH_WASHER_DEVICE_TYPE_ID 0x0075
#define ESP_MATTER_DISH_WASHER_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_MICROWAVE_OVEN_DEVICE_TYPE_ID 0x0079
#define ESP_MATTER_MICROWAVE_OVEN_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_SMOKE_CO_ALARM_DEVICE_TYPE_ID 0x0076
#define ESP_MATTER_SMOKE_CO_ALARM_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_LAUNDRY_DRYER_DEVICE_TYPE_ID 0x007C
#define ESP_MATTER_LAUNDRY_DRYER_DEVICE_TYPE_VERSION 1

#define ESP_MATTER_FAN_DEVICE_TYPE_ID 0x002B
#define ESP_MATTER_FAN_DEVICE_TYPE_VERSION 3
#define ESP_MATTER_THERMOSTAT_DEVICE_TYPE_ID 0x0301
#define ESP_MATTER_THERMOSTAT_DEVICE_TYPE_VERSION 4
#define ESP_MATTER_AIR_QUALITY_SENSOR_DEVICE_TYPE_ID 0x002C
#define ESP_MATTER_AIR_QUALITY_SENSOR_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_AIR_PURIFIER_DEVICE_TYPE_ID 0x002D
#define ESP_MATTER_AIR_PURIFIER_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_DOOR_LOCK_DEVICE_TYPE_ID 0x000A
#define ESP_MATTER_DOOR_LOCK_DEVICE_TYPE_VERSION 3
#define ESP_MATTER_WINDOW_COVERING_DEVICE_TYPE_ID 0x0202
#define ESP_MATTER_WINDOW_COVERING_DEVICE_TYPE_VERSION 3
#define ESP_MATTER_PUMP_DEVICE_TYPE_ID 0x0303
#define ESP_MATTER_PUMP_DEVICE_TYPE_VERSION 3
#define ESP_MATTER_PUMP_CONTROLLER_DEVICE_TYPE_ID 0x0304
#define ESP_MATTER_PUMP_CONTROLLER_DEVICE_TYPE_VERSION 4
#define ESP_MATTER_MODE_SELECT_DEVICE_TYPE_ID 0x0027
#define ESP_MATTER_MODE_SELECT_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_ROBOTIC_VACCUM_CLEANER_DEVICE_TYPE_ID 0x0074
#define ESP_MATTER_ROBOTIC_VACCUM_CLEANER_DEVICE_TYPE_VERSION 3
#define ESP_MATTER_WATER_LEAK_DETECTOR_DEVICE_TYPE_ID 0x0043
#define ESP_MATTER_WATER_LEAK_DETECTOR_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_RAIN_SENSOR_DEVICE_TYPE_ID 0x0044
#define ESP_MATTER_RAIN_SENSOR_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_COOK_SURFACE_DEVICE_TYPE_ID 0x0077
#define ESP_MATTER_COOK_SURFACE_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_COOKTOP_DEVICE_TYPE_ID 0x0078
#define ESP_MATTER_COOKTOP_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_ELECTRICAL_SENSOR_DEVICE_TYPE_ID 0x0510
#define ESP_MATTER_ELECTRICAL_SENSOR_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_OVEN_DEVICE_TYPE_ID 0x007B
#define ESP_MATTER_OVEN_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_WATER_FREEZE_DETECTOR_DEVICE_TYPE_ID 0x0041
#define ESP_MATTER_WATER_FREEZE_DETECTOR_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_ENERGY_EVSE_DEVICE_TYPE_ID 0x050C
#define ESP_MATTER_ENERGY_EVSE_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_EXTRACTOR_HOOD_DEVICE_TYPE_ID 0x007A
#define ESP_MATTER_EXTRACTOR_HOOD_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_WATER_VALVE_DEVICE_TYPE_ID 0x0042
#define ESP_MATTER_WATER_VALVE_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_DEVICE_ENERGY_MANAGEMENT_DEVICE_TYPE_ID 0x050D
#define ESP_MATTER_DEVICE_ENERGY_MANAGEMENT_DEVICE_TYPE_VERSION 2
#define ESP_MATTER_SECONDARY_NETWORK_INTERFACE_DEVICE_TYPE_ID 0x0019
#define ESP_MATTER_SECONDARY_NETWORK_INTERFACE_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_WATER_HEATER_DEVICE_TYPE_ID 0x050F
#define ESP_MATTER_WATER_HEATER_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_SOLAR_POWER_DEVICE_TYPE_ID 0x0017
#define ESP_MATTER_SOLAR_POWER_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_BATTERY_STORAGE_DEVICE_TYPE_ID 0x0018
#define ESP_MATTER_BATTERY_STORAGE_DEVICE_TYPE_VERSION 1

#define ESP_MATTER_THREAD_BORDER_ROUTER_DEVICE_TYPE_ID 0x0091
#define ESP_MATTER_THREAD_BORDER_ROUTER_DEVICE_TYPE_VERSION 1
#define ESP_MATTER_HEAT_PUMP_DEVICE_TYPE_ID 0x0309
#define ESP_MATTER_HEAT_PUMP_DEVICE_TYPE_VERSION 1

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

typedef struct {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
} app_base_config;

typedef struct : app_base_config {
    cluster::groups::config_t groups;
} app_with_group_config;

typedef struct : app_base_config {
    cluster::binding::config_t binding;
} app_client_config;

typedef struct : app_with_group_config {
    cluster::on_off::config_t on_off;
} on_off_config;

typedef struct : app_with_group_config {
    cluster::scenes_management::config_t scenes_management;
    cluster::on_off::config_t on_off;
} on_off_with_scenes_management_config;

typedef struct : app_base_config {
    cluster::boolean_state::config_t boolean_state;
} app_with_bool_state_config;

typedef struct {
    cluster::descriptor::config_t descriptor;
    cluster::operational_state::config_t operational_state;
} app_with_operational_state_config;

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
    cluster::icd_management::config_t icd_management;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* root_node */

namespace ota_requestor{
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::ota_requestor::config_t ota_requestor;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* ota_requestor */

namespace ota_provider{
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::ota_provider::config_t ota_provider;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* ota_provider */

namespace power_source_device{
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::power_source::config_t power_source;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* power_source_device */

namespace on_off_light {

using config_t = on_off_config;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* on_off_light */

namespace dimmable_light {
typedef struct config : on_off_light::config_t {
    cluster::level_control::config_t level_control;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* dimmable_light */

namespace color_temperature_light {
typedef struct config : dimmable_light::config_t {
    cluster::color_control::config_t color_control;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* color_temperature_light */

namespace extended_color_light {
typedef struct config : dimmable_light::config_t {
    cluster::color_control::config_t color_control;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* extended_color_light */

namespace on_off_switch {

using config_t = app_client_config;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* on_off_switch */

namespace dimmer_switch {

using config_t = app_client_config;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* dimmer_switch */

namespace color_dimmer_switch {

using config_t = app_client_config;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* color_dimmer_switch */

namespace generic_switch {
typedef struct config : app_base_config {
    cluster::switch_cluster::config_t switch_cluster;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* generic_switch */

namespace on_off_plugin_unit {

using config_t = on_off_config;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* on_off_plugin_unit */

namespace dimmable_plugin_unit {
typedef struct config : on_off_plugin_unit::config_t {
    cluster::level_control::config_t level_control;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* dimmable_plugin_unit */

namespace fan {
typedef struct config : app_with_group_config {
    cluster::fan_control::config_t fan_control;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* fan */

namespace thermostat {
typedef struct config : app_with_group_config {
    cluster::scenes_management::config_t scenes_management;
    cluster::thermostat::config_t thermostat;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* thermostat */

namespace air_quality_sensor {
typedef struct config : app_base_config {
    cluster::air_quality::config_t air_quality;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* air_quality_sensor */

namespace air_purifier {
typedef struct config : app_base_config {
    cluster::fan_control::config_t fan_control;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* air_purifier */

namespace dish_washer {
using config_t = app_with_operational_state_config;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* dish_washer */

namespace laundry_washer {
using config_t = app_with_operational_state_config;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* laundry_washer */

namespace laundry_dryer {
using config_t = app_with_operational_state_config;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* laundry_dryer */

namespace smoke_co_alarm {
typedef struct config : app_base_config {
    cluster::smoke_co_alarm::config_t smoke_co_alarm;
} config_t;
uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* smoke_co_alarm */

namespace aggregator {
typedef struct config {
    cluster::descriptor::config_t descriptor;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* aggregator */

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

namespace control_bridge {

using config_t = app_client_config;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* control_bridge */

namespace door_lock {
typedef struct config : app_base_config {
    cluster::door_lock::config_t door_lock;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* door_lock */

namespace window_covering_device {
typedef struct config : app_with_group_config {
    cluster::scenes_management::config_t scenes_management;
    cluster::window_covering::config_t window_covering;
    config(uint8_t end_product_type = 0) : window_covering(end_product_type) {}
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* window_covering */

namespace temperature_sensor {
typedef struct config : app_base_config {
    cluster::temperature_measurement::config_t temperature_measurement;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* temperature_sensor */

namespace humidity_sensor {
typedef struct config : app_base_config {
    cluster::relative_humidity_measurement::config_t relative_humidity_measurement;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* humidity_sensor */

namespace occupancy_sensor {
typedef struct config : app_base_config {
    cluster::occupancy_sensing::config_t occupancy_sensing;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* occupancy_sensor */

namespace contact_sensor {
using config_t = app_with_bool_state_config;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* contact_sensor */

namespace light_sensor {
typedef struct config : app_base_config {
    cluster::illuminance_measurement::config_t illuminance_measurement;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* light_sensor */

namespace pressure_sensor {
typedef struct config : app_base_config {
    cluster::pressure_measurement::config_t pressure_measurement;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* pressure_sensor */

namespace flow_sensor {
typedef struct config : app_base_config {
    cluster::flow_measurement::config_t flow_measurement;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* flow_sensor */

namespace pump{
typedef struct config : app_base_config {
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
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /** pump **/

namespace pump_controller {
typedef struct config : app_client_config {
    cluster::on_off::config_t on_off;
    cluster::pump_configuration_and_control::config_t pump_configuration_and_control;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /** pump_controller **/

namespace mode_select_device {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::mode_select::config_t mode_select;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /** mode_select_device **/

namespace room_air_conditioner{
typedef struct config : app_base_config {
    cluster::on_off::config_t on_off;
    cluster::thermostat::config_t thermostat;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /** room air conditioner **/

namespace temperature_controlled_cabinet{
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::temperature_control::config_t temperature_control;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /** temperature_controlled_cabinet **/

namespace refrigerator{
typedef struct config {
    cluster::descriptor::config_t descriptor;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /** refrigerator **/

namespace oven {
typedef struct config {
    cluster::descriptor::config_t descriptor;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /** oven **/

namespace robotic_vacuum_cleaner{
typedef struct config : app_base_config {
    cluster::rvc_run_mode::config_t rvc_run_mode;
    cluster::rvc_operational_state::config_t rvc_operational_state;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /** robotic_vacuum_cleaner **/

namespace water_leak_detector {
using config_t = app_with_bool_state_config;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* water_leak_detector */

namespace water_freeze_detector {

using config_t = app_with_bool_state_config;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* water_freeze_detector */

namespace rain_sensor {

using config_t = app_with_bool_state_config;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* rain_sensor */

namespace electrical_sensor {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::power_topology::config_t power_topology;
    cluster::electrical_power_measurement::config_t electrical_power_measurement;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* electrical_sensor */

namespace cook_surface {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::temperature_control::config_t temperature_control;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* cook_surface */

namespace cooktop {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::on_off::config_t on_off;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* cooktop */

namespace energy_evse {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::energy_evse::config_t energy_evse;
    cluster::energy_evse_mode::config_t energy_evse_mode;
    cluster::device_energy_management::config_t device_energy_management;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* energy_evse */

namespace microwave_oven {
typedef struct config : app_with_operational_state_config {
    cluster::microwave_oven_mode::config_t microwave_oven_mode;
    cluster::microwave_oven_control::config_t microwave_oven_control;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* microwave_oven */

namespace extractor_hood {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::fan_control::config_t fan_control;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* extractor_hood */

namespace water_valve {
typedef struct config : app_base_config {
    cluster::valve_configuration_and_control::config_t valve_configuration_and_control;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /** water_valve **/

namespace device_energy_management {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::device_energy_management::config_t device_energy_management;
    cluster::device_energy_management_mode::config_t device_energy_management_mode;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* device_energy_management */

namespace thread_border_router {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::thread_network_diagnostics::config_t thread_network_diagnostics;
    cluster::thread_border_router_management::config_t thread_border_router_management;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* thread_border_router */

namespace secondary_network_interface {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::network_commissioning::config_t network_commissioning;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* secondary_network_interface */

namespace mounted_on_off_control {
using config_t = on_off_config;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /** mounted_on_off_control **/

namespace mounted_dimmable_load_control {
using config_t = dimmable_light::config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /** mounted_dimmable_load_control **/

namespace water_heater {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::thermostat::config_t thermostat;
    cluster::water_heater_management::config_t water_heater_management;
    cluster::water_heater_mode::config_t water_heater_mode;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* water_heater */

namespace solar_power {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    power_source_device::config_t power_source_device;
    electrical_sensor::config_t electrical_sensor;
    cluster::electrical_energy_measurement::config_t electrical_energy_measurement;
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* solar_power */

namespace battery_storage {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    power_source_device::config_t power_source_device;
    electrical_sensor::config_t electrical_sensor;
    device_energy_management::config_t device_energy_management;
    cluster::electrical_energy_measurement::config_t electrical_energy_measurement;

    nullable<uint32_t> bat_voltage;
    nullable<uint8_t> bat_percent_remaining;
    nullable<uint32_t> bat_time_remaining;
    uint32_t bat_capacity;
    nullable<uint32_t> bat_time_to_full_charge;
    nullable<uint32_t> bat_charging_current;

    nullable<int64_t> voltage;
    nullable<int64_t> active_current;

    config(): bat_voltage(), bat_percent_remaining(), bat_capacity(0), bat_time_to_full_charge(), bat_charging_current(), voltage(0), active_current(0) {}
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /** battery_storage **/

namespace heat_pump {
typedef struct config {
    cluster::descriptor::config_t descriptor;
    power_source_device::config_t power_source_device;
    electrical_sensor::config_t electrical_sensor;
    device_energy_management::config_t device_energy_management;
    cluster::electrical_energy_measurement::config_t electrical_energy_measurement;

    nullable<int64_t> voltage;
    nullable<int64_t> active_current;

    config(): voltage(0), active_current(0) {}
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /** heat_pump **/

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
 * @param[in] priv_data          Private data to send to the node. This parameter is optional
 *                               and defaults to nullptr.This private data can be accessed in the attribute callback
 *                               for the root endpoint only.
 */
node_t *create(config_t *config, attribute::callback_t attribute_callback,
               identification::callback_t identify_callback, void* priv_data = nullptr);

} /* node */
} /* esp_matter */
