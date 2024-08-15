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

#include <controller/CommissioneeDeviceProxy.h>
#include <esp_matter.h>
#include <esp_matter_client.h>
#include <esp_matter_mem.h>
#include <lib/core/Optional.h>

namespace esp_matter {
namespace controller {

using chip::ScopedNodeId;
using chip::SessionHandle;
using chip::app::ConcreteCommandPath;
using chip::app::StatusIB;
using chip::Messaging::ExchangeManager;
using chip::TLV::TLVReader;
using esp_matter::client::peer_device_t;
using esp_matter::client::interaction::invoke::custom_command_callback;
using esp_matter::client::interaction::custom_encodable_type;

/** Cluster command class to send an invoke interaction command to a server **/
class cluster_command {
public:
    cluster_command(uint64_t destination_id, uint16_t endpoint_id, uint32_t cluster_id, uint32_t command_id,
                    const char *command_data_field,
                    const chip::Optional<uint16_t> timed_invoke_timeout_ms = chip::NullOptional,
                    custom_command_callback::on_success_callback_t on_success = default_success_fcn,
                    custom_command_callback::on_error_callback_t on_error = default_error_fcn)
        : m_destination_id(destination_id)
        , m_endpoint_id(endpoint_id)
        , m_cluster_id(cluster_id)
        , m_command_id(command_id)
        , m_command_data_field(command_data_field, custom_encodable_type::interaction_type::k_invoke_cmd)
        , m_timed_invoke_timeout_ms(timed_invoke_timeout_ms)
        , on_device_connected_cb(on_device_connected_fcn, this)
        , on_device_connection_failure_cb(on_device_connection_failure_fcn, this)
        , on_success_cb(on_success)
        , on_error_cb(on_error)
    {
    }

    ~cluster_command() {}

    esp_err_t send_command();

    bool is_group_command() { return chip::IsGroupId(m_destination_id); }

private:
    uint64_t m_destination_id;
    uint16_t m_endpoint_id;
    uint32_t m_cluster_id;
    uint32_t m_command_id;
    custom_encodable_type m_command_data_field;
    chip::Optional<uint16_t> m_timed_invoke_timeout_ms;

    static void on_device_connected_fcn(void *context, ExchangeManager &exchangeMgr,
                                        const SessionHandle &sessionHandle);
    static void on_device_connection_failure_fcn(void *context, const ScopedNodeId &peerId, CHIP_ERROR error);

    static void default_success_fcn(void *ctx, const ConcreteCommandPath &command_path, const StatusIB &status,
                                    TLVReader *response_data);

    static void default_error_fcn(void *ctx, CHIP_ERROR error);

    static esp_err_t dispatch_group_command(void *context);

    chip::Callback::Callback<chip::OnDeviceConnected> on_device_connected_cb;
    chip::Callback::Callback<chip::OnDeviceConnectionFailure> on_device_connection_failure_cb;

    custom_command_callback::on_success_callback_t on_success_cb;
    custom_command_callback::on_error_callback_t on_error_cb;
};

/** Send cluster invoke command
 *
 * @note When the destination_id is a Matter GroupId, the endpoint_id parameter will be ignored.
 * @note When the command has no data field, command_data_field can be NULL.
 *
 * @param[in] destination_id NodeId or GroupId
 * @param[in] endpoint_id EndpointId
 * @param[in] cluster_id ClusterId
 * @param[in] command_id CommandId
 * @param[in] command_data_field Command data string with JSON format
 *            (https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html#cluster-commands)
 * @param[in] timed_invoke_timeout_ms Timeout in millisecond for timed-invoke command
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t send_invoke_cluster_command(uint64_t destination_id, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t command_id, const char *command_data_field,
                                      chip::Optional<uint16_t> timed_invoke_timeout_ms = chip::NullOptional);

} // namespace controller
} // namespace esp_matter
