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
#include <esp_matter_console.h>
#include <platform/CHIPDeviceLayer.h>
#if CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY_CLIENT
#include <app/server/Dnssd.h>
#include <app/server/Server.h>
#include <string.h>
#endif

namespace esp_matter {
namespace console {
#if CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY_CLIENT
static const char *TAG = "esp_matter_console_udc";
static engine udc_console;

static esp_err_t send_udc_request(int argc, char *argv[])
{
    ESP_RETURN_ON_FALSE(argc == 2, ESP_ERR_INVALID_ARG, TAG, "Incorrect arguments");
    chip::Inet::IPAddress commissioner;
    chip::Inet::IPAddress::FromString(argv[0], commissioner);
    uint16_t port = (uint16_t)strtol(argv[1], nullptr, 10);
    chip::Protocols::UserDirectedCommissioning::IdentificationDeclaration id;
    chip::Server::GetInstance().SendUserDirectedCommissioningRequest(
        chip::Transport::PeerAddress::UDP(commissioner, port), id);
    return ESP_OK;
}

static esp_err_t send_udc_cancel(int argc, char *argv[])
{
    ESP_RETURN_ON_FALSE(argc == 2, ESP_ERR_INVALID_ARG, TAG, "Incorrect arguments");
    chip::Inet::IPAddress commissioner;
    chip::Inet::IPAddress::FromString(argv[0], commissioner);
    uint16_t port = (uint16_t)strtol(argv[1], nullptr, 10);
    chip::Protocols::UserDirectedCommissioning::IdentificationDeclaration id;
    id.SetCancelPasscode(true);
    chip::Server::GetInstance().SendUserDirectedCommissioningRequest(
        chip::Transport::PeerAddress::UDP(commissioner, port), id);
    return ESP_OK;
}

static esp_err_t discover_commissioner(int argc, char *argv[])
{
    ESP_LOGI(TAG, "Use 'matter dns browse commissioner' command to discovery commissioner");
    return ESP_OK;
}

static esp_err_t udc_dispatch(int argc, char *argv[])
{
    if (argc <= 0) {
        udc_console.for_each_command(print_description, NULL);
        return ESP_OK;
    }
    return udc_console.exec_command(argc, argv);
}
#endif

esp_err_t udc_register_commands()
{
#if CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY_CLIENT
    static const command_t command = {
        .name = "udc",
        .description = "udc commands. Usage: matter esp udc <udc_command>.",
        .handler = udc_dispatch,
    };

    static const command_t udc_commands[] = {
        {
            .name = "discover",
            .description = "Discover commissioner",
            .handler = discover_commissioner,
        },
        {
            .name = "send",
            .description = "Send UDC message to address. Usage: matter esp udc send [address] [port].",
            .handler = send_udc_request,
        },
        {
            .name = "cancel",
            .description = "Send UDC cancel message to address. Usage: matter esp udc cancel [address] [port].",
            .handler = send_udc_cancel,
        },
    };
    udc_console.register_commands(udc_commands, sizeof(udc_commands) / sizeof(command_t));
    return add_commands(&command, 1);
#else
    return ESP_OK;
#endif
}
} // namespace console
} // namespace esp_matter
