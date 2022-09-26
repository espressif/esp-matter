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

#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_endpoint.h>

/* Replace these with IDs from submodule whenever they are implemented */
#define ESP_MATTER_ROOT_NODE_DEVICE_TYPE_ID 0x0016
#define ESP_MATTER_BRIDGE_DEVICE_TYPE_ID 0x000E
#define ESP_MATTER_BRIDGED_NODE_DEVICE_TYPE_ID 0x0013

#define ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID 0x0100
#define ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_ID 0x0101
#define ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_ID 0x010C
#define ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_ID 0x010D

#define ESP_MATTER_ON_OFF_SWITCH_DEVICE_TYPE_ID 0x0103
#define ESP_MATTER_DIMMER_SWITCH_DEVICE_TYPE_ID 0x0104
#define ESP_MATTER_COLOR_DIMMER_SWITCH_DEVICE_TYPE_ID 0x0105

#define ESP_MATTER_ON_OFF_PLUGIN_UNIT_DEVICE_TYPE_ID 0x010A
#define ESP_MATTER_DIMMABLE_PLUGIN_UNIT_DEVICE_TYPE_ID 0x010B

#define ESP_MATTER_TEMPERATURE_SENSOR_DEVICE_TYPE_ID 0x0302
#define ESP_MATTER_OCCUPANCY_SENSOR_DEVICE_TYPE_ID 0x0107
#define ESP_MATTER_CONTACT_SENSOR_DEVICE_TYPE_ID 0x0015

#define ESP_MATTER_FAN_DEVICE_TYPE_ID 0x002B
#define ESP_MATTER_THERMOSTAT_DEVICE_TYPE_ID 0x0301
#define ESP_MATTER_DOOR_LOCK_DEVICE_TYPE_ID 0x000A

static const char *TAG = "esp_matter_endpoint";

namespace esp_matter {
using namespace cluster;

namespace endpoint {
namespace root_node {
uint32_t get_device_type_id()
{
    return ESP_MATTER_ROOT_NODE_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id(), 1);

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    access_control::create(endpoint, CLUSTER_FLAG_SERVER);
    basic::create(endpoint, &(config->basic), CLUSTER_FLAG_SERVER);
    general_commissioning::create(endpoint, &(config->general_commissioning), CLUSTER_FLAG_SERVER);
    network_commissioning::create(endpoint, &(config->network_commissioning), CLUSTER_FLAG_SERVER);
    general_diagnostics::create(endpoint, &(config->general_diagnostics), CLUSTER_FLAG_SERVER);
    administrator_commissioning::create(endpoint, &(config->administrator_commissioning), CLUSTER_FLAG_SERVER);
    operational_credentials::create(endpoint, &(config->operational_credentials), CLUSTER_FLAG_SERVER);
    group_key_management::create(endpoint, CLUSTER_FLAG_SERVER);

#if CHIP_DEVICE_CONFIG_ENABLE_WIFI
    diagnostics_network_wifi::create(endpoint, &(config->diagnostics_network_wifi), CLUSTER_FLAG_SERVER);
#endif
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
    diagnostics_network_thread::create(endpoint, &(config->diagnostics_network_thread), CLUSTER_FLAG_SERVER);
#endif

    return endpoint;
}
} /* root_node */

namespace on_off_light {
uint32_t get_device_type_id()
{
    return ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id(), 2);

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    scenes::create(endpoint, &(config->scenes), CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_SERVER, on_off::feature::lighting::get_id());

    return endpoint;
}
} /* on_off_light */

namespace dimmable_light {
uint32_t get_device_type_id()
{
    return ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id(), 2);

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    scenes::create(endpoint, &(config->scenes), CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_SERVER, on_off::feature::lighting::get_id());
    level_control::create(endpoint, &(config->level_control), CLUSTER_FLAG_SERVER,
                          level_control::feature::on_off::get_id() | level_control::feature::lighting::get_id());

    return endpoint;
}
} /* dimmable_light */

namespace color_temperature_light {
uint32_t get_device_type_id()
{
    return ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id(), 2);

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    scenes::create(endpoint, &(config->scenes), CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_SERVER, on_off::feature::lighting::get_id());
    level_control::create(endpoint, &(config->level_control), CLUSTER_FLAG_SERVER,
                          level_control::feature::on_off::get_id() | level_control::feature::lighting::get_id());
    color_control::create(endpoint, &(config->color_control), CLUSTER_FLAG_SERVER,
                          color_control::feature::color_temperature::get_id());

    return endpoint;
}
} /* color_temperature_light */

namespace extended_color_light {
uint32_t get_device_type_id()
{
    return ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id(), 2);

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    scenes::create(endpoint, &(config->scenes), CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_SERVER, on_off::feature::lighting::get_id());
    level_control::create(endpoint, &(config->level_control), CLUSTER_FLAG_SERVER,
                          level_control::feature::on_off::get_id() | level_control::feature::lighting::get_id());
    color_control::create(endpoint, &(config->color_control), CLUSTER_FLAG_SERVER,
                          color_control::feature::color_temperature::get_id() | color_control::feature::xy::get_id());

    return endpoint;
}
} /* extended_color_light */

namespace on_off_switch {
uint32_t get_device_type_id()
{
    return ESP_MATTER_ON_OFF_SWITCH_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id(), 2);

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER | CLUSTER_FLAG_CLIENT);
    binding::create(endpoint, &(config->binding), CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, NULL, CLUSTER_FLAG_CLIENT, ESP_MATTER_NONE_FEATURE_ID);

    return endpoint;
}
} /* on_off_switch */

namespace dimmer_switch {
uint32_t get_device_type_id()
{
    return ESP_MATTER_DIMMER_SWITCH_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id(), 2);

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER | CLUSTER_FLAG_CLIENT);
    binding::create(endpoint, &(config->binding), CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, NULL, CLUSTER_FLAG_CLIENT, ESP_MATTER_NONE_FEATURE_ID);
    level_control::create(endpoint, NULL, CLUSTER_FLAG_CLIENT, ESP_MATTER_NONE_FEATURE_ID);

    return endpoint;
}
} /* dimmer_switch */

namespace color_dimmer_switch {
uint32_t get_device_type_id()
{
    return ESP_MATTER_COLOR_DIMMER_SWITCH_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id(), 2);

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER | CLUSTER_FLAG_CLIENT);
    binding::create(endpoint, &(config->binding), CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, NULL, CLUSTER_FLAG_CLIENT, ESP_MATTER_NONE_FEATURE_ID);
    level_control::create(endpoint, NULL, CLUSTER_FLAG_CLIENT, ESP_MATTER_NONE_FEATURE_ID);
    color_control::create(endpoint, NULL, CLUSTER_FLAG_CLIENT, ESP_MATTER_NONE_FEATURE_ID);

    return endpoint;
}
} /* color_dimmer_switch */

namespace on_off_plugin_unit {
uint32_t get_device_type_id()
{
    return ESP_MATTER_ON_OFF_PLUGIN_UNIT_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id(), 2);

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    scenes::create(endpoint, &(config->scenes), CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_SERVER, on_off::feature::lighting::get_id());

    return endpoint;
}
} /* on_off_plugin_unit */

namespace dimmable_plugin_unit {
uint32_t get_device_type_id()
{
    return ESP_MATTER_DIMMABLE_PLUGIN_UNIT_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id(), 2);

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    scenes::create(endpoint, &(config->scenes), CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_SERVER, on_off::feature::lighting::get_id());
    level_control::create(endpoint, &(config->level_control), CLUSTER_FLAG_SERVER,
                          level_control::feature::on_off::get_id() | level_control::feature::lighting::get_id());

    return endpoint;
}
} /* dimmable_plugin_unit */

namespace fan {
uint32_t get_device_type_id()
{
    return ESP_MATTER_FAN_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id(), 1);

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    fan_control::create(endpoint, &(config->fan_control), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* fan */

namespace thermostat {
uint32_t get_device_type_id()
{
    return ESP_MATTER_THERMOSTAT_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id(), 2);

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    cluster::thermostat::create(endpoint, &(config->thermostat), CLUSTER_FLAG_SERVER);
    time_synchronization::create(endpoint, &(config->time_synchronization), CLUSTER_FLAG_SERVER | CLUSTER_FLAG_CLIENT);

    return endpoint;
}
} /* thermostat */

namespace bridge {
uint32_t get_device_type_id()
{
    return ESP_MATTER_BRIDGE_DEVICE_TYPE_ID;
}
} /* bridge */

namespace bridged_node {
uint32_t get_device_type_id()
{
    return ESP_MATTER_BRIDGED_NODE_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    // bridged node endpoints are always deletable
    endpoint_t *endpoint = endpoint::create(node, flags | ENDPOINT_FLAG_DESTROYABLE, priv_data);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }

    set_device_type_id(endpoint, get_device_type_id(), 1);

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    bridged_device_basic::create(endpoint, &(config->bridged_device_basic), CLUSTER_FLAG_SERVER);
    fixed_label::create(endpoint, &(config->fixed_label), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* bridged_node */

namespace door_lock {
uint32_t get_device_type_id()
{
    return ESP_MATTER_DOOR_LOCK_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }

    set_device_type_id(endpoint, get_device_type_id(), 2);

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    cluster::door_lock::create(endpoint, &(config->door_lock), CLUSTER_FLAG_SERVER);
    time_synchronization::create(endpoint, &(config->time_synchronization), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* door_lock */

namespace temperature_sensor {
uint32_t get_device_type_id()
{
    return ESP_MATTER_TEMPERATURE_SENSOR_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id(), 2);

    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER | CLUSTER_FLAG_CLIENT);
    groups::create(endpoint, NULL, CLUSTER_FLAG_CLIENT);
    temperature_measurement::create(endpoint, &(config->temperature_measurement), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* temperature_sensor */

namespace occupancy_sensor {
uint32_t get_device_type_id()
{
    return ESP_MATTER_OCCUPANCY_SENSOR_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id(), 2);

    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER | CLUSTER_FLAG_CLIENT);
    occupancy_sensing::create(endpoint, &(config->occupancy_sensing), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* occupancy_sensor */

namespace contact_sensor {
uint32_t get_device_type_id()
{
    return ESP_MATTER_CONTACT_SENSOR_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id(), 1);

    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER | CLUSTER_FLAG_CLIENT);
    boolean_state::create(endpoint, &(config->boolean_state), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* contact_sensor */
} /* endpoint */

namespace node {

node_t *create(config_t *config, attribute::callback_t attribute_callback,
               identification::callback_t identification_callback)
{
    attribute::set_callback(attribute_callback);
    identification::set_callback(identification_callback);

    node_t *node = create_raw();
    if (!node) {
        ESP_LOGE(TAG, "Could not create node");
        return NULL;
    }

    endpoint::root_node::create(node, &(config->root_node), ENDPOINT_FLAG_NONE, NULL);

    return node;
}

} /* node */
} /* esp_matter */
