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

namespace esp_matter::controller::console {

static ::esp_matter::console::engine controller_console;

static esp_err_t controller_help_handler(int argc, char **argv)
{
    ESP_LOGI(TAG, "help: print this page");
    controller_console.for_each_command(::esp_matter::console::print_description, nullptr);
    return ESP_OK;
}

static esp_err_t controller_dispatch(int argc, char **argv)
{
    if (argc == 0 || strcmp(argv[0], "help") == 0) {
        return controller_help_handler(argc, argv);
    }
    return controller_console.exec_command(argc, argv);
}

esp_err_t register_commands(const command_t *commands, unsigned count)
{
    static bool root_registered = false;
    if (!root_registered) {
        static const command_t command = {
            .name = "controller",
            .description = "Controller commands. Usage: matter esp controller <command_name>",
            .handler = controller_dispatch,
        };
        esp_err_t error = ::esp_matter::console::add_commands(&command, 1);
        if (error != ESP_OK) {
            return error;
        }
        root_registered = true;
    }
    return controller_console.register_commands(commands, count);
}

} // namespace esp_matter::controller::console
