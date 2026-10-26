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

#include <commands/clusters/DataModelLogger.h>
#include <esp_matter_client_read_command.h>

namespace esp_matter::controller::console {

using chip::Platform::ScopedMemoryBufferWithSize;

class ConsoleReadDelegate final : public client::read_command::delegate {
public:
    client::read_command::interaction operation;
    void OnAttribute(const esp_matter::client::target_t &, const chip::app::ConcreteDataAttributePath &path, chip::TLV::TLVReader *value, const chip::app::StatusIB &status) override
    {
        CHIP_ERROR err = status.ToChipError();
        if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Read status: 0x%" PRIx32, err.AsInteger());
        } else {
            LogErrorOnFailure(DataModelLogger::LogAttribute(path, value));
        }
    }

    void OnEvent(const esp_matter::client::target_t &, const chip::app::EventHeader &header, chip::TLV::TLVReader *value, const chip::app::StatusIB *status) override
    {
        if (status != nullptr) {
            ESP_LOGE(TAG, "Read event status: 0x%" PRIx32,
                     status->ToChipError().AsInteger());
        } else {
            LogErrorOnFailure(DataModelLogger::LogEvent(header, value));
        }
    }

    void OnDone(const esp_matter::client::target_t &, CHIP_ERROR err) override
    {
        if (err != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Read failed: 0x%" PRIx32, err.AsInteger());
        }
        chip::Platform::Delete(this);
    }
};

template <typename Path> static esp_err_t controller_read_handler(int argc, char **argv)
{
    if (argc != 4) {
        return ESP_ERR_INVALID_ARG;
    }

    uint64_t node_id;
    ESP_RETURN_ON_FALSE(parse_unsigned(argv[0], node_id), ESP_ERR_INVALID_ARG, TAG, "Invalid node ID");
    ScopedMemoryBufferWithSize<Path> paths;
    ESP_RETURN_ON_ERROR(parse_path_columns(argv[1], argv[2], argv[3], paths), TAG, "Failed to parse paths");
    esp_matter::lock::ScopedChipStackLock lock(portMAX_DELAY);
    chip::ScopedNodeId target(node_id, active_fabric_index());
    client::read_command::request_t request(paths.Span());
    auto *command = chip::Platform::New<ConsoleReadDelegate>();
    ESP_RETURN_ON_FALSE(command != nullptr, ESP_ERR_NO_MEM, TAG, "Failed to allocate Read");
    CHIP_ERROR err = command->operation.start(target, request, *command);
    if (err != CHIP_NO_ERROR) {
        chip::Platform::Delete(command);
    }
    return err == CHIP_NO_ERROR ? ESP_OK : ESP_FAIL;
}

static esp_err_t controller_read_group_handler(int argc, char **argv)
{
    if (argc == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (strcmp(argv[0], "attr") == 0) {
        return controller_read_handler<chip::app::AttributePathParams>(argc - 1, argv + 1);
    }
    if (strcmp(argv[0], "event") == 0) {
        return controller_read_handler<chip::app::EventPathParams>(argc - 1, argv + 1);
    }
    return ESP_ERR_INVALID_ARG;
}

esp_err_t register_read_commands()
{
    static const command_t commands[] = {
        {
            .name = "read",
            .description = "Usage: controller read attr|event <node> <endpoints> <clusters> <ids>\n"
            "\tPath columns are comma-separated lists of equal length.",
            .handler = controller_read_group_handler,
        },
    };
    return register_commands(commands, sizeof(commands) / sizeof(commands[0]));
}

} // namespace esp_matter::controller::console
