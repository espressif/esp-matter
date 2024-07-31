// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_check.h>
#include <esp_log.h>
#include <esp_matter_bridge.h>
#include <esp_matter_console_bridge.h>
#include <esp_matter_endpoint.h>
#include <esp_matter_mem.h>
#include <string.h>

using namespace esp_matter::endpoint;
namespace esp_matter {
namespace console {
static const char *TAG = "esp_matter_console_bridge";
static engine bridge_console;

typedef struct cli_bridged_device {
    esp_matter_bridge::device_t *device;
    struct cli_bridged_device *next;
} cli_bridged_device_t;

static cli_bridged_device_t *cli_device = NULL;

template <typename config, esp_err_t (*add)(esp_matter::endpoint_t *ep, config *)>
esp_err_t add_device(esp_matter::endpoint_t *ep)
{
    config _config;
    return add(ep, &_config);
}

struct device_type_handler {
    const char *device_type_name;
    uint32_t device_type_id;
    esp_err_t (*add_fun)(esp_matter::endpoint_t *ep);
};

#define ADD_DEVICE_FUN(type) add_device<type::config_t, type::add>

// Add device_handlers to support more device types
const device_type_handler device_handlers[] = {
    {"on_off_light", ESP_MATTER_ON_OFF_LIGHT_DEVICE_TYPE_ID, ADD_DEVICE_FUN(on_off_light)},
    {"dimmable_light", ESP_MATTER_DIMMABLE_LIGHT_DEVICE_TYPE_ID, ADD_DEVICE_FUN(dimmable_light)},
    {"on_off_switch", ESP_MATTER_ON_OFF_SWITCH_DEVICE_TYPE_ID, ADD_DEVICE_FUN(on_off_switch)},
    {"occupancy_sensor", ESP_MATTER_OCCUPANCY_SENSOR_DEVICE_TYPE_ID, ADD_DEVICE_FUN(occupancy_sensor)},
    {"on_off_plugin_unit", ESP_MATTER_ON_OFF_PLUGIN_UNIT_DEVICE_TYPE_ID, ADD_DEVICE_FUN(on_off_plugin_unit)},
    {"color_temperature_light", ESP_MATTER_COLOR_TEMPERATURE_LIGHT_DEVICE_TYPE_ID, ADD_DEVICE_FUN(color_temperature_light)},
    {"extended_color_light", ESP_MATTER_EXTENDED_COLOR_LIGHT_DEVICE_TYPE_ID, ADD_DEVICE_FUN(extended_color_light)},
    {"thermostat", ESP_MATTER_THERMOSTAT_DEVICE_TYPE_ID, ADD_DEVICE_FUN(thermostat)},
    {"temperature_sensor", ESP_MATTER_TEMPERATURE_SENSOR_DEVICE_TYPE_ID, ADD_DEVICE_FUN(temperature_sensor)},
};

static bool is_device_type_supported(uint32_t device_type_id) {
    for (const auto &handler : device_handlers) {
        if (handler.device_type_id == device_type_id) {
            return true;
        }
    }
    return false;
}

static esp_err_t reset_bridge_handler(int argc, char *argv[])
{
    ESP_RETURN_ON_FALSE(argc == 0, ESP_ERR_INVALID_ARG, TAG, "Incorrect arguments");
    esp_matter_bridge::factory_reset();
    esp_matter::factory_reset();
    return ESP_OK;
}

static esp_err_t list_endpoint_handler(int argc, char *argv[])
{
    ESP_RETURN_ON_FALSE(argc == 0, ESP_ERR_INVALID_ARG, TAG, "Incorrect arguments");
    uint16_t matter_endpoint_id_array[MAX_BRIDGED_DEVICE_COUNT];
    ESP_RETURN_ON_ERROR(esp_matter_bridge::get_bridged_endpoint_ids(matter_endpoint_id_array), TAG, 
                        "Failed to get bridged endpoint id");
    ESP_LOGI(TAG, "Bridged endpoint id:");
    uint8_t count = 0;
    for (size_t idx = 0; idx < MAX_BRIDGED_DEVICE_COUNT; ++idx) {
        if (matter_endpoint_id_array[idx] != chip::kInvalidEndpointId) {
            ESP_LOGI(TAG, "Device %d endpoint id: %d", ++count, matter_endpoint_id_array[idx]);
        }
    }
    if (count == 0) {
        ESP_LOGI(TAG, "No bridged device");
    }
    return ESP_OK;
}

static esp_err_t list_support_handler(int argc, char *argv[])
{
    ESP_RETURN_ON_FALSE(argc == 0, ESP_ERR_INVALID_ARG, TAG, "Incorrect arguments");
    ESP_LOGI(TAG, "Supported device type:");
    for (const auto &handler : device_handlers) {
        ESP_LOGI(TAG, "Device type id 0x%04" PRIX32 ", name: %s", handler.device_type_id, handler.device_type_name);
    }
    return ESP_OK;
}

static esp_err_t remove_bridge_device_handler(int argc, char *argv[])
{
    ESP_RETURN_ON_FALSE(argc == 1, ESP_ERR_INVALID_ARG, TAG, "Incorrect arguments");
    uint16_t endpoint_id = (uint16_t)strtoul(argv[0], NULL, 0);
    cli_bridged_device_t *previous_device = NULL;
    cli_bridged_device_t *current_device = cli_device;
    while (current_device) {
        if (current_device->device->persistent_info.device_endpoint_id == endpoint_id) {
            break;
        }
        previous_device = current_device;
        current_device = current_device->next;
    }
    if (!current_device) {
        ESP_LOGE(TAG, "No endpoint was found with given endpoint id 0x%04" PRIX16, endpoint_id);
        list_endpoint_handler(0, NULL);
        return ESP_ERR_NOT_FOUND;
    }

    ESP_RETURN_ON_ERROR(esp_matter_bridge::remove_device(current_device->device), TAG,
                        "Failed to remove bridged device");
    if (!previous_device) {
        cli_device = current_device->next;
    } else {
        previous_device->next = current_device->next;
    }
    esp_matter_mem_free(current_device);
    ESP_LOGI(TAG, "Removed bridged device (endpoint id 0x%04" PRIX16 ") successfully", endpoint_id);
    return ESP_OK;
}

static esp_err_t add_bridge_device_handler(int argc, char *argv[])
{
    ESP_RETURN_ON_FALSE(argc == 2, ESP_ERR_INVALID_ARG, TAG, "Incorrect arguments");
    node_t *node = node::get();
    uint16_t parent_endpoint_id = (uint16_t)strtoul(argv[0], NULL, 0);
    uint32_t device_type_id = strtoul(argv[1], NULL, 0);

    if (!is_device_type_supported(device_type_id)) {
        ESP_LOGE(TAG, "Device type 0x%04" PRIX32 " is unsupported", device_type_id);
        list_support_handler(0, NULL);
        return ESP_ERR_INVALID_ARG;
    }

    cli_bridged_device_t *new_cli_dev = (cli_bridged_device_t *)esp_matter_mem_calloc(1, sizeof(cli_bridged_device_t));
    ESP_RETURN_ON_FALSE(new_cli_dev != NULL, ESP_ERR_NO_MEM, TAG, "Failed to allocate memory for bridged device");

    new_cli_dev->device = esp_matter_bridge::create_device(node, parent_endpoint_id, device_type_id, NULL);
    if (!new_cli_dev->device) {
        ESP_LOGE(TAG, "Failed to create bridged device");
        esp_matter_mem_free(new_cli_dev);
        return ESP_ERR_NO_MEM;
    }

    if (esp_matter::endpoint::enable(new_cli_dev->device->endpoint) != ESP_OK) {
        esp_matter_bridge::remove_device(new_cli_dev->device);
        esp_matter_mem_free(new_cli_dev);
        ESP_LOGE(TAG, "Failed to enable endpoint");
        return ESP_ERR_INVALID_STATE;
    }

    new_cli_dev->next = cli_device;
    cli_device = new_cli_dev;
    ESP_LOGI(TAG, "Created bridged device (endpoint id 0x%04" PRIX16 ") successfully",
             new_cli_dev->device->persistent_info.device_endpoint_id);
    return ESP_OK;
}

static esp_err_t device_type_callback(esp_matter::endpoint_t *ep, uint32_t device_type_id, void *priv_data)
{
    for (const auto &handler : device_handlers) {
        if (handler.device_type_id == device_type_id) {
            return handler.add_fun(ep);
        }
    }

    ESP_LOGE(TAG, "Unsupported bridged device type");
    return ESP_ERR_INVALID_ARG;
}

static esp_err_t bridge_initialize()
{
    node_t *node = node::get();
    esp_err_t err = esp_matter_bridge::initialize(node, device_type_callback);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize the esp_matter_bridge");
        return err;
    }

    uint16_t matter_endpoint_id_array[MAX_BRIDGED_DEVICE_COUNT];
    esp_matter_bridge::get_bridged_endpoint_ids(matter_endpoint_id_array);
    for (size_t idx = 0; idx < MAX_BRIDGED_DEVICE_COUNT; ++idx) {
        if (matter_endpoint_id_array[idx] != chip::kInvalidEndpointId) {
            cli_bridged_device_t *new_cli_dev =
                (cli_bridged_device_t *)esp_matter_mem_calloc(1, sizeof(cli_bridged_device_t));
            if (!(new_cli_dev)) {
                ESP_LOGE(TAG, "Failed to allocate memory for bridged device");
                return ESP_ERR_NO_MEM;
            }

            new_cli_dev->device = esp_matter_bridge::resume_device(node, matter_endpoint_id_array[idx], NULL);
            if (!(new_cli_dev->device)) {
                ESP_LOGE(TAG, "Failed to resume the bridged device");
                esp_matter_mem_free(new_cli_dev);
                continue;
            }

            if (esp_matter::endpoint::enable(new_cli_dev->device->endpoint) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to enable endpoint");
                esp_matter_bridge::remove_device(new_cli_dev->device);
                esp_matter_mem_free(new_cli_dev);
                continue;
            }
            new_cli_dev->next = cli_device;
            cli_device = new_cli_dev;
            ESP_LOGI(TAG, "Resume endpoint 0x%04" PRIX16, matter_endpoint_id_array[idx]);
        }
    }
    return ESP_OK;
}

static esp_err_t bridge_dispatch(int argc, char *argv[])
{
    if (argc <= 0) {
        bridge_console.for_each_command(print_description, NULL);
        return ESP_OK;
    }
    return bridge_console.exec_command(argc, argv);
}

esp_err_t bridge_register_commands()
{
    // Initialize bridge and try to resume bridged device
    bridge_initialize();

    static const command_t command = {
        .name = "bridge",
        .description = "Bridge commands. Usage: matter esp bridge <bridge_command>.",
        .handler = bridge_dispatch,
    };

    static const command_t bridge_commands[] = {
        {
            .name = "add",
            .description =
                "Add matter bridged device. Usage: matter esp bridge add <parent_endpoint_id> <device_type_id>.",
            .handler = add_bridge_device_handler,
        },
        {
            .name = "remove",
            .description = "Remove matter bridged device. Usage: matter esp bridge remove <endpoint_id>.",
            .handler = remove_bridge_device_handler,
        },
        {
            .name = "list",
            .description = "List bridged device endpoint id now. Usage: matter esp bridge list.",
            .handler = list_endpoint_handler,
        },
        {
            .name = "support",
            .description = "List supported device type. Usage: matter esp bridge support.",
            .handler = list_support_handler,
        },
        {
            .name = "reset",
            .description = "reset bridge. Usage: matter esp bridge reset.",
            .handler = reset_bridge_handler,
        }};
    bridge_console.register_commands(bridge_commands, sizeof(bridge_commands) / sizeof(command_t));
    return add_commands(&command, 1);
}
} // namespace console
} // namespace esp_matter
