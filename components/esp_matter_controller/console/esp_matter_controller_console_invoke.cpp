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
#include <esp_matter_client_invoke_command.h>
#include <esp_matter_client.h>
#include <stdio.h>
#include <tlv_to_json.h>

namespace esp_matter::controller::console {

class ConsoleInvokeDelegate final
    : public client::invoke_command::delegate {
public:
    client::invoke_command::interaction operation;
    bool group = false;
    void OnResponse(const esp_matter::client::target_t &, const chip::app::ConcreteCommandPath &, const chip::app::StatusIB &status, chip::TLV::TLVReader *response) override
    {
        CHIP_ERROR err = status.ToChipError();
        if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Invoke status: 0x%" PRIx32, err.AsInteger());
            return;
        }
        if (response == nullptr) {
            ESP_LOGI(TAG, "Invoke completed without response data");
            return;
        }
        cJSON *json = nullptr;
        if (tlv_to_json(*response, &json, tlv_to_json_options{.human_readable_bytes = true}) == ESP_OK) {
            char *text = cJSON_Print(json);
            if (text != nullptr) {
                ESP_LOGI(TAG, "Response JSON:\n%s", text);
                cJSON_free(text);
            }
        }
        cJSON_Delete(json);
    }

    void OnDone(const esp_matter::client::target_t &, CHIP_ERROR err) override
    {
        if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Invoke failed: 0x%" PRIx32, err.AsInteger());
        } else if (group) {
            ESP_LOGI(TAG, "Group invoke dispatched; member acknowledgments are not available");
        }
        chip::Platform::Delete(this);
    }
};

static esp_err_t controller_invoke_command_handler(int argc, char **argv)
{
    if ((argc != 6 && argc != 8) || strcmp(argv[argc - 2], "--") != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    uint64_t node_id;
    uint16_t endpoint_id;
    uint32_t cluster_id, command_id;
    ESP_RETURN_ON_FALSE(parse_command_target(argv[0], argv[1], node_id, endpoint_id) &&
                        parse_unsigned(argv[2], cluster_id) && parse_unsigned(argv[3], command_id),
                        ESP_ERR_INVALID_ARG, TAG, "Invalid command path");
    uint16_t timeout;
    ESP_RETURN_ON_ERROR(parse_timed_option(argc - 6, argv + 4, timeout), TAG, "Invalid timed invoke");
    ScopedMemoryBufferWithSize<char> json;
    ESP_RETURN_ON_FALSE(json.Alloc(strlen(argv[argc - 1]) + sizeof("{\"0:OBJ\":}")),
                        ESP_ERR_NO_MEM, TAG, "Failed to allocate Invoke fields");
    snprintf(json.Get(), json.AllocatedSize(), "{\"0:OBJ\":%s}", argv[argc - 1]);
    client::json_encodable fields(json.Get());
    client::invoke_command::request_t request = {
        .endpoint_id = endpoint_id,
        .cluster_id = cluster_id,
        .command_id = command_id,
        .fields = &fields,
    };
    if (timeout > 0) {
        request.options.timed_timeout_ms.SetValue(timeout);
    }

    esp_matter::lock::ScopedChipStackLock lock(portMAX_DELAY);
    auto *command = chip::Platform::New<ConsoleInvokeDelegate>();
    ESP_RETURN_ON_FALSE(command != nullptr, ESP_ERR_NO_MEM, TAG, "Failed to allocate Invoke");
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
    return submission_result("Invoke", err);
}

esp_err_t register_invoke_commands()
{
    static const command_t commands[] = {
        {
            .name = "invoke",
            .description = "Usage: controller invoke <node> <endpoint> <cluster> <command> [--timed-ms N] -- <json>\n"
            "\tcontroller invoke group <group> <cluster> <command> -- <json>\n"
            "\tFields are a tagged JSON object, e.g. {} or {\"0:U16\":42}.\n"
            "\tEncoded group node IDs are supported; their endpoint is ignored. Group invokes cannot be timed.",
            .handler = controller_invoke_command_handler,
        },
    };
    return register_commands(commands, sizeof(commands) / sizeof(commands[0]));
}

} // namespace esp_matter::controller::console
