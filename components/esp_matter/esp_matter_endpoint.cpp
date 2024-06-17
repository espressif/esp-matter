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

static const char *TAG = "esp_matter_endpoint";

namespace esp_matter {
using namespace cluster;

namespace endpoint {
namespace root_node {
uint32_t get_device_type_id()
{
    return ESP_MATTER_ROOT_NODE_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_ROOT_NODE_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    access_control::create(endpoint, CLUSTER_FLAG_SERVER);
    basic_information::create(endpoint, &(config->basic_information), CLUSTER_FLAG_SERVER);
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

uint8_t get_device_type_version()
{
    return ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    cluster_t *identify_cluster = identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    identify::command::create_trigger_effect(identify_cluster);
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

uint8_t get_device_type_version()
{
    return ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    cluster_t *identify_cluster = identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    identify::command::create_trigger_effect(identify_cluster);
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

uint8_t get_device_type_version()
{
    return ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
     if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    cluster_t *identify_cluster = identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    identify::command::create_trigger_effect(identify_cluster);
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

uint8_t get_device_type_version()
{
    return ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    cluster_t *identify_cluster = identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    identify::command::create_trigger_effect(identify_cluster);
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

uint8_t get_device_type_version()
{
    return ESP_MATTER_ON_OFF_SWITCH_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

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

uint8_t get_device_type_version()
{
    return ESP_MATTER_DIMMER_SWITCH_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

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

uint8_t get_device_type_version()
{
    return ESP_MATTER_COLOR_DIMMER_SWITCH_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER | CLUSTER_FLAG_CLIENT);
    binding::create(endpoint, &(config->binding), CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, NULL, CLUSTER_FLAG_CLIENT, ESP_MATTER_NONE_FEATURE_ID);
    level_control::create(endpoint, NULL, CLUSTER_FLAG_CLIENT, ESP_MATTER_NONE_FEATURE_ID);
    color_control::create(endpoint, NULL, CLUSTER_FLAG_CLIENT, ESP_MATTER_NONE_FEATURE_ID);

    return endpoint;
}
} /* color_dimmer_switch */

namespace generic_switch {
uint32_t get_device_type_id()
{
    return ESP_MATTER_GENERIC_SWITCH_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_GENERIC_SWITCH_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    switch_cluster::create(endpoint, &(config->switch_cluster), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* color_dimmer_switch */

namespace on_off_plugin_unit {
uint32_t get_device_type_id()
{
    return ESP_MATTER_ON_OFF_PLUGIN_UNIT_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_ON_OFF_PLUGIN_UNIT_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    cluster_t *identify_cluster = identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    identify::command::create_trigger_effect(identify_cluster);
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

uint8_t get_device_type_version()
{
    return ESP_MATTER_DIMMABLE_PLUGIN_UNIT_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    cluster_t *identify_cluster = identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    identify::command::create_trigger_effect(identify_cluster);
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

uint8_t get_device_type_version()
{
    return ESP_MATTER_FAN_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

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

uint8_t get_device_type_version()
{
    return ESP_MATTER_THERMOSTAT_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    cluster::thermostat::create(endpoint, &(config->thermostat), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* thermostat */

namespace aggregator {
uint32_t get_device_type_id()
{
    return ESP_MATTER_AGGREGATOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_AGGREGATOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint);
}

endpoint_t *add(endpoint_t *endpoint)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

    descriptor::create(endpoint,CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* aggregator */

namespace bridged_node {
uint32_t get_device_type_id()
{
    return ESP_MATTER_BRIDGED_NODE_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_BRIDGED_NODE_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    // bridged node endpoints are always deletable
    endpoint_t *endpoint = endpoint::create(node, flags | ENDPOINT_FLAG_DESTROYABLE, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }

    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    bridged_device_basic_information::create(endpoint, &(config->bridged_device_basic_information), CLUSTER_FLAG_SERVER);

    return endpoint;
}

endpoint_t *resume(node_t *node, config_t *config, uint8_t flags, uint16_t endpoint_id, void *priv_data)
{
    endpoint_t *endpoint = endpoint::resume(node, flags | ENDPOINT_FLAG_DESTROYABLE, endpoint_id, priv_data);
    return add(endpoint, config);
}
} /* bridged_node */

namespace door_lock {
uint32_t get_device_type_id()
{
    return ESP_MATTER_DOOR_LOCK_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_DOOR_LOCK_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }

    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    cluster::door_lock::create(endpoint, &(config->door_lock), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* door_lock */

namespace window_covering_device {
uint32_t get_device_type_id()
{
    return ESP_MATTER_WINDOW_COVERING_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_WINDOW_COVERING_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void * priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }

    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    scenes::create(endpoint, &(config->scenes), CLUSTER_FLAG_SERVER);
    window_covering::create(endpoint, &(config->window_covering), CLUSTER_FLAG_SERVER, ESP_MATTER_NONE_FEATURE_ID);

    return endpoint;
}
} /* window_covering */

namespace temperature_sensor {
uint32_t get_device_type_id()
{
    return ESP_MATTER_TEMPERATURE_SENSOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_TEMPERATURE_SENSOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    temperature_measurement::create(endpoint, &(config->temperature_measurement), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* temperature_sensor */

namespace humidity_sensor {
uint32_t get_device_type_id()
{
    return ESP_MATTER_HUMIDITY_SENSOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_HUMIDITY_SENSOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    relative_humidity_measurement::create(endpoint, &(config->relative_humidity_measurement), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* humidity_sensor */

namespace occupancy_sensor {
uint32_t get_device_type_id()
{
    return ESP_MATTER_OCCUPANCY_SENSOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_OCCUPANCY_SENSOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    occupancy_sensing::create(endpoint, &(config->occupancy_sensing), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* occupancy_sensor */

namespace contact_sensor {
uint32_t get_device_type_id()
{
    return ESP_MATTER_CONTACT_SENSOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_CONTACT_SENSOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    boolean_state::create(endpoint, &(config->boolean_state), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* contact_sensor */

namespace light_sensor {
uint32_t get_device_type_id()
{
    return ESP_MATTER_LIGHT_SENSOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_LIGHT_SENSOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    illuminance_measurement::create(endpoint, &(config->illuminance_measurement), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* light_sensor */

namespace pressure_sensor {
uint32_t get_device_type_id()
{
    return ESP_MATTER_PRESSURE_SENSOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_PRESSURE_SENSOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    pressure_measurement::create(endpoint, &(config->pressure_measurement), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* pressure_sensor */

namespace flow_sensor {
uint32_t get_device_type_id()
{
    return ESP_MATTER_FLOW_SENSOR_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_FLOW_SENSOR_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return NULL;
    }
    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    flow_measurement::create(endpoint, &(config->flow_measurement), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* flow_sensor */

namespace pump{

uint32_t get_device_type_id()
{
    return ESP_MATTER_PUMP_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_PUMP_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    return add(endpoint, config);
}

endpoint_t *add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    add_device_type(endpoint, get_device_type_id(), get_device_type_version());

    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_SERVER, ESP_MATTER_NONE_FEATURE_ID);
    pump_configuration_and_control::create(endpoint, &(config->pump_configuration_and_control), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /** pump **/

namespace mode_select_device {
uint32_t get_device_type_id()
{
    return ESP_MATTER_MODE_SELECT_DEVICE_TYPE_ID;
}

uint8_t get_device_type_version()
{
    return ESP_MATTER_MODE_SELECT_DEVICE_TYPE_VERSION;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
{
    endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
    add(endpoint, config);
    return endpoint;
}

esp_err_t add(endpoint_t *endpoint, config_t *config)
{
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t err = add_device_type(endpoint, get_device_type_id(), get_device_type_version());
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add device type id:%" PRIu32 ",err: %d", get_device_type_id(), err);
        return err;
    }

    cluster_t *cluster = descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    if (!cluster) {
        return ESP_ERR_INVALID_STATE;
    }
    cluster = mode_select::create(endpoint, &(config->mode_select), CLUSTER_FLAG_SERVER, ESP_MATTER_NONE_FEATURE_ID);
    if (!cluster) {
        return ESP_ERR_INVALID_STATE;
    }

    return ESP_OK;
}
} /** mode_select_device **/

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
