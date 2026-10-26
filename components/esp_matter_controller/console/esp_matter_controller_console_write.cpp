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

#include <esp_matter_client.h>
#include <esp_matter_client_target.h>
#include <esp_matter_client_write_command.h>

namespace esp_matter::controller::console {

class ConsoleWriteDelegate final : public client::write_command::delegate {
public:
    client::write_command::interaction operation;
    bool group = false;
    void OnResponse(const esp_matter::client::target_t &, const chip::app::ConcreteDataAttributePath &, const chip::app::StatusIB &status) override
    {
        CHIP_ERROR err = status.ToChipError();
        if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Write status: 0x%" PRIx32, err.AsInteger());
        }
    }

    void OnDone(const esp_matter::client::target_t &, CHIP_ERROR err) override
    {
        if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Write failed: 0x%" PRIx32, err.AsInteger());
        } else if (group) {
            ESP_LOGI(TAG, "Group write dispatched; member acknowledgments are not available");
        }
        chip::Platform::Delete(this);
    }
};

static esp_err_t controller_write_attr_handler(int argc, char **argv)
{
    if ((argc != 7 && argc != 9) || strcmp(argv[0], "attr") != 0 || strcmp(argv[argc - 2], "--") != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    uint64_t node_id;
    client::write_command::attribute_write_t attribute;
    ESP_RETURN_ON_FALSE(parse_command_target(argv[1], argv[2], node_id, attribute.path.mEndpointId) &&
                        parse_unsigned(argv[3], attribute.path.mClusterId) && parse_unsigned(argv[4], attribute.path.mAttributeId),
                        ESP_ERR_INVALID_ARG, TAG, "Invalid attribute path");
    uint16_t timeout;
    ESP_RETURN_ON_ERROR(parse_timed_option(argc - 7, argv + 5, timeout), TAG, "Invalid timed write");
    client::json_encodable value(argv[argc - 1]);
    attribute.value = &value;
    client::write_command::request_t request = {
        .attributes = chip::Span<const client::write_command::attribute_write_t>(&attribute, 1),
    };
    if (timeout > 0) {
        request.options.timed_timeout_ms.SetValue(timeout);
    }

    esp_matter::lock::ScopedChipStackLock lock(portMAX_DELAY);
    auto *command = chip::Platform::New<ConsoleWriteDelegate>();
    ESP_RETURN_ON_FALSE(command != nullptr, ESP_ERR_NO_MEM, TAG, "Failed to allocate Write");
    command->group = chip::IsGroupId(node_id);
    CHIP_ERROR err;
    if (command->group) {
        err = command->operation.start(chip::GroupIdFromNodeId(node_id), active_fabric_index(), request, command);
    } else {
        chip::ScopedNodeId target(node_id, active_fabric_index());
        err = command->operation.start(target, request, command);
    }
    if (err != CHIP_NO_ERROR) {
        chip::Platform::Delete(command);
    }
    return submission_result("Write", err);
}

esp_err_t register_write_commands()
{
    static const command_t commands[] = {
        {
            .name = "write",
            .description = "Usage: controller write attr <node> <endpoint> <cluster> <id> [--timed-ms N] -- <json>\n"
            "\tcontroller write attr group <group> <cluster> <id> -- <json>\n"
            "\tEncoded group node IDs are supported; their endpoint is ignored. Group writes cannot be timed.",
            .handler = controller_write_attr_handler,
        },
    };
    return register_commands(commands, sizeof(commands) / sizeof(commands[0]));
}

} // namespace esp_matter::controller::console
