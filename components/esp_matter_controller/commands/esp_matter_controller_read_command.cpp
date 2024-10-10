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

#include <controller/CommissioneeDeviceProxy.h>
#include <esp_log.h>
#include <esp_matter_client.h>
#include <esp_matter_controller_client.h>
#include <esp_matter_controller_read_command.h>

#include <app/server/Server.h>

#include <commands/clusters/DataModelLogger.h>

using namespace chip::app::Clusters;
using namespace esp_matter::client;
using chip::DeviceProxy;
using chip::app::InteractionModelEngine;
using chip::app::ReadClient;
using chip::app::ReadPrepareParams;

static const char *TAG = "read_command";

namespace esp_matter {
namespace controller {

void read_command::on_device_connected_fcn(void *context, ExchangeManager &exchangeMgr,
                                           const SessionHandle &sessionHandle)
{
    read_command *cmd = (read_command *)context;
    chip::OperationalDeviceProxy device_proxy(&exchangeMgr, sessionHandle);
    esp_err_t err = interaction::read::send_request(&device_proxy, cmd->m_attr_paths.Get(),
                                                    cmd->m_attr_paths.AllocatedSize(), cmd->m_event_paths.Get(),
                                                    cmd->m_event_paths.AllocatedSize(), cmd->m_buffered_read_cb);
    if (err != ESP_OK) {
        chip::Platform::Delete(cmd);
    }
    return;
}

void read_command::on_device_connection_failure_fcn(void *context, const ScopedNodeId &peerId, CHIP_ERROR error)
{
    read_command *cmd = (read_command *)context;
    chip::Platform::Delete(cmd);
    return;
}

esp_err_t read_command::send_command()
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

// ReadClient Callback Interface
void read_command::OnAttributeData(const chip::app::ConcreteDataAttributePath &path, chip::TLV::TLVReader *data,
                                   const chip::app::StatusIB &status)
{
    CHIP_ERROR error = status.ToChipError();
    if (CHIP_NO_ERROR != error) {
        ESP_LOGE(TAG, "Response Failure: %s", chip::ErrorStr(error));
        return;
    }

    if (data == nullptr) {
        ESP_LOGE(TAG, "Response Failure: No Data");
        return;
    }
    if (attribute_data_cb) {
        chip::TLV::TLVReader data_cpy;
        data_cpy.Init(*data);
        attribute_data_cb(m_node_id, path, &data_cpy);
    }
    error = DataModelLogger::LogAttribute(path, data);
    if (CHIP_NO_ERROR != error) {
        ESP_LOGE(TAG, "Response Failure: Can not decode Data");
    }
}

void read_command::OnEventData(const chip::app::EventHeader &event_header, chip::TLV::TLVReader *data,
                               const chip::app::StatusIB *status)
{
    CHIP_ERROR error = CHIP_NO_ERROR;
    if (status != nullptr) {
        error = status->ToChipError();
        if (CHIP_NO_ERROR != error) {
            ESP_LOGE(TAG, "Response Failure: %s", chip::ErrorStr(error));
            return;
        }
    }

    if (data == nullptr) {
        ESP_LOGE(TAG, "Response Failure: No Data");
        return;
    }
    if (event_data_cb) {
        chip::TLV::TLVReader data_cpy;
        data_cpy.Init(*data);
        event_data_cb(m_node_id, event_header, &data_cpy);
    }
    error = DataModelLogger::LogEvent(event_header, data);
    if (CHIP_NO_ERROR != error) {
        ESP_LOGE(TAG, "Response Failure: Can not decode Data");
    }
}

void read_command::OnError(CHIP_ERROR error)
{
    ESP_LOGE(TAG, "Read Error: %s", chip::ErrorStr(error));
}

void read_command::OnDeallocatePaths(chip::app::ReadPrepareParams &&aReadPrepareParams)
{
    // Intentionally empty because the AttributePathParamsList or EventPathParamsList will be deleted with the
    // read_command.
}

void read_command::OnDone(ReadClient *apReadClient)
{
    ESP_LOGI(TAG, "read done");
    if (read_done_cb) {
        read_done_cb(m_node_id, m_attr_paths, m_event_paths);
    }
    chip::Platform::Delete(this);
}

esp_err_t send_read_attr_command(uint64_t node_id, ScopedMemoryBufferWithSize<uint16_t> &endpoint_ids,
                                 ScopedMemoryBufferWithSize<uint32_t> &cluster_ids,
                                 ScopedMemoryBufferWithSize<uint32_t> &attribute_ids)
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

    read_command *cmd = chip::Platform::New<read_command>(node_id, std::move(attr_paths), std::move(event_paths),
                                                          nullptr, nullptr, nullptr);
    if (!cmd) {
        ESP_LOGE(TAG, "Failed to alloc memory for read_command");
        return ESP_ERR_NO_MEM;
    }
    return cmd->send_command();
}

esp_err_t send_read_event_command(uint64_t node_id, ScopedMemoryBufferWithSize<uint16_t> &endpoint_ids,
                                  ScopedMemoryBufferWithSize<uint32_t> &cluster_ids,
                                  ScopedMemoryBufferWithSize<uint32_t> &event_ids)
{
    if (endpoint_ids.AllocatedSize() != cluster_ids.AllocatedSize() ||
        endpoint_ids.AllocatedSize() != event_ids.AllocatedSize()) {
        ESP_LOGE(TAG,
                 "The endpoint_id array length should be the same as the cluster_ids array length"
                 "and the attribute_ids array length");
        return ESP_ERR_INVALID_ARG;
    }
    ScopedMemoryBufferWithSize<AttributePathParams> attr_paths;
    ScopedMemoryBufferWithSize<EventPathParams> event_paths;
    event_paths.Alloc(endpoint_ids.AllocatedSize());
    if (!event_paths.Get()) {
        ESP_LOGE(TAG, "Failed to alloc memory for attribute paths");
        return ESP_ERR_NO_MEM;
    }
    for (size_t i = 0; i < event_paths.AllocatedSize(); ++i) {
        event_paths[i] = EventPathParams(endpoint_ids[i], cluster_ids[i], event_ids[i]);
    }

    read_command *cmd = chip::Platform::New<read_command>(node_id, std::move(attr_paths), std::move(event_paths),
                                                          nullptr, nullptr, nullptr);
    if (!cmd) {
        ESP_LOGE(TAG, "Failed to alloc memory for read_command");
        return ESP_ERR_NO_MEM;
    }
    return cmd->send_command();
}

esp_err_t send_read_attr_command(uint64_t node_id, uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id)
{
    ScopedMemoryBufferWithSize<uint16_t> endpoint_ids;
    ScopedMemoryBufferWithSize<uint32_t> cluster_ids;
    ScopedMemoryBufferWithSize<uint32_t> attribute_ids;
    endpoint_ids.Alloc(1);
    cluster_ids.Alloc(1);
    attribute_ids.Alloc(1);
    if (!(endpoint_ids.Get() && cluster_ids.Get() && attribute_ids.Get())) {
        return ESP_ERR_NO_MEM;
    } else {
        *endpoint_ids.Get() = endpoint_id;
        *cluster_ids.Get() = cluster_id;
        *attribute_ids.Get() = attribute_id;
    }
    return send_read_attr_command(node_id, endpoint_ids, cluster_ids, attribute_ids);
}

esp_err_t send_read_event_command(uint64_t node_id, uint16_t endpoint_id, uint32_t cluster_id, uint32_t event_id)
{
    ScopedMemoryBufferWithSize<uint16_t> endpoint_ids;
    ScopedMemoryBufferWithSize<uint32_t> cluster_ids;
    ScopedMemoryBufferWithSize<uint32_t> event_ids;
    endpoint_ids.Alloc(1);
    cluster_ids.Alloc(1);
    event_ids.Alloc(1);
    if (!(endpoint_ids.Get() && cluster_ids.Get() && event_ids.Get())) {
        return ESP_ERR_NO_MEM;
    } else {
        *endpoint_ids.Get() = endpoint_id;
        *cluster_ids.Get() = cluster_id;
        *event_ids.Get() = event_id;
    }
    return send_read_event_command(node_id, endpoint_ids, cluster_ids, event_ids);
}

} // namespace controller
} // namespace esp_matter
