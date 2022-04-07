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
#define ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID 0x0100
#define ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_ID 0x0101
#define ESP_MATTER_COLOR_DIMMABLE_LIGHT_DEVICE_TYPE_ID 0x0102
#define ESP_MATTER_ON_OFF_SWITCH_DEVICE_TYPE_ID 0x0103
#define ESP_MATTER_FAN_DEVICE_TYPE_ID 0x002B
#define ESP_MATTER_THERMOSTAT_DEVICE_TYPE_ID 0x0301
#define ESP_MATTER_BRIDGED_NODE_DEVICE_TYPE_ID 0x0013
#define ESP_MATTER_DOOR_LOCK_DEVICE_TYPE_ID 0x000A
#define ESP_MATTER_TEMPERATURE_SENSOR_DEVICE_TYPE_ID 0x0302

static const char *TAG = "esp_matter_endpoint";

namespace esp_matter {
using namespace cluster;

namespace endpoint {
namespace root_node {
int get_device_type_id()
{
    return ESP_MATTER_ROOT_NODE_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags)
{
    endpoint_t *endpoint = endpoint::create(node, flags);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id());

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    access_control::create(endpoint, CLUSTER_FLAG_SERVER);
    basic::create(endpoint, &(config->basic), CLUSTER_FLAG_SERVER);
    ota_provider::create(endpoint, &(config->ota_provider), CLUSTER_FLAG_CLIENT);
    ota_requestor::create(endpoint, &(config->ota_requestor), CLUSTER_FLAG_SERVER);
    general_commissioning::create(endpoint, &(config->general_commissioning), CLUSTER_FLAG_SERVER);
    network_commissioning::create(endpoint, &(config->network_commissioning), CLUSTER_FLAG_SERVER);
    general_diagnostics::create(endpoint, &(config->general_diagnostics), CLUSTER_FLAG_SERVER);
    administrator_commissioning::create(endpoint, &(config->administrator_commissioning), CLUSTER_FLAG_SERVER);
    operational_credentials::create(endpoint, &(config->operational_credentials), CLUSTER_FLAG_SERVER);
    group_key_management::create(endpoint, CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* root_node */

namespace on_off_light {
int get_device_type_id()
{
    return ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags)
{
    endpoint_t *endpoint = endpoint::create(node, flags);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id());

    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    scenes::create(endpoint, &(config->scenes), CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_SERVER, on_off::feature::lighting::get_id());
    basic::create(endpoint, &(config->basic), CLUSTER_FLAG_SERVER);
    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* on_off_light */

namespace dimmable_light {
int get_device_type_id()
{
    return ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags)
{
    endpoint_t *endpoint = endpoint::create(node, flags);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id());

    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    scenes::create(endpoint, &(config->scenes), CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_SERVER, on_off::feature::lighting::get_id());
    level_control::create(endpoint, &(config->level_control), CLUSTER_FLAG_SERVER,
                          level_control::feature::on_off::get_id() | level_control::feature::lighting::get_id());
    basic::create(endpoint, &(config->basic), CLUSTER_FLAG_SERVER);
    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* dimmable_light */

namespace color_dimmable_light {
int get_device_type_id()
{
    return ESP_MATTER_COLOR_DIMMABLE_LIGHT_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags)
{
    endpoint_t *endpoint = endpoint::create(node, flags);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id());

    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    scenes::create(endpoint, &(config->scenes), CLUSTER_FLAG_SERVER);
    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_SERVER, on_off::feature::lighting::get_id());
    level_control::create(endpoint, &(config->level_control), CLUSTER_FLAG_SERVER,
                          level_control::feature::on_off::get_id() | level_control::feature::lighting::get_id());
    basic::create(endpoint, &(config->basic), CLUSTER_FLAG_SERVER);
    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    color_control::create(endpoint, &(config->color_control), CLUSTER_FLAG_SERVER,
                          color_control::feature::hue_saturation::get_id());

    return endpoint;
}
} /* color_dimmable_light */

namespace on_off_switch {
int get_device_type_id()
{
    return ESP_MATTER_ON_OFF_SWITCH_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags)
{
    endpoint_t *endpoint = endpoint::create(node, flags);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id());

    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_CLIENT);
    scenes::create(endpoint, &(config->scenes), CLUSTER_FLAG_CLIENT);
    on_off::create(endpoint, &(config->on_off), CLUSTER_FLAG_CLIENT, ESP_MATTER_NONE_FEATURE_ID);
    basic::create(endpoint, &(config->basic), CLUSTER_FLAG_SERVER);
    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    binding::create(endpoint, &(config->binding), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* on_off_switch */

namespace fan {
int get_device_type_id()
{
    return ESP_MATTER_FAN_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags)
{
    endpoint_t *endpoint = endpoint::create(node, flags);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id());

    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    fan_control::create(endpoint, &(config->fan_control), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* fan */

namespace thermostat {
int get_device_type_id()
{
    return ESP_MATTER_THERMOSTAT_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags)
{
    endpoint_t *endpoint = endpoint::create(node, flags);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id());

    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_SERVER);
    scenes::create(endpoint, &(config->scenes), CLUSTER_FLAG_SERVER);
    basic::create(endpoint, &(config->basic), CLUSTER_FLAG_SERVER);
    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    cluster::thermostat::create(endpoint, &(config->thermostat), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* thermostat */

namespace bridged_node {
int get_device_type_id()
{
    return ESP_MATTER_BRIDGED_NODE_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags)
{
    // bridged node endpoints are always deletable
    endpoint_t *endpoint = endpoint::create(node, flags | ENDPOINT_FLAG_DESTROYABLE);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }

    set_device_type_id(endpoint, get_device_type_id());

    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    bridged_device_basic::create(endpoint, &(config->bridged_device_basic), CLUSTER_FLAG_SERVER);
    fixed_label::create(endpoint, &(config->fixed_label), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* bridged_node */

namespace door_lock {
int get_device_type_id()
{
    return ESP_MATTER_DOOR_LOCK_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags)
{
    endpoint_t *endpoint = endpoint::create(node, flags);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }

    set_device_type_id(endpoint, get_device_type_id());

    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
    descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
    cluster::door_lock::create(endpoint, &(config->door_lock), CLUSTER_FLAG_SERVER);
    time_synchronization::create(endpoint, &(config->time_synchronization), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* door_lock */

namespace temperature_sensor {
int get_device_type_id()
{
    return ESP_MATTER_TEMPERATURE_SENSOR_DEVICE_TYPE_ID;
}

endpoint_t *create(node_t *node, config_t *config, uint8_t flags)
{
    endpoint_t *endpoint = endpoint::create(node, flags);
    if (!endpoint) {
        ESP_LOGE(TAG, "Could not create endpoint");
        return NULL;
    }
    set_device_type_id(endpoint, get_device_type_id());

    identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER | CLUSTER_FLAG_CLIENT);
    groups::create(endpoint, &(config->groups), CLUSTER_FLAG_CLIENT);
    temperature_measurement::create(endpoint, &(config->temperature_measurement), CLUSTER_FLAG_SERVER);

    return endpoint;
}
} /* temperature_sensor */
} /* endpoint */

namespace node {

node_t *create(config_t *config, attribute::callback_t callback, void *priv_data)
{
    attribute::set_callback(callback, priv_data);

    node_t *node = create_raw();
    if (!node) {
        ESP_LOGE(TAG, "Could not create node");
        return NULL;
    }

    endpoint::root_node::create(node, &(config->root_node), ENDPOINT_FLAG_NONE);

    return node;
}

} /* node */
} /* esp_matter */
