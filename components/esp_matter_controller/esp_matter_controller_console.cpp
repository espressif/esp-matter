/*
 *
 *    Copyright (c) 2022 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include <esp_matter_controller_cluster_command.h>
#include <esp_matter_controller_console.h>
#include <esp_matter_controller_pairing_command.h>
#include <esp_matter_controller_read_command.h>
#include <esp_matter_controller_subscribe_command.h>
#include <esp_matter_controller_utils.h>
#include <esp_matter_controller_write_command.h>
#include <lib/core/CHIPCore.h>
#include <lib/shell/Commands.h>
#include <lib/shell/Engine.h>
#include <lib/shell/commands/Help.h>
#include <lib/shell/streamer.h>
#include <lib/support/CHIPArgParser.hpp>
#include <lib/support/CHIPMem.h>
#include <lib/support/CodeUtils.h>
#include <platform/CHIPDeviceLayer.h>
#include <protocols/secure_channel/RendezvousParameters.h>

using chip::NodeId;
using chip::Inet::IPAddress;
using chip::Transport::PeerAddress;
using esp_matter::controller::command_data_t;

const static char *TAG = "esp_matter_controller";
namespace esp_matter {

namespace console {
static engine controller_console;

static esp_err_t controller_help_handler(int argc, char **argv)
{
    controller_console.for_each_command(print_description, nullptr);
    return ESP_OK;
}

static esp_err_t controller_pairing_handler(int argc, char **argv)
{
    if (argc < 3 || argc > 6) {
        return ESP_ERR_INVALID_ARG;
    }

    if (strncmp(argv[0], "onnetwork", sizeof("onnetwork")) == 0) {
        if (argc != 3) {
            return ESP_ERR_INVALID_ARG;
        }

        uint64_t nodeId = string_to_uint64(argv[1]);
        uint32_t pincode = string_to_uint32(argv[2]);
        return controller::pairing_on_network(nodeId, pincode);
    } else if (strncmp(argv[0], "ble-wifi", sizeof("ble-wifi")) == 0) {
        return ESP_ERR_NOT_SUPPORTED;
    } else if (strncmp(argv[0], "ble-thread", sizeof("ble-thread")) == 0) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    return ESP_ERR_INVALID_ARG;
}

static esp_err_t controller_invoke_command_handler(int argc, char **argv)
{
    if (argc < 4) {
        return ESP_ERR_INVALID_ARG;
    }

    uint64_t node_id = string_to_uint64(argv[0]);
    uint16_t endpoint_id = string_to_uint16(argv[1]);

    return controller::send_invoke_cluster_command(node_id, endpoint_id, argc - 2, argv + 2);
}

static esp_err_t controller_read_attr_handler(int argc, char **argv)
{
    if (argc != 4) {
        return ESP_ERR_INVALID_ARG;
    }

    uint64_t node_id = string_to_uint64(argv[0]);
    uint16_t endpoint_id = string_to_uint16(argv[1]);
    uint32_t cluster_id = string_to_uint32(argv[2]);
    uint32_t attribute_id = string_to_uint32(argv[3]);

    return controller::send_read_attr_command(node_id, endpoint_id, cluster_id, attribute_id);
}

static esp_err_t controller_write_attr_handler(int argc, char **argv)
{
    if (argc != 5) {
        return ESP_ERR_INVALID_ARG;
    }

    uint64_t node_id = string_to_uint64(argv[0]);
    uint16_t endpoint_id = string_to_uint16(argv[1]);
    uint32_t cluster_id = string_to_uint32(argv[2]);
    uint32_t attribute_id = string_to_uint32(argv[3]);
    char *attribute_val_str = argv[4];

    return controller::send_write_attr_command(node_id, endpoint_id, cluster_id, attribute_id, attribute_val_str);
}

static esp_err_t controller_read_event_handler(int argc, char **argv)
{
    if (argc != 4) {
        return ESP_ERR_INVALID_ARG;
    }

    uint64_t node_id = string_to_uint64(argv[0]);
    uint16_t endpoint_id = string_to_uint16(argv[1]);
    uint32_t cluster_id = string_to_uint32(argv[2]);
    uint32_t event_id = string_to_uint32(argv[3]);

    return controller::send_read_event_command(node_id, endpoint_id, cluster_id, event_id);
}

static esp_err_t controller_subscribe_attr_handler(int argc, char **argv)
{
    if (argc != 6) {
        return ESP_ERR_INVALID_ARG;
    }

    uint64_t node_id = string_to_uint64(argv[0]);
    uint16_t endpoint_id = string_to_uint16(argv[1]);
    uint32_t cluster_id = string_to_uint32(argv[2]);
    uint32_t attribute_id = string_to_uint32(argv[3]);
    uint16_t min_interval = string_to_uint16(argv[4]);
    uint16_t max_interval = string_to_uint16(argv[5]);
    return controller::send_subscribe_attr_command(node_id, endpoint_id, cluster_id, attribute_id, min_interval,
                                                   max_interval);
}

static esp_err_t controller_subscribe_event_handler(int argc, char **argv)
{
    if (argc != 6) {
        return ESP_ERR_INVALID_ARG;
    }

    uint64_t node_id = string_to_uint64(argv[0]);
    uint16_t endpoint_id = string_to_uint16(argv[1]);
    uint32_t cluster_id = string_to_uint32(argv[2]);
    uint32_t event_id = string_to_uint32(argv[3]);
    uint16_t min_interval = string_to_uint16(argv[4]);
    uint16_t max_interval = string_to_uint16(argv[5]);
    return controller::send_subscribe_event_command(node_id, endpoint_id, cluster_id, event_id, min_interval,
                                                    max_interval);
}

static esp_err_t controller_shutdown_subscription_handler(int argc, char **argv)
{
    if (argc != 2) {
        return ESP_ERR_INVALID_ARG;
    }
    uint64_t node_id = string_to_uint64(argv[0]);
    uint32_t subscription_id = string_to_uint32(argv[1]);
    return controller::send_shutdown_subscription(node_id, subscription_id);
}

static esp_err_t controller_dispatch(int argc, char **argv)
{
    if (argc == 0) {
        return controller_help_handler(argc, argv);
    }
    return controller_console.exec_command(argc, argv);
}

esp_err_t controller_register_commands()
{
    // Subcommands for root command: `controller <subcommand>`
    const static command_t controller_sub_commands[] = {
        {
            .name = "help",
            .description = "print this page",
            .handler = controller_help_handler,
        },
#if CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
        {
            .name = "pairing",
            .description = "Pairing a node. "
                           "Usage: controller pairing onnetwork [nodeid] [pincode] Or "
                           "controller pairing ble-wifi [nodeid] [pincode] [discriminator] [ssid] [password] Or "
                           "controller pairing ble-thread [nodeid] [pincode] [discriminator] [dataset]",
            .handler = controller_pairing_handler,
        },
#endif
        {
            .name = "invoke-cmd",
            .description = "Send command to the nodes. "
                           "Usage: controller invoke-cmd [node-id] [endpoint-id] [cluster-id] [command-id] [payload]",
            .handler = controller_invoke_command_handler,
        },
        {
            .name = "read-attr",
            .description = "Read attributes of the nodes. "
                           "Usage: controller read-attr [node-id] [endpoint-id] [cluster-id] [attr-id]",
            .handler = controller_read_attr_handler,
        },
        {
            .name = "write-attr",
            .description = "Write attributes of the nodes. "
                           "Usage: controller write-attr [node-id] [endpoint-id] [cluster-id] [attr-id] [attr-value]",
            .handler = controller_write_attr_handler,
        },
        {
            .name = "read-event",
            .description = "Read events of the nodes. "
                           "Usage: controller read-event [node-id] [endpoint-id] [cluster-id] [event-id]",
            .handler = controller_read_event_handler,
        },
        {
            .name = "subs-attr",
            .description = "Subscribe attributes of the nodes. "
                           "Usage: controller subs-attr [node-id] [endpoint-id] [cluster-id] [attr-id] [min-interval] "
                           "[max-interval]",
            .handler = controller_subscribe_attr_handler,
        },
        {
            .name = "subs-event",
            .description = "Subscribe events of the nodes. "
                           "Usage: controller subs-attr [node-id] [endpoint-id] [cluster-id] [event-id] [min-interval] "
                           "[max-interval]",
            .handler = controller_subscribe_event_handler,
        },
        {
            .name = "shutdown-subs",
            .description = "Shutdown subscription."
                           "Usage: controller shutdown-subs [node-id] [subscription-id]",
            .handler = controller_shutdown_subscription_handler,
        },
    };

    const static command_t controller_command = {
        .name = "controller",
        .description = "Controller commands. Usage: matter controller [command_name]",
        .handler = controller_dispatch,
    };
    // Register the controller commands
    controller_console.register_commands(controller_sub_commands, sizeof(controller_sub_commands) / sizeof(command_t));
    return add_commands(&controller_command, 1);
}

} // namespace console
} // namespace esp_matter
