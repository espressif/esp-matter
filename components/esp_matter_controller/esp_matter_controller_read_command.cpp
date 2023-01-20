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
#if CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
#include <esp_matter_commissioner.h>
#else
#include <app/server/Server.h>
#endif
#include <esp_matter_controller_read_command.h>

#include "DataModelLogger.h"

using namespace chip::app::Clusters;
using chip::DeviceProxy;
using chip::app::InteractionModelEngine;
using chip::app::ReadClient;
using chip::app::ReadPrepareParams;

static const char *TAG = "read_command";

namespace esp_matter {
namespace controller {

void read_command::on_device_connected_fcn(void *context, ExchangeManager &exchangeMgr, SessionHandle &sessionHandle)
{
    read_command *cmd = (read_command *)context;
    ReadPrepareParams params(sessionHandle);

    if (cmd->get_command_type() == READ_ATTRIBUTE) {
        params.mpEventPathParamsList = nullptr;
        params.mEventPathParamsListSize = 0;
        params.mpAttributePathParamsList = &cmd->get_attr_path();
        params.mAttributePathParamsListSize = 1;
    } else if (cmd->get_command_type() == READ_EVENT) {
        params.mpEventPathParamsList = &cmd->get_event_path();
        params.mEventPathParamsListSize = 1;
        params.mpAttributePathParamsList = nullptr;
        params.mAttributePathParamsListSize = 0;
    }
    params.mIsFabricFiltered = 0;
    params.mpDataVersionFilterList = nullptr;
    params.mDataVersionFilterListSize = 0;

    ReadClient *client =
        chip::Platform::New<ReadClient>(InteractionModelEngine::GetInstance(), &exchangeMgr,
                                        cmd->get_buffered_read_cb(), ReadClient::InteractionType::Read);
    if (!client) {
        ESP_LOGE(TAG, "Failed to alloc memory for read client");
        chip::Platform::Delete(cmd);
    }
    if (CHIP_NO_ERROR != client->SendRequest(params)) {
        ESP_LOGE(TAG, "Failed to send read request");
        chip::Platform::Delete(client);
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
#if CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
    if (CHIP_NO_ERROR ==
        commissioner::get_device_commissioner()->GetConnectedDevice(m_node_id, &on_device_connected_cb,
                                                                    &on_device_connection_failure_cb)) {
        return ESP_OK;
    }
#else
    chip::Server *server = &(chip::Server::GetInstance());
    server->GetCASESessionManager()->FindOrEstablishSession(ScopedNodeId(m_node_id, /* fabric index */ 1),
                                                            &on_device_connected_cb, &on_device_connection_failure_cb);
    return ESP_OK;
#endif

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
    chip::Platform::Delete(apReadClient);
    chip::Platform::Delete(this);
}

esp_err_t send_read_attr_command(uint64_t node_id, uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id)
{
    read_command *cmd =
        chip::Platform::New<read_command>(node_id, endpoint_id, cluster_id, attribute_id, READ_ATTRIBUTE);
    if (!cmd) {
        ESP_LOGE(TAG, "Failed to alloc memory for read_command");
        return ESP_ERR_NO_MEM;
    }

    return cmd->send_command();
}

esp_err_t send_read_event_command(uint64_t node_id, uint16_t endpoint_id, uint32_t cluster_id, uint32_t event_id)
{
    read_command *cmd = chip::Platform::New<read_command>(node_id, endpoint_id, cluster_id, event_id, READ_EVENT);
    if (!cmd) {
        ESP_LOGE(TAG, "Failed to alloc memory for read_command");
        return ESP_ERR_NO_MEM;
    }

    return cmd->send_command();
}

} // namespace controller
} // namespace esp_matter
