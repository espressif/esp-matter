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

#include "local_console.h"

#include <esp_log.h>
#include <esp_matter_client.h>
#include <esp_matter_client_invoke_command.h>
#include <esp_matter_client_read_command.h>
#include <esp_matter_client_write_command.h>
#include <esp_matter_console.h>
#include <tlv_to_json.h>

#include <app/server/Server.h>
#include <lib/support/CHIPArgParser.hpp>
#include <lib/support/CHIPMem.h>
#include <lib/support/ScopedMemoryBuffer.h>
#include <stdio.h>
#include <string.h>

namespace local_console {
namespace {

using namespace chip;
using namespace chip::app;
using namespace esp_matter;

constexpr const char *TAG = "local_console";

enum class operation_t { read, write, invoke };

template <typename T>
bool parse_unsigned(const char *text, T &value)
{
    return text != nullptr && *text >= '0' && *text <= '9' &&
           ArgParser::ParseInt(text, value, text[0] == '0' && (text[1] == 'x' || text[1] == 'X') ? 16 : 10);
}

void print_response(const StatusIB &status, TLV::TLVReader *value = nullptr)
{
    if (!status.IsSuccess() || value == nullptr) {
        ESP_LOGI(TAG, "Status: %s", ErrorStr(status.ToChipError()));
        return;
    }

    cJSON *json = nullptr;
    if (tlv_to_json(*value, &json) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to decode response");
        return;
    }
    char *text = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);
    if (text != nullptr) {
        ESP_LOGI(TAG, "Value: %s", text);
        cJSON_free(text);
    } else {
        ESP_LOGE(TAG, "Failed to allocate response text");
    }
}

class LocalCommand final : public client::read_command::delegate,
    public client::write_command::delegate, public client::invoke_command::delegate {
public:
    CHIP_ERROR Start(operation_t operation, const ScopedNodeId &target, EndpointId endpoint,
                     ClusterId cluster, uint32_t id, const char *json)
    {
        VerifyOrReturnError(!mBusy, CHIP_ERROR_BUSY);
        mRead.stop();
        mWrite.stop();
        mInvoke.stop();
        if (operation == operation_t::read) {
            AttributePathParams path(endpoint, cluster, id);
            client::read_command::request_t request(Span<const AttributePathParams>(&path, 1));
            ReturnErrorOnFailure(mRead.start(target, request, *this));
            mBusy = true;
            return CHIP_NO_ERROR;
        }

        if (operation == operation_t::write) {
            client::json_encodable value(json);
            client::write_command::attribute_write_t attribute = {
                .path = ConcreteDataAttributePath(endpoint, cluster, id),
                .value = &value,
            };
            client::write_command::request_t request = {
                .attributes = Span<const client::write_command::attribute_write_t>(&attribute, 1),
            };
            ReturnErrorOnFailure(mWrite.start(target, request, this));
            mBusy = true;
            return CHIP_NO_ERROR;
        }

        Platform::ScopedMemoryBufferWithSize<char> fields_json;
        VerifyOrReturnError(fields_json.Alloc(strlen(json) + sizeof("{\"0:OBJ\":}")), CHIP_ERROR_NO_MEMORY);
        snprintf(fields_json.Get(), fields_json.AllocatedSize(), "{\"0:OBJ\":%s}", json);
        client::json_encodable fields(fields_json.Get());
        client::invoke_command::request_t request = {
            .endpoint_id = endpoint,
            .cluster_id = cluster,
            .command_id = id,
            .fields = &fields,
        };
        ReturnErrorOnFailure(mInvoke.start(target, request, this));
        mBusy = true;
        return CHIP_NO_ERROR;
    }

    void OnAttribute(const client::target_t &, const ConcreteDataAttributePath &, TLV::TLVReader *value, const StatusIB &status) override
    {
        print_response(status, value);
    }

    void OnResponse(const client::target_t &, const ConcreteDataAttributePath &, const StatusIB &status) override
    {
        print_response(status);
    }

    void OnResponse(const client::target_t &, const ConcreteCommandPath &, const StatusIB &status, TLV::TLVReader *value) override
    {
        print_response(status, value);
    }

    void OnDone(const client::target_t &, CHIP_ERROR error) override
    {
        if (error != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Local operation failed: %s", ErrorStr(error));
        }
        mBusy = false;
    }

private:
    client::read_command::interaction mRead;
    client::write_command::interaction mWrite;
    client::invoke_command::interaction mInvoke;
    bool mBusy = false;
};

esp_err_t local_command_handler(int argc, char **argv)
{
    if (argc == 0 || (argc == 1 && strcmp(argv[0], "help") == 0)) {
        ESP_LOGI(TAG, "read attr <endpoint> <cluster> <attribute>");
        ESP_LOGI(TAG, "write attr <endpoint> <cluster> <attribute> -- <tagged-json>");
        ESP_LOGI(TAG, "invoke <endpoint> <cluster> <command> -- <json-fields>");
        ESP_LOGI(TAG, "Invoke fields: {} or {\"0:U16\":42}");
        return ESP_OK;
    }

    operation_t operation;
    if (strcmp(argv[0], "read") == 0) {
        operation = operation_t::read;
    } else if (strcmp(argv[0], "write") == 0) {
        operation = operation_t::write;
    } else if (strcmp(argv[0], "invoke") == 0) {
        operation = operation_t::invoke;
    } else {
        return ESP_ERR_INVALID_ARG;
    }

    const int offset = operation == operation_t::invoke ? 1 : 2;
    const int expected = operation == operation_t::read ? 5 : offset + 5;
    if (argc != expected || (offset == 2 && strcmp(argv[1], "attr") != 0) ||
            (operation != operation_t::read && strcmp(argv[argc - 2], "--") != 0)) {
        return ESP_ERR_INVALID_ARG;
    }
    EndpointId endpoint;
    ClusterId cluster;
    uint32_t id;
    if (!parse_unsigned(argv[offset], endpoint) || endpoint == kInvalidEndpointId ||
            !parse_unsigned(argv[offset + 1], cluster) || !parse_unsigned(argv[offset + 2], id)) {
        return ESP_ERR_INVALID_ARG;
    }

    auto &fabric_table = Server::GetInstance().GetFabricTable();
    auto fabric = fabric_table.begin();
    if (fabric == fabric_table.end()) {
        ESP_LOGE(TAG, "Commission the bridge before using local commands");
        return ESP_ERR_INVALID_STATE;
    }
    ScopedNodeId target(fabric->GetNodeId(), fabric->GetFabricIndex());
    client::execution_t execution;
    CHIP_ERROR error = client::resolve_target(target, execution);
    if (error != CHIP_NO_ERROR || execution != client::execution_t::local) {
        return ESP_ERR_INVALID_STATE;
    }
    static LocalCommand *command = nullptr;
    if (command == nullptr) {
        command = Platform::New<LocalCommand>();
    }
    VerifyOrReturnError(command != nullptr, ESP_ERR_NO_MEM);
    error = command->Start(operation, target, endpoint, cluster, id, operation == operation_t::read ? nullptr : argv[argc - 1]);
    if (error != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Local submission failed: %s", ErrorStr(error));
        return error == CHIP_ERROR_BUSY ? ESP_ERR_INVALID_STATE : ESP_FAIL;
    }
    return ESP_OK;
}

} // namespace

esp_err_t register_commands()
{
    static const esp_matter::console::command_t command = {
        .name = "local",
        .description = "Local Read, Write, and Invoke: local help",
        .handler = local_command_handler,
    };
    return esp_matter::console::add_commands(&command, 1);
}

} // namespace local_console
