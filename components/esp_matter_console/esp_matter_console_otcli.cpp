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

#include <sdkconfig.h>
#if CONFIG_OPENTHREAD_CLI
#include <esp_check.h>
#include <esp_matter_console.h>
#include <freertos/FreeRTOS.h>
#include <lib/shell/Engine.h>
#include <memory>
#include <platform/ESP32/OpenthreadLauncher.h>

#define CLI_INPUT_BUFF_LENGTH 256u
namespace esp_matter {
namespace console {

static esp_err_t otcli_handler(int argc, char *argv[])
{
    /* the beginning of command "matter esp otcli" has already been removed */
    std::unique_ptr<char[]> cli_str(new char[CLI_INPUT_BUFF_LENGTH]);
    memset(cli_str.get(), 0, CLI_INPUT_BUFF_LENGTH);
    uint8_t len = 0;
    for (size_t i = 0; i < (size_t)argc; ++i) {
        len = len + strlen(argv[i]) + 1;
        if (len > CLI_INPUT_BUFF_LENGTH - 1) {
            return ESP_FAIL;
        }
        strcat(cli_str.get(), argv[i]);
        if (i < (size_t)argc - 1) {
            strcat(cli_str.get(), " ");
        }
    }

    if (cli_transmit_task_post(std::move(cli_str)) != CHIP_NO_ERROR) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t otcli_register_commands()
{
    static const command_t command = {
        .name = "ot_cli",
        .description = "Openthread cli commands. Usage: matter esp ot_cli <command>.",
        .handler = otcli_handler,
    };
    return add_commands(&command, 1);
}
} // namespace console
} // namespace esp_matter
#endif // CONFIG_OPENTHREAD_CLI
