/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_matter_client.h>
#include <cstddef>
#include <cstdio>
#include <array>
#include <esp_log.h>
#include <stdlib.h>
#include <string.h>

#include <esp_matter.h>
#include <esp_matter_console.h>
#include "bsp/esp-bsp.h"

#include <app_priv.h>
#include <app_reset.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "app_driver";
extern uint16_t switch_endpoint_id;

// Four caller-owned operations preserve rapid edge-triggered button actions.
static std::array<client::invoke_command::interaction, 4> invoke_actions;
static client::binding::dispatcher bound_actions;

static CHIP_ERROR StartInvoke(chip::ScopedNodeId target,
                              const client::invoke_command::request_t &request)
{
    lock::ScopedChipStackLock lock(portMAX_DELAY);
    for (auto &action : invoke_actions) {
        if (!action.active()) {
            return action.start(target, request, nullptr);
        }
    }
    return CHIP_ERROR_BUSY;
}

static CHIP_ERROR StartGroupInvoke(chip::GroupId group_id, chip::FabricIndex fabric_index,
                                   const client::invoke_command::request_t &request)
{
    lock::ScopedChipStackLock lock(portMAX_DELAY);
    for (auto &action : invoke_actions) {
        if (!action.active()) {
            return action.start(group_id, fabric_index, request, nullptr);
        }
    }
    return CHIP_ERROR_BUSY;
}

static CHIP_ERROR StartBoundInvoke(chip::EndpointId local_endpoint,
                                   const client::invoke_command::request_t &request)
{
    lock::ScopedChipStackLock lock(portMAX_DELAY);
    return bound_actions.dispatch(local_endpoint, request, nullptr);
}

static CHIP_ERROR FormatCommandFields(chip::ClusterId cluster_id,
                                      chip::CommandId command_id,
                                      int parameter_count, char **parameters,
                                      char *output, size_t output_size,
                                      bool &has_fields)
{
    VerifyOrReturnError(parameter_count >= 0 && parameter_count <= 10,
                        CHIP_ERROR_INVALID_ARGUMENT);
    for (int index = 0; index < parameter_count; ++index) {
        VerifyOrReturnError(parameters[index][0] == '0' && parameters[index][1] == 'x' &&
                            strlen(parameters[index] + 2) <= 10,
                            CHIP_ERROR_INVALID_ARGUMENT);
    }

    has_fields = false;
    if (cluster_id == OnOff::Id) {
        return CHIP_NO_ERROR;
    }
    VerifyOrReturnError(cluster_id == Identify::Id &&
                        command_id == Identify::Commands::Identify::Id,
                        CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE);
    VerifyOrReturnError(parameter_count == 1, CHIP_ERROR_INVALID_ARGUMENT);
    snprintf(output, output_size, "{\"0:OBJ\":{\"0:U16\": %ld}}",
             strtoul(parameters[0] + 2, NULL, 16));
    has_fields = true;
    return CHIP_NO_ERROR;
}

#if CONFIG_ENABLE_CHIP_SHELL
static esp_err_t app_driver_bound_console_handler(int argc, char **argv)
{
    if (argc == 1 && strncmp(argv[0], "help", sizeof("help")) == 0) {
        printf("Bound commands:\n"
               "\thelp: Print help\n"
               "\tinvoke: <local_endpoint_id> <cluster_id> <command_id> parameters ... \n"
               "\t\tExample: matter esp bound invoke 0x0001 0x0003 0x0000 0x78.\n");
    } else if (argc >= 4 && strncmp(argv[0], "invoke", sizeof("invoke")) == 0) {
        chip::EndpointId local_endpoint_id = strtoul((const char *)&argv[1][2], NULL, 16);
        client::invoke_command::request_t request = {
            .cluster_id = strtoul((const char *)&argv[2][2], NULL, 16),
            .command_id = strtoul((const char *)&argv[3][2], NULL, 16),
        };

        char command_data_str[32] = {};
        bool has_fields;
        CHIP_ERROR err = FormatCommandFields(request.cluster_id, request.command_id,
                                             argc - 4, argv + 4,
                                             command_data_str, sizeof(command_data_str), has_fields);
        if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Unsupported command fields: %s", ErrorStr(err));
            return ESP_ERR_INVALID_ARG;
        }
        client::json_encodable fields(command_data_str);
        request.fields = has_fields ? &fields : nullptr;
        err = StartBoundInvoke(local_endpoint_id, request);
        if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Bound Invoke submission failed: %s", ErrorStr(err));
            return ESP_FAIL;
        }
    } else {
        ESP_LOGE(TAG, "Incorrect arguments. Check help for more details.");
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

static esp_err_t app_driver_client_console_handler(int argc, char **argv)
{
    if (argc == 1 && strncmp(argv[0], "help", sizeof("help")) == 0) {
        printf("Client commands:\n"
               "\thelp: Print help\n"
               "\tinvoke: <fabric_index> <remote_node_id> <remote_endpoint_id> <cluster_id> <command_id> parameters ... \n"
               "\t\tExample: matter esp client invoke 0x0001 0xBC5C01 0x0001 0x0003 0x0000 0x78.\n"
               "\tinvoke-group: <fabric_index> <group_id> <cluster_id> <command_id> parameters ... \n"
               "\t\tExample: matter esp client invoke-group 0x0001 0x257 0x0003 0x0000 0x78.\n");
        return ESP_OK;
    }

    const bool is_unicast = argc >= 6 && strncmp(argv[0], "invoke", sizeof("invoke")) == 0;
    const bool is_group = argc >= 5 && strncmp(argv[0], "invoke-group", sizeof("invoke-group")) == 0;
    if (!is_unicast && !is_group) {
        return ESP_ERR_INVALID_ARG;
    }

    const int first_parameter = is_unicast ? 6 : 5;
    const chip::FabricIndex fabric_index = strtoul(argv[1] + 2, nullptr, 16);
    const chip::EndpointId endpoint_id = is_unicast ? strtoul(argv[3] + 2, nullptr, 16) : chip::kInvalidEndpointId;
    const chip::ClusterId cluster_id = strtoul(argv[is_unicast ? 4 : 3] + 2, nullptr, 16);
    const chip::CommandId command_id = strtoul(argv[is_unicast ? 5 : 4] + 2, nullptr, 16);
    char command_data_str[32] = {};
    bool has_fields;
    CHIP_ERROR err = FormatCommandFields(cluster_id, command_id,
                                         argc - first_parameter, argv + first_parameter,
                                         command_data_str, sizeof(command_data_str), has_fields);
    if (err != CHIP_NO_ERROR) {
        return ESP_ERR_INVALID_ARG;
    }
    client::json_encodable fields(command_data_str);
    client::invoke_command::request_t request = {
        .endpoint_id = endpoint_id,
        .cluster_id = cluster_id,
        .command_id = command_id,
        .fields = has_fields ? &fields : nullptr,
    };
    if (is_unicast) {
        err = StartInvoke(chip::ScopedNodeId(strtoull(argv[2] + 2, nullptr, 16), fabric_index), request);
    } else {
        const chip::GroupId group_id = static_cast<chip::GroupId>(strtoul(argv[2] + 2, nullptr, 16));
        err = StartGroupInvoke(group_id, fabric_index, request);
    }
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Invoke submission failed: %s", ErrorStr(err));
        return ESP_FAIL;
    }
    return ESP_OK;
}

static void app_driver_register_commands()
{
    /* Add console command for bound devices */
    static const esp_matter::console::command_t bound_command = {
        .name = "bound",
        .description = "This can be used to simulate on-device control for bound devices."
        "Usage: matter esp bound <bound_command>. "
        "Bound commands: help, invoke",
        .handler = app_driver_bound_console_handler,
    };
    esp_matter::console::add_commands(&bound_command, 1);

    /* Add console command for client to control non-bound devices */
    static const esp_matter::console::command_t client_command = {
        .name = "client",
        .description = "This can be used to simulate on-device control for client devices."
        "Usage: matter esp client <client_command>. "
        "Client commands: help, invoke",
        .handler = app_driver_client_console_handler,
    };
    esp_matter::console::add_commands(&client_command, 1);
}
#endif // CONFIG_ENABLE_CHIP_SHELL

static void app_driver_button_toggle_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "Toggle button pressed");
    LogErrorOnFailure(chip::DeviceLayer::PlatformMgr().ScheduleWork([](intptr_t) {
        client::invoke_command::request_t request = {
            .cluster_id = OnOff::Id,
            .command_id = OnOff::Commands::Toggle::Id,
        };
        CHIP_ERROR err = bound_actions.dispatch(switch_endpoint_id, request, nullptr);
        if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Toggle submission failed: %s", ErrorStr(err));
        }
    }));
}

app_driver_handle_t app_driver_switch_init()
{
    /* Initialize button */

    button_handle_t btns[BSP_BUTTON_NUM];
    ESP_ERROR_CHECK(bsp_iot_button_create(btns, NULL, BSP_BUTTON_NUM));
    ESP_ERROR_CHECK(iot_button_register_cb(btns[0], BUTTON_PRESS_DOWN, NULL, app_driver_button_toggle_cb, NULL));

    /* Other initializations */
#if CONFIG_ENABLE_CHIP_SHELL
    app_driver_register_commands();
#endif // CONFIG_ENABLE_CHIP_SHELL

    return (app_driver_handle_t)btns[0];
}
