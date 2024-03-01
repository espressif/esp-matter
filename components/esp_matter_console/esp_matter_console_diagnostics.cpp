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

#include <esp_heap_caps.h>
#include <esp_log.h>
#include <esp_matter_console.h>
#include <esp_timer.h>
#include <string.h>

namespace esp_matter {
namespace console {

static const char *TAG = "esp_matter_console_diagnostics";
static engine diagnostics_console;

static esp_err_t mem_dump_console_handler(int argc, char *argv[])
{
    printf("\tDescription\tInternal\tSPIRAM\n");
    printf("Current Free Memory\t%d\t\t%d\n",
           heap_caps_get_free_size(MALLOC_CAP_8BIT) - heap_caps_get_free_size(MALLOC_CAP_SPIRAM),
           heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    printf("Largest Free Block\t%d\t\t%d\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL),
           heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM));
    printf("Min. Ever Free Size\t%d\t\t%d\n", heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL),
           heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM));
    return ESP_OK;
}

static esp_err_t up_time_console_handler(int argc, char *argv[])
{
    printf("%s: Uptime of the device: %lld milliseconds\n", TAG, esp_timer_get_time() / 1000);
    return ESP_OK;
}

static esp_err_t diagnostics_dispatch(int argc, char **argv)
{
    if (argc <= 0) {
        diagnostics_console.for_each_command(print_description, NULL);
        return ESP_OK;
    }
    return diagnostics_console.exec_command(argc, argv);
}

esp_err_t diagnostics_register_commands()
{
    static const command_t command = {
        .name = "diagnostics",
        .description = "Diagnostic commands. Usage matter esp diagnostics <diagnostic_command>.",
        .handler = diagnostics_dispatch,
    };

    static const command_t diagnostics_commands[] = {
        {
            .name = "mem-dump",
            .description = "help for memory analysis",
            .handler = mem_dump_console_handler,
        },
        {
            .name = "up-time",
            .description = "print the uptime of the device",
            .handler = up_time_console_handler,
        },
    };
    diagnostics_console.register_commands(diagnostics_commands, sizeof(diagnostics_commands)/sizeof(command_t));

    return add_commands(&command, 1);
}
} // namespace console
} // namespace esp_matter
