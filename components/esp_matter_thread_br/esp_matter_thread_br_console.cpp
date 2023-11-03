// Copyright 2023 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_matter_thread_br_console.h>
#include <esp_matter_thread_br_launcher.h>

namespace esp_matter {
namespace console {

static esp_err_t thread_br_cli_handler(int argc, char **argv)
{
    ot_cli_buffer_t cli_buf;
    memset(&cli_buf, 0, sizeof(cli_buf));
    for (size_t i = 0; i < argc; ++i) {
        cli_buf.buf_len += strlen(argv[i]) + (i == argc - 1 ? 0 : 1);
        if (cli_buf.buf_len > OPENTHREAD_CLI_BUFFER_LENGTH) {
            return ESP_ERR_INVALID_ARG;
        }
        strcat(cli_buf.buf, argv[i]);
        if (i < argc - 1) {
            strcat(cli_buf.buf, " ");
        }
    }
    return cli_transmit_task_post(cli_buf);
}

esp_err_t thread_br_cli_register_command()
{
    static const command_t command = {
        .name = "ot_cli",
        .description = "OpenThread Cli commands. Usage: matter esp ot_cli <otcli_command>.",
        .handler = thread_br_cli_handler,
    };

    return add_commands(&command, 1);
}

} // namespace console
} // namespace esp_matter
