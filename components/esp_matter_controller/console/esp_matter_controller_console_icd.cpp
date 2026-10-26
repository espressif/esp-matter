// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
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

#include "console_utils.h"
#include <esp_matter_controller_icd_client.h>

namespace esp_matter::controller::console {

static esp_err_t controller_icd_list_handler(int argc, char **argv)
{
    if (argc != 1 || strncmp(argv[0], "list", sizeof("list")) != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    controller::list_registered_icd();
    return ESP_OK;
}

esp_err_t register_icd_commands()
{
    static const command_t commands[] = {
        {
            .name = "icd",
            .description = "icd client management.\n"
            "\tUsage: controller icd list",
            .handler = controller_icd_list_handler,
        },
    };
    return register_commands(commands, sizeof(commands) / sizeof(commands[0]));
}

} // namespace esp_matter::controller::console
