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

#include <esp_check.h>
#include <esp_matter_client.h>
#include <esp_matter_controller_client.h>
#include <esp_matter_controller_utils.h>
#include <esp_matter_controller_write_command.h>
#include <json_to_tlv.h>

#include <app/OperationalSessionSetup.h>
#include <app/server/Server.h>

using namespace chip::app::Clusters;
using namespace esp_matter::client;
using chip::ByteSpan;
using chip::DeviceProxy;
using chip::app::DataModel::List;
using chip::Platform::MakeUnique;
using chip::Platform::New;
using chip::TLV::ContextTag;
using chip::TLV::TLVReader;
using chip::TLV::TLVType;
using chip::TLV::TLVWriter;
using attribute_data_tag = chip::app::AttributeDataIB::Tag;

static const char *TAG = "write_command";
static constexpr size_t k_encoded_buf_size = chip::kMaxAppMessageLen;

namespace esp_matter {
namespace controller {

void write_command::on_device_connected_fcn(void *context, ExchangeManager &exchangeMgr,
                                            const SessionHandle &sessionHandle)
{
    write_command *cmd = (write_command *)context;
    chip::OperationalDeviceProxy device_proxy(&exchangeMgr, sessionHandle);
    esp_err_t err = interaction::write::send_request(&device_proxy, cmd->m_attr_paths, cmd->m_attr_vals,
                                                     cmd->m_chunked_callback, cmd->m_timed_write_timeout_ms);
    if (err != ESP_OK) {
        chip::Platform::Delete(cmd);
    }
}

void write_command::on_device_connection_failure_fcn(void *context, const ScopedNodeId &peerId, CHIP_ERROR error)
{
    write_command *cmd = (write_command *)context;
    chip::Platform::Delete(cmd);
    return;
}

esp_err_t write_command::send_command()
{
#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    chip::Server &server = chip::Server::GetInstance();
    server.GetCASESessionManager()->FindOrEstablishSession(ScopedNodeId(m_node_id, get_fabric_index()),
                                                           &on_device_connected_cb, &on_device_connection_failure_cb);
    return ESP_OK;
#else
    auto &controller_instance = esp_matter::controller::matter_controller_client::get_instance();
#ifdef CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
    if (CHIP_NO_ERROR ==
        controller_instance.get_commissioner()->GetConnectedDevice(m_node_id, &on_device_connected_cb,
                                                                   &on_device_connection_failure_cb)) {
        return ESP_OK;
    }
#else
    if (CHIP_NO_ERROR ==
        controller_instance.get_controller()->GetConnectedDevice(m_node_id, &on_device_connected_cb,
                                                                 &on_device_connection_failure_cb)) {
        return ESP_OK;
    }
#endif // CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    chip::Platform::Delete(this);
    return ESP_FAIL;
}

esp_err_t send_write_attr_command(uint64_t node_id, uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                                  const char *attr_val_json_str, chip::Optional<uint16_t> timed_write_timeout_ms)
{
    if (!attr_val_json_str) {
        ESP_LOGE(TAG, "attribute value json string cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    write_command *cmd = chip::Platform::New<write_command>(node_id, endpoint_id, cluster_id, attribute_id,
                                                            attr_val_json_str, timed_write_timeout_ms);

    if (!cmd) {
        ESP_LOGE(TAG, "Failed to alloc memory for cluster_command");
        return ESP_ERR_NO_MEM;
    }

    return cmd->send_command();
}

esp_err_t send_write_attr_command(uint64_t node_id, ScopedMemoryBufferWithSize<uint16_t> &endpoint_ids,
                                  ScopedMemoryBufferWithSize<uint32_t> &cluster_ids,
                                  ScopedMemoryBufferWithSize<uint32_t> &attribute_ids, const char *attr_val_json_str,
                                  chip::Optional<uint16_t> timed_write_timeout_ms)
{
    if (endpoint_ids.AllocatedSize() != cluster_ids.AllocatedSize() ||
        endpoint_ids.AllocatedSize() != attribute_ids.AllocatedSize()) {
        ESP_LOGE(TAG,
                 "The endpoint_id array length should be the same as the cluster_ids array length"
                 "and the attribute_ids array length");
        return ESP_ERR_INVALID_ARG;
    }
    ScopedMemoryBufferWithSize<AttributePathParams> attr_paths;
    ScopedMemoryBufferWithSize<EventPathParams> event_paths;
    attr_paths.Alloc(endpoint_ids.AllocatedSize());
    if (!attr_paths.Get()) {
        ESP_LOGE(TAG, "Failed to alloc memory for attribute paths");
        return ESP_ERR_NO_MEM;
    }
    for (size_t i = 0; i < attr_paths.AllocatedSize(); ++i) {
        attr_paths[i] = AttributePathParams(endpoint_ids[i], cluster_ids[i], attribute_ids[i]);
    }

    write_command *cmd =
        chip::Platform::New<write_command>(node_id, std::move(attr_paths), attr_val_json_str, timed_write_timeout_ms);
    if (!cmd) {
        ESP_LOGE(TAG, "Failed to alloc memory for read_command");
        return ESP_ERR_NO_MEM;
    }
    return cmd->send_command();
}

} // namespace controller
} // namespace esp_matter
