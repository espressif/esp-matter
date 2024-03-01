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
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>

#include <esp_matter_console.h>
#include <lib/shell/Engine.h>
#include <platform/ESP32/ESP32Utils.h>

namespace esp_matter {
namespace console {

static const char *TAG = "esp_matter_console";
static engine base_engine;

void engine::for_each_command(command_iterator_t *on_command, void *arg)
{
    for (unsigned i = 0; i < _command_set_count; ++i) {
        for (unsigned j = 0; j < _command_set_size[i]; ++j) {
            if (on_command(&_command_set[i][j], arg) != ESP_OK) {
                return;
            }
        }
    }
}

esp_err_t engine::exec_command(int argc, char *argv[])
{
    esp_err_t err = ESP_ERR_INVALID_ARG;
    if (argc <= 0) {
        return err;
    }
    // find the command from the command set
    for (unsigned i = 0; i < _command_set_count; ++i) {
        for (unsigned j = 0; j < _command_set_size[i]; ++j) {
            if (strcmp(argv[0], _command_set[i][j].name) == 0 && _command_set[i][j].handler) {
                err = _command_set[i][j].handler(argc - 1, &argv[1]);
                break;
            }
        }
    }
    return err;
}

esp_err_t engine::register_commands(const command_t *command_set, unsigned count)
{
    if (_command_set_count >= CONSOLE_MAX_COMMAND_SETS) {
        ESP_LOGE(TAG, "Max number of command sets reached");
        return ESP_FAIL;
    }

    _command_set[_command_set_count] = command_set;
    _command_set_size[_command_set_count] = count;
    ++_command_set_count;
    return ESP_OK;
}

esp_err_t add_commands(const command_t *command_set, unsigned count)
{
    return base_engine.register_commands(command_set, count);
}

esp_err_t print_description(const command_t *command, void *arg)
{
    ESP_LOGI(TAG, "\t%s: %s", command->name, command->description);
    return ESP_OK;
}


static esp_err_t help_handler(int argc, char **argv)
{
    base_engine.for_each_command(print_description, NULL);
    return ESP_OK;
}

static esp_err_t register_default_commands()
{
    static const command_t command= {
        .name = "help",
        .description = "Print help",
        .handler = help_handler,
    };
    return add_commands(&command, 1);
}

static CHIP_ERROR common_handler(int argc, char **argv)
{
    /* This common handler is added to avoid adding `CHIP_ERROR` and its component requirements in other esp-matter
     * components */
    if (argc <= 0) {
        help_handler(argc, argv);
        return CHIP_NO_ERROR;
    }
    return chip::DeviceLayer::Internal::ESP32Utils::MapError(base_engine.exec_command(argc, argv));
}

static esp_err_t register_common_shell_handler()
{
    static const chip::Shell::shell_command_t cmds[] = {
        {
            .cmd_func = common_handler,
            .cmd_name = "esp",
            .cmd_help = "Usage: matter esp <sub_command>",
        },
    };
    int cmds_num = sizeof(cmds) / sizeof(chip::Shell::shell_command_t);
    chip::Shell::Engine::Root().RegisterCommands(cmds, cmds_num);
    return ESP_OK;
}

static void ChipShellTask(void *args)
{
    chip::Shell::Engine::Root().RunMainLoop();
}

esp_err_t init()
{
    esp_err_t err = ESP_OK;
    err = register_default_commands();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't register default console commands");
    }
    err = register_common_shell_handler();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't register common handler");
        return err;
    }
    chip::Shell::Engine::Root().Init();
    if (xTaskCreate(&ChipShellTask, "console", CONFIG_ESP_MATTER_CONSOLE_TASK_STACK, NULL, 5, NULL) != pdPASS) {
        ESP_LOGE(TAG, "Couldn't create console task");
        err = ESP_FAIL;
    }
    return err;
}

} // namespace console
} // namespace esp_matter
