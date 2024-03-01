// Copyright 2022 Espressif Systems (Shanghai) PTE LTD
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

#include <app/ChunkedWriteCallback.h>
#include <controller/CommissioneeDeviceProxy.h>
#include <esp_matter.h>
#include <esp_matter_mem.h>

namespace esp_matter {
namespace controller {

using chip::Optional;
using chip::ScopedNodeId;
using chip::SessionHandle;
using chip::app::AttributePathParams;
using chip::app::ChunkedWriteCallback;
using chip::app::ConcreteDataAttributePath;
using chip::app::StatusIB;
using chip::app::WriteClient;
using chip::Messaging::ExchangeManager;
using chip::TLV::TLVElementType;
using esp_matter::client::peer_device_t;

constexpr char *k_null_attribute_val_str = "null";
constexpr size_t k_attr_val_str_buf_size = CONFIG_ESP_MATTER_CONTROLLER_JSON_STRING_BUFFER_LEN;

class write_command : public WriteClient::Callback {
public:
    write_command(uint64_t node_id, uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                  const char *attribute_val_str)
        : m_node_id(node_id)
        , m_attr_path(endpoint_id, cluster_id, attribute_id)
        , m_chunked_callback(this)
        , on_device_connected_cb(on_device_connected_fcn, this)
        , on_device_connection_failure_cb(on_device_connection_failure_fcn, this)
    {
        if (attribute_val_str) {
            strncpy(m_attr_val_str, attribute_val_str, k_attr_val_str_buf_size - 1);
            m_attr_val_str[strnlen(attribute_val_str, k_attr_val_str_buf_size - 1)] = 0;
        } else {
            strcpy(m_attr_val_str, k_null_attribute_val_str);
            m_attr_val_str[strlen(k_null_attribute_val_str)] = 0;
        }
    }

    ~write_command() {}

    esp_err_t send_command();

    // WriteClient Callback Interface
    void OnResponse(const WriteClient *client, const ConcreteDataAttributePath &path, StatusIB status) override
    {
        CHIP_ERROR error = status.ToChipError();
        if (CHIP_NO_ERROR != error) {
            ChipLogError(chipTool, "Response Failure: %s", chip::ErrorStr(error));
        }
    }

    void OnError(const WriteClient *client, CHIP_ERROR error) override
    {
        ChipLogProgress(chipTool, "Error: %s", chip::ErrorStr(error));
    }

    void OnDone(WriteClient *client) override
    {
        ChipLogProgress(chipTool, "Write Done");
        chip::Platform::Delete(client);
        chip::Platform::Delete(this);
    }

private:
    uint64_t m_node_id;
    AttributePathParams m_attr_path;
    ChunkedWriteCallback m_chunked_callback;
    char m_attr_val_str[k_attr_val_str_buf_size];

    static void on_device_connected_fcn(void *context, ExchangeManager &exchangeMgr,
                                        const SessionHandle &sessionHandle);
    static void on_device_connection_failure_fcn(void *context, const ScopedNodeId &peerId, CHIP_ERROR error);

    static esp_err_t encode_attribute_value(uint8_t *encoded_buf, size_t encoded_buf_size,
                                            const char *attr_val_json_str, TLVReader &out_reader);

    chip::Callback::Callback<chip::OnDeviceConnected> on_device_connected_cb;
    chip::Callback::Callback<chip::OnDeviceConnectionFailure> on_device_connection_failure_cb;
};

esp_err_t send_write_attr_command(uint64_t node_id, uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                                  const char *attr_val_json_str);

} // namespace controller
} // namespace esp_matter
