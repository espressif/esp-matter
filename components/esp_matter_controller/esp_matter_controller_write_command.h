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

namespace esp_matter {
namespace controller {

using chip::ScopedNodeId;
using chip::SessionHandle;
using chip::app::AttributePathParams;
using chip::app::ChunkedWriteCallback;
using chip::app::ConcreteDataAttributePath;
using chip::app::StatusIB;
using chip::app::WriteClient;
using chip::Messaging::ExchangeManager;
using esp_matter::client::peer_device_t;

template <class T>
class write_command : public WriteClient::Callback {
public:
    write_command(uint64_t node_id, uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, T attribute_val)
        : m_node_id(node_id)
        , m_attr_path(endpoint_id, cluster_id, attribute_id)
        , m_chunked_callback(this)
        , m_attr_val(attribute_val)
        , m_attr_free(nullptr)
        , m_attr_free_ctx(nullptr)
        , on_device_connected_cb(on_device_connected_fcn, this)
        , on_device_connection_failure_cb(on_device_connection_failure_fcn, this)
    {
    }

    ~write_command()
    {
        if (m_attr_free && m_attr_free_ctx) {
            m_attr_free(m_attr_free_ctx);
        }
    }

    AttributePathParams &get_attribute_path() { return m_attr_path; }

    T &get_attribute_val() { return m_attr_val; }

    ChunkedWriteCallback &get_chunked_write_callback() { return m_chunked_callback; }

    esp_err_t send_command();

    using attribute_free_handler = void (*)(void *);
    esp_err_t set_attribute_free_handler(attribute_free_handler attr_free_handler, void *ctx)
    {
        m_attr_free = attr_free_handler;
        m_attr_free_ctx = ctx;
        return ESP_OK;
    }

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
    T m_attr_val;
    // We need to alloc memory for writing complex attributes asynchronously and free the memory when we delete the
    // write_command.
    attribute_free_handler m_attr_free;
    void *m_attr_free_ctx;

    static void on_device_connected_fcn(void *context, ExchangeManager &exchangeMgr, SessionHandle &sessionHandle);
    static void on_device_connection_failure_fcn(void *context, const ScopedNodeId &peerId, CHIP_ERROR error);

    chip::Callback::Callback<chip::OnDeviceConnected> on_device_connected_cb;
    chip::Callback::Callback<chip::OnDeviceConnectionFailure> on_device_connection_failure_cb;
};

esp_err_t send_write_attr_command(uint64_t node_id, uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                                  char *attribute_val_str);

} // namespace controller
} // namespace esp_matter
