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
using chip::Platform::ScopedMemoryBufferWithSize;
using chip::TLV::TLVElementType;
using esp_matter::client::peer_device_t;
using esp_matter::client::interaction::custom_encodable_type;
using esp_matter::client::interaction::multiple_write_encodable_type;

/** Write command class to send a write interaction command to a server **/
class write_command : public WriteClient::Callback {
public:
    /** Constructor for command with multiple paths**/
    write_command(uint64_t node_id, ScopedMemoryBufferWithSize<AttributePathParams> &&attr_paths,
                  const char *attribute_val_str,
                  const chip::Optional<uint16_t> timed_write_timeout_ms = chip::NullOptional)
        : m_node_id(node_id)
        , m_attr_paths(std::move(attr_paths))
        , m_chunked_callback(this)
        , m_attr_vals(attribute_val_str)
        , m_timed_write_timeout_ms(timed_write_timeout_ms)
        , on_device_connected_cb(on_device_connected_fcn, this)
        , on_device_connection_failure_cb(on_device_connection_failure_fcn, this)
    {
    }

    /** Constructor for command with an attribute path**/
    write_command(uint64_t node_id, uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                  const char *attribute_val_str,
                  const chip::Optional<uint16_t> timed_write_timeout_ms = chip::NullOptional)
        : m_node_id(node_id)
        , m_chunked_callback(this)
        , m_attr_vals(attribute_val_str)
        , m_timed_write_timeout_ms(timed_write_timeout_ms)
        , on_device_connected_cb(on_device_connected_fcn, this)
        , on_device_connection_failure_cb(on_device_connection_failure_fcn, this)
    {
        m_attr_paths.Alloc(1);
        if (m_attr_paths.Get()) {
            m_attr_paths[0] = AttributePathParams(endpoint_id, cluster_id, attribute_id);
        } else {
            ChipLogError(DeviceLayer, "Alloc space for attribute path failed!");
            assert(0);
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
        chip::Platform::Delete(this);
    }

private:
    uint64_t m_node_id;
    ScopedMemoryBufferWithSize<AttributePathParams> m_attr_paths;
    ChunkedWriteCallback m_chunked_callback;
    multiple_write_encodable_type m_attr_vals;
    chip::Optional<uint16_t> m_timed_write_timeout_ms;

    static void on_device_connected_fcn(void *context, ExchangeManager &exchangeMgr,
                                        const SessionHandle &sessionHandle);
    static void on_device_connection_failure_fcn(void *context, const ScopedNodeId &peerId, CHIP_ERROR error);

    chip::Callback::Callback<chip::OnDeviceConnected> on_device_connected_cb;
    chip::Callback::Callback<chip::OnDeviceConnectionFailure> on_device_connection_failure_cb;
};

/** Send write attribute command
 *
 * @param[in] node_id Remote NodeId
 * @param[in] endpoint_id EndpointId
 * @param[in] cluster_id ClusterId
 * @param[in] attribute_id AttributeId
 * @param[in] attr_val_json_str Attribute value string with JSON format
 *            (https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html#write-attribute-commands)
 * @param[in] timed_write_timeout_ms Timeout in millisecond for timed-write attribute
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t send_write_attr_command(uint64_t node_id, uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                                  const char *attr_val_json_str,
                                  chip::Optional<uint16_t> timed_write_timeout_ms = chip::NullOptional);

/** Send write attribute command to multiple attribute paths
 *
 * @param[in] node_id Remote NodeId
 * @param[in] endpoint_ids EndpointIds
 * @param[in] cluster_ids ClusterIds
 * @param[in] attribute_ids AttributeIds
 * @param[in] attr_val_json_str Attribute value string with JSON format
 *            (https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html#write-attribute-commands)
 * @param[in] timed_write_timeout_ms Timeout in millisecond for timed-write attributes
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t send_write_attr_command(uint64_t node_id, ScopedMemoryBufferWithSize<uint16_t> &endpoint_ids,
                                  ScopedMemoryBufferWithSize<uint32_t> &cluster_ids,
                                  ScopedMemoryBufferWithSize<uint32_t> &attribute_ids, const char *attr_val_json_str,
                                  chip::Optional<uint16_t> timed_write_timeout_ms = chip::NullOptional);

} // namespace controller
} // namespace esp_matter
