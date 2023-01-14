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
#include <string.h>

#define CONSOLE_MAX_COMMAND_SETS CONFIG_ESP_MATTER_CONSOLE_MAX_COMMANDS

namespace esp_matter {
namespace console {

/** Callback for console commands
 *
 * This callback handler will be called when console command is triggered.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
typedef esp_err_t (*command_handler_t)(int argc, char **argv);

/** ESP Matter Console Command */
typedef struct {
    /** Command Name */
    const char *name;
    /** Command Description/Help */
    const char *description;
    /** Command Handler */
    command_handler_t handler;
} command_t;

/** Command iterator callback for the console commands
 *
 * @param command               The console command being iterated.
 * @param arg                   A context variable passed to the iterator function.
 *
 * @return                      ESP_OK to continue iteration; anything else to break iteration.
 */
typedef esp_err_t command_iterator_t(const command_t *command, void *arg);

class engine
{
protected:
    const command_t *_command_set[CONSOLE_MAX_COMMAND_SETS];
    unsigned _command_set_size[CONSOLE_MAX_COMMAND_SETS];
    unsigned _command_set_count;
public:
    engine(): _command_set_count(0) {}

    /** Execution callback for a console command.
     *
     * @param[in] on_command An iterator callback to be called for each command.
     * @param[in] arg        A context variable to be passed to each command iterated.
     */
    void for_each_command(command_iterator_t *on_command, void *arg);

    /** Dispatch and execute the command for the given argument list.
     *
     * @param[in] argc Number of arguments in argv.
     * @param[in] argv Array of arguments in the tokenized command line to execute.
     *
     * @return ESP_OK on success
     * @return error in case of failure.
     */
    esp_err_t exec_command(int argc, char *argv[]);

    /** Registers a command set, or array of commands with the console.
     *
     * @param command_set[in] An array of commands to add to the console.
     * @param count[in]       The number of commands in the command set array.
     *
     * @return ESP_OK on success
     * @return error in case of failure.
     */
    esp_err_t register_commands(const command_t *command_set, unsigned count);
};

/** Print the description of a command
 *
 * @param command[in] The command which's description will be printed.
 * @param arg[in] A context variable passed to the iterator function.
 *
 * @return ESP_OK on success
 * @return error in case of failure.
 */
esp_err_t print_description(const command_t *command, void *arg);

/** Initialize Console
 *
 * This API internally initializes the matter shell.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t init(void);

/** Add Console Command Set
 *
 * Add a new console command.
 * This can be done before calling `esp_matter_console_init()` but the commands will not work until initialized.
 *
 * @param[in] command_set Command struct set array pointer
 * @param[in] count Command struct set array size
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t add_commands(const command_t *command_set, unsigned count);

/** Add Diagnostics Commands
 *
 * Adds the default diagnostics commands.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t diagnostics_register_commands();

/** Add Wi-Fi Commands
 *
 * Adds the default Wi-Fi commands.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t wifi_register_commands();

} // namespace console
} // namespace esp_matter
