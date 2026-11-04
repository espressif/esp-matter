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
#include <esp_matter_controller_group_settings.h>

namespace esp_matter::controller::console {

static esp_err_t controller_group_settings_handler(int argc, char **argv)
{
    uint16_t id;
    if (argc == 1 && strcmp(argv[0], "list") == 0) {
        return group_settings::show_groups();
    }
    if (argc == 3 && strcmp(argv[0], "add") == 0 && parse_unsigned(argv[1], id)) {
        return group_settings::add_group(argv[2], id);
    }
    if (argc == 2 && strcmp(argv[0], "remove") == 0 && parse_unsigned(argv[1], id)) {
        return group_settings::remove_group(id);
    }
    if (argc == 3 && (strcmp(argv[0], "bind") == 0 || strcmp(argv[0], "unbind") == 0)) {
        uint16_t keyset;
        VerifyOrReturnError(parse_unsigned(argv[1], id) && parse_unsigned(argv[2], keyset), ESP_ERR_INVALID_ARG);
        return strcmp(argv[0], "bind") == 0 ? group_settings::bind_keyset(id, keyset) : group_settings::unbind_keyset(id, keyset);
    }
    if (argc >= 2 && strcmp(argv[0], "keyset") == 0) {
        if (argc == 2 && strcmp(argv[1], "list") == 0) {
            return group_settings::show_keysets();
        }
        if (argc == 3 && strcmp(argv[1], "remove") == 0 && parse_unsigned(argv[2], id)) {
            return group_settings::remove_keyset(id);
        }
        if (argc == 6 && strcmp(argv[1], "add") == 0) {
            uint8_t policy;
            uint64_t validity;
            VerifyOrReturnError(parse_unsigned(argv[2], id) && parse_unsigned(argv[3], policy) &&
                                parse_unsigned(argv[4], validity), ESP_ERR_INVALID_ARG);
            return group_settings::add_keyset(id, policy, validity, argv[5]);
        }
    }
    return ESP_ERR_INVALID_ARG;
}

esp_err_t register_groups_commands()
{
    static const command_t command = {
        .name = "group",
        .description = "Usage: matter esp group list|add <id> <name>|remove <id> OR\n"
        "\tgroup bind|unbind <group> <keyset> OR\n"
        "\tgroup keyset list|remove <id>|add <id> <policy> <validity> <epoch-key-hex>",
        .handler = controller_group_settings_handler,
    };
    return ::esp_matter::console::add_commands(&command, 1);
}

} // namespace esp_matter::controller::console
