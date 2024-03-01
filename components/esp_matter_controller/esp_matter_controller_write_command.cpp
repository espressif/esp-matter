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
#include <esp_matter_controller_utils.h>
#include <esp_matter_controller_write_command.h>
#include <json_to_tlv.h>
#if CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
#include <esp_matter_commissioner.h>
#else
#include <app/server/Server.h>
#endif

using namespace chip::app::Clusters;
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

esp_err_t write_command::encode_attribute_value(uint8_t *encoded_buf, size_t encoded_buf_size,
                                                const char *attr_val_json_str, TLVReader &out_reader)
{
    TLVWriter writer;
    uint32_t encoded_len = 0;
    TLVReader reader;

    writer.Init(encoded_buf, encoded_buf_size);
    ESP_RETURN_ON_ERROR(json_to_tlv(attr_val_json_str, writer, chip::TLV::AnonymousTag()), TAG,
                        "Failed to parse attribute value");
    ESP_RETURN_ON_FALSE(writer.Finalize() == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to finalize tlv writer");
    encoded_len = writer.GetLengthWritten();
    reader.Init(encoded_buf, encoded_len);
    ESP_RETURN_ON_FALSE(reader.Next() == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to read next");
    ESP_RETURN_ON_FALSE(reader.GetType() == TLVType::kTLVType_Structure, ESP_ERR_INVALID_ARG, TAG,
                        "The TLV type must be structure");
    ESP_RETURN_ON_FALSE(reader.OpenContainer(out_reader) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to open container");
    ESP_RETURN_ON_FALSE(out_reader.Next() == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to read next");
    return ESP_OK;
}

void write_command::on_device_connected_fcn(void *context, ExchangeManager &exchangeMgr,
                                            const SessionHandle &sessionHandle)
{
    write_command *cmd = (write_command *)context;
    if (cmd->m_attr_path.HasWildcardEndpointId()) {
        ESP_LOGE(TAG, "Endpoint Id Invalid");
        chip::Platform::Delete(cmd);
        return;
    }
    ConcreteDataAttributePath path(cmd->m_attr_path.mEndpointId, cmd->m_attr_path.mClusterId,
                                   cmd->m_attr_path.mAttributeId);
    chip::Platform::ScopedMemoryBuffer<uint8_t> encoded_buf;

    auto write_client = MakeUnique<WriteClient>(&exchangeMgr, &(cmd->m_chunked_callback), chip::NullOptional, false);
    if (write_client == nullptr) {
        ESP_LOGE(TAG, "Failed to alloc memory for WriteClient");
        chip::Platform::Delete(cmd);
        return;
    }
    encoded_buf.Alloc(k_encoded_buf_size);
    if (!encoded_buf.Get()) {
        ESP_LOGE(TAG, "Failed to alloc memory for encoded_buf");
        chip::Platform::Delete(cmd);
        return;
    }
    TLVReader attr_val_reader;
    if (encode_attribute_value(encoded_buf.Get(), k_encoded_buf_size, cmd->m_attr_val_str, attr_val_reader) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to encode attribute value to a TLV reader");
        chip::Platform::Delete(cmd);
        return;
    }

    if (write_client->PutPreencodedAttribute(path, attr_val_reader) != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to put pre-encoded attribute value to WriteClient");
        chip::Platform::Delete(cmd);
        return;
    }

    if (write_client->SendWriteRequest(sessionHandle) != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to Send Write Request");
        chip::Platform::Delete(cmd);
        return;
    }
    // Release the write_client as it will be managed by the callbacks
    write_client.release();
}

void write_command::on_device_connection_failure_fcn(void *context, const ScopedNodeId &peerId, CHIP_ERROR error)
{
    write_command *cmd = (write_command *)context;
    chip::Platform::Delete(cmd);
    return;
}

esp_err_t write_command::send_command()
{
#if CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
    if (CHIP_NO_ERROR ==
        commissioner::get_device_commissioner()->GetConnectedDevice(m_node_id, &on_device_connected_cb,
                                                                    &on_device_connection_failure_cb)) {
        return ESP_OK;
    }
#else
    chip::Server *server = &(chip::Server::GetInstance());
    server->GetCASESessionManager()->FindOrEstablishSession(ScopedNodeId(m_node_id, get_fabric_index()),
                                                            &on_device_connected_cb, &on_device_connection_failure_cb);
    return ESP_OK;
#endif
    chip::Platform::Delete(this);
    return ESP_FAIL;
}

esp_err_t send_write_attr_command(uint64_t node_id, uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                                  const char *attr_val_json_str)
{
    if (!attr_val_json_str) {
        ESP_LOGE(TAG, "attribute value json string cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    write_command *cmd =
        chip::Platform::New<write_command>(node_id, endpoint_id, cluster_id, attribute_id, attr_val_json_str);

    if (!cmd) {
        ESP_LOGE(TAG, "Failed to alloc memory for cluster_command");
        return ESP_ERR_NO_MEM;
    }

    return cmd->send_command();
}

} // namespace controller
} // namespace esp_matter
