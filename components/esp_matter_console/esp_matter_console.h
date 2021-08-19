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

#pragma once

#include <esp_err.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** Callback for console commands
 *
 * This callback handler will be called when console command is triggered.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
typedef esp_err_t (*esp_matter_console_handler_t)(int argc, char** argv);

/** ESP Matter Console Command */
typedef struct {
    /** Command Name */
    const char *name;
    /** Command Description/Help */
    const char *description;
    /** Command Handler */
    esp_matter_console_handler_t handler;
} esp_matter_console_command_t;

/** Initialize Console
 *
 * This API internally initializes the matter shell.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t esp_matter_console_init(void);

/** Add Console Command
 *
 * Add a new console command.
 * This can be done before calling `esp_matter_console_init()` but the commands will not work until initialized.
 *
 * @param[in] command Pointer to command struct
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t esp_matter_console_add_command(esp_matter_console_command_t *command);

#ifdef __cplusplus
}
#endif
