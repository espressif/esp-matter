/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_matter_client.h"
#include <cstddef>
#include <cstdio>
#include <esp_log.h>
#include <stdlib.h>
#include <string.h>

#include <esp_matter.h>
#include <esp_matter_console.h>
#include "bsp/esp-bsp.h"

#include <app_priv.h>
#include <app_reset.h>

#include <app/server/Server.h>
#include <lib/core/Optional.h>

using chip::kInvalidClusterId;
static constexpr chip::CommandId kInvalidCommandId = 0xFFFF'FFFF;

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "app_driver";
extern uint16_t switch_endpoint_id;

#if CONFIG_ENABLE_CHIP_SHELL
static char console_buffer[101] = {0};
static esp_err_t app_driver_bound_console_handler(int argc, char **argv)
{
    if (argc == 1 && strncmp(argv[0], "help", sizeof("help")) == 0) {
        printf("Bound commands:\n"
               "\thelp: Print help\n"
               "\tinvoke: <local_endpoint_id> <cluster_id> <command_id> parameters ... \n"
               "\t\tExample: matter esp bound invoke 0x0001 0x0003 0x0000 0x78.\n");
    } else if (argc >= 4 && strncmp(argv[0], "invoke", sizeof("invoke")) == 0) {
        client::request_handle_t req_handle;
        req_handle.type = esp_matter::client::INVOKE_CMD;
        uint16_t local_endpoint_id = strtoul((const char *)&argv[1][2], NULL, 16);
        req_handle.command_path.mClusterId = strtoul((const char *)&argv[2][2], NULL, 16);
        req_handle.command_path.mCommandId = strtoul((const char *)&argv[3][2], NULL, 16);

        if (argc > 4) {
            console_buffer[0] = argc - 4;
            for (int i = 0; i < (argc - 4); i++) {
                if ((argv[4 + i][0] != '0') || (argv[4 + i][1] != 'x') ||
                    (strlen((const char *)&argv[4 + i][2]) > 10)) {
                    ESP_LOGE(TAG, "Incorrect arguments. Check help for more details.");
                    return ESP_ERR_INVALID_ARG;
                }
                strcpy((console_buffer + 1 + 10 * i), &argv[4 + i][2]);
            }

            req_handle.request_data = console_buffer;
        }

        client::cluster_update(local_endpoint_id, &req_handle);
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
               "\tinvoke: <fabric_index> <remote_node_id> <remote_endpoint_id> <cluster_id> <command_id> parameters "
               "... \n"
               "\t\tExample: matter esp client invoke 0x0001 0xBC5C01 0x0001 0x0003 0x0000 0x78.\n"
               "\tinvoke-group: <fabric_index> <group_id> <cluster_id> <command_id> parameters ... \n"
               "\t\tExample: matter esp client invoke-group 0x0001 0x257 0x0003 0x0000 0x78.\n");
    } else if (argc >= 6 && strncmp(argv[0], "invoke", sizeof("invoke")) == 0) {
        client::request_handle_t req_handle;
        req_handle.type = esp_matter::client::INVOKE_CMD;
        uint8_t fabric_index = strtoul((const char *)&argv[1][2], NULL, 16);
        uint64_t node_id = strtoull((const char *)&argv[2][2], NULL, 16);
        req_handle.command_path = {(chip::EndpointId)strtoul((const char *)&argv[3][2], NULL, 16) /* EndpointId */,
                                   0 /* GroupId */, strtoul((const char *)&argv[4][2], NULL, 16) /* ClusterId */,
                                   strtoul((const char *)&argv[5][2], NULL, 16) /* CommandId */,
                                   chip::app::CommandPathFlags::kEndpointIdValid};

        if (argc > 6) {
            console_buffer[0] = argc - 6;
            for (int i = 0; i < (argc - 6); i++) {
                if ((argv[6 + i][0] != '0') || (argv[6 + i][1] != 'x') ||
                    (strlen((const char *)&argv[6 + i][2]) > 10)) {
                    ESP_LOGE(TAG, "Incorrect arguments. Check help for more details.");
                    return ESP_ERR_INVALID_ARG;
                }
                strcpy((console_buffer + 1 + 10 * i), &argv[6 + i][2]);
            }

            req_handle.request_data = console_buffer;
        }
        auto &server = chip::Server::GetInstance();
        client::connect(server.GetCASESessionManager(), fabric_index, node_id, &req_handle);
    } else if (argc >= 5 && strncmp(argv[0], "invoke-group", sizeof("invoke-group")) == 0) {
        client::request_handle_t req_handle;
        req_handle.type = esp_matter::client::INVOKE_CMD;
        uint8_t fabric_index = strtoul((const char *)&argv[1][2], NULL, 16);
        req_handle.command_path.mGroupId = strtoul((const char *)&argv[2][2], NULL, 16);
        req_handle.command_path.mClusterId = strtoul((const char *)&argv[3][2], NULL, 16);
        req_handle.command_path.mCommandId = strtoul((const char *)&argv[4][2], NULL, 16);
        req_handle.command_path = {
            0 /* EndpointId */, (chip::GroupId)strtoul((const char *)&argv[2][2], NULL, 16) /* GroupId */,
            strtoul((const char *)&argv[3][2], NULL, 16) /* ClusterId */,
            strtoul((const char *)&argv[4][2], NULL, 16) /* CommandId */, chip::app::CommandPathFlags::kGroupIdValid};

        if (argc > 5) {
            console_buffer[0] = argc - 5;
            for (int i = 0; i < (argc - 5); i++) {
                if ((argv[5 + i][0] != '0') || (argv[5 + i][1] != 'x') ||
                    (strlen((const char *)&argv[5 + i][2]) > 10)) {
                    ESP_LOGE(TAG, "Incorrect arguments. Check help for more details.");
                    return ESP_ERR_INVALID_ARG;
                }
                strcpy((console_buffer + 1 + 10 * i), &argv[5 + i][2]);
            }

            req_handle.request_data = console_buffer;
        }

        client::group_request_send(fabric_index, &req_handle);
    } else {
        ESP_LOGE(TAG, "Incorrect arguments. Check help for more details.");
        return ESP_ERR_INVALID_ARG;
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

static void send_command_success_callback(void *context, const ConcreteCommandPath &command_path,
                                          const chip::app::StatusIB &status, TLVReader *response_data)
{
    ESP_LOGI(TAG, "Send command success");
}

static void send_command_failure_callback(void *context, CHIP_ERROR error)
{
    ESP_LOGI(TAG, "Send command failure: err :%" CHIP_ERROR_FORMAT, error.Format());
}

void app_driver_client_invoke_command_callback(client::peer_device_t *peer_device, client::request_handle_t *req_handle,
                                               void *priv_data)
{
    if (req_handle->type != esp_matter::client::INVOKE_CMD) {
        return;
    }
    char command_data_str[32];
    // on_off light switch should support on_off cluster and identify cluster commands sending.
    if (req_handle->command_path.mClusterId == OnOff::Id) {
        strcpy(command_data_str, "{}");
    } else if (req_handle->command_path.mClusterId == Identify::Id) {
        if (req_handle->command_path.mCommandId == Identify::Commands::Identify::Id) {
            if (((char *)req_handle->request_data)[0] != 1) {
                ESP_LOGE(TAG, "Number of parameters error");
                return;
            }
            sprintf(command_data_str, "{\"0:U16\": %ld}",
                    strtoul((const char *)(req_handle->request_data) + 1, NULL, 16));
        } else {
            ESP_LOGE(TAG, "Unsupported command");
            return;
        }
    } else {
        ESP_LOGE(TAG, "Unsupported cluster");
        return;
    }
    client::interaction::invoke::send_request(NULL, peer_device, req_handle->command_path, command_data_str,
                                              send_command_success_callback, send_command_failure_callback,
                                              chip::NullOptional);
}

void app_driver_client_group_invoke_command_callback(uint8_t fabric_index, client::request_handle_t *req_handle,
                                                     void *priv_data)
{
    if (req_handle->type != esp_matter::client::INVOKE_CMD) {
        return;
    }
    char command_data_str[32];
    // on_off light switch should support on_off cluster and identify cluster commands sending.
    if (req_handle->command_path.mClusterId == OnOff::Id) {
        strcpy(command_data_str, "{}");
    } else if (req_handle->command_path.mClusterId == Identify::Id) {
        if (req_handle->command_path.mCommandId == Identify::Commands::Identify::Id) {
            if (((char *)req_handle->request_data)[0] != 1) {
                ESP_LOGE(TAG, "Number of parameters error");
                return;
            }
            sprintf(command_data_str, "{\"0:U16\": %ld}",
                    strtoul((const char *)(req_handle->request_data) + 1, NULL, 16));
        } else {
            ESP_LOGE(TAG, "Unsupported command");
            return;
        }
    } else {
        ESP_LOGE(TAG, "Unsupported cluster");
        return;
    }
    client::interaction::invoke::send_group_request(fabric_index, req_handle->command_path, command_data_str);
}

static void app_driver_button_toggle_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "Toggle button pressed");
    client::request_handle_t req_handle;
    req_handle.type = esp_matter::client::INVOKE_CMD;
    req_handle.command_path.mClusterId = OnOff::Id;
    req_handle.command_path.mCommandId = OnOff::Commands::Toggle::Id;

    lock::chip_stack_lock(portMAX_DELAY);
    client::cluster_update(switch_endpoint_id, &req_handle);
    lock::chip_stack_unlock();
}

app_driver_handle_t app_driver_switch_init()
{
    /* Initialize button */

    button_handle_t btns[BSP_BUTTON_NUM];
    ESP_ERROR_CHECK(bsp_iot_button_create(btns, NULL, BSP_BUTTON_NUM));
    ESP_ERROR_CHECK(iot_button_register_cb(btns[0], BUTTON_PRESS_DOWN, app_driver_button_toggle_cb, NULL));

    /* Other initializations */
#if CONFIG_ENABLE_CHIP_SHELL
    app_driver_register_commands();
#endif // CONFIG_ENABLE_CHIP_SHELL
    client::set_request_callback(app_driver_client_invoke_command_callback,
                                 app_driver_client_group_invoke_command_callback, NULL);

    return (app_driver_handle_t)btns[0];
}
