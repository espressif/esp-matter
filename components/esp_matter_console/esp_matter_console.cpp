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

#define MAX_CONSOLE_COMMANDS CONFIG_ESP_MATTER_CONSOLE_MAX_COMMANDS

static const char *TAG = "esp_matter_console";
static esp_matter_console_command_t commands[MAX_CONSOLE_COMMANDS];
static int total_added_commands = 0;

esp_err_t esp_matter_console_add_command(esp_matter_console_command_t *command)
{
    if (total_added_commands + 1 > MAX_CONSOLE_COMMANDS) {
        ESP_LOGE(TAG, "Could not add command. Increase the max limit to add more.");
        return ESP_FAIL;
    }
    /* Since the strings in esp_matter_console_command_t are constants, this will work */
    commands[total_added_commands] = *command;
    total_added_commands++;
    return ESP_OK;
}

static void esp_matter_console_print_help()
{
    ESP_LOGI(TAG, "Usage: matter esp <sub_command>");
    ESP_LOGI(TAG, "Sub commands:");
    for (int i = 0; i < total_added_commands; i++) {
        ESP_LOGI(TAG, "\t%s: %s", commands[i].name, commands[i].description);
    }
}

static esp_err_t esp_matter_console_help_handler(int argc, char **argv)
{
    esp_matter_console_print_help();
    return ESP_OK;
}

static esp_err_t esp_matter_console_register_default_commands()
{
    esp_matter_console_command_t command = {
        .name = "help",
        .description = "Print help",
        .handler = esp_matter_console_help_handler,
    };
    return esp_matter_console_add_command(&command);
}

static CHIP_ERROR esp_matter_console_common_handler(int argc, char **argv)
{
    /* This common handler is added to avoid adding `CHIP_ERROR` and its component requirements in other esp-matter
     * components */
    if (argc <= 0) {
        esp_matter_console_print_help();
        return CHIP_NO_ERROR;
    }
    for (int i = 0; i < total_added_commands; i++) {
        if (strncmp(argv[0], commands[i].name, strlen(commands[i].name) + 1) == 0) {
            if (commands[i].handler == NULL) {
                ESP_LOGW(TAG, "No handler set for the command: %s", argv[0]);
                return CHIP_NO_ERROR;
            }
            if (commands[i].handler(argc - 1, &argv[1]) == ESP_OK) { /* Removing the first argument from argv */
                return CHIP_NO_ERROR;
            }
            /* The command handler returned error */
            return CHIP_ERROR_INVALID_ARGUMENT;
        }
    }
    ESP_LOGE(TAG, "Could not find the command: %s. Try the help command for more details: matter esp help", argv[0]);
    return CHIP_ERROR_INVALID_ARGUMENT;
}

static esp_err_t esp_matter_console_register_common_shell_handler()
{
    static chip::Shell::shell_command_t cmds[] = {
        {
            .cmd_func = esp_matter_console_common_handler,
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

esp_err_t esp_matter_console_init()
{
    esp_err_t err = ESP_OK;
    err = esp_matter_console_register_default_commands();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't register default console commands");
    }
    err = esp_matter_console_register_common_shell_handler();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't register common handler");
        return err;
    }

    if (xTaskCreate(&ChipShellTask, "console", CONFIG_ESP_MATTER_CONSOLE_TASK_STACK, NULL, 5, NULL) != pdPASS) {
        ESP_LOGE(TAG, "Couldn't create console task");
        err = ESP_FAIL;
    }
    return err;
}
