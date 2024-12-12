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
#include <esp_matter_core.h>

namespace esp_matter {
namespace console {

static engine factoryreset;

static esp_err_t factoryreset_dispatch(int argc, char *argv[])
{
    return factory_reset();
}

esp_err_t factoryreset_register_commands()
{
    static const command_t command = {
        .name = "factoryreset",
        .description = "factoryreset the device. Usage: matter esp factoryreset.",
        .handler = factoryreset_dispatch,
    };
    return add_commands(&command, 1);
}
} // namespace console
} // namespace esp_matter
