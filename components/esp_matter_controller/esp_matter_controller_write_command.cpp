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

#include <esp_matter_commissioner.h>
#include <esp_matter_controller_write_command.h>

using namespace chip::app::Clusters;
using chip::DeviceProxy;

static const char *TAG = "write_command";

namespace esp_matter {
namespace controller {

template <class T>
void write_command<T>::on_device_connected_fcn(void *context, ExchangeManager &exchangeMgr,
                                               SessionHandle &sessionHandle)
{
    write_command<T> *cmd = (write_command<T> *)context;
    CHIP_ERROR err = CHIP_NO_ERROR;
    WriteClient *write_client =
        chip::Platform::New<WriteClient>(&exchangeMgr, &(cmd->get_chunked_write_callback()), chip::NullOptional, false);
    if (!write_client) {
        ESP_LOGE(TAG, "Failed to alloc memory for WriteClient");
        chip::Platform::Delete(cmd);
    }
    err = write_client->EncodeAttribute(cmd->get_attribute_path(), cmd->get_attribute_val(), chip::NullOptional);
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to Encode Attribute for WriteClient");
        chip::Platform::Delete(cmd);
        chip::Platform::Delete(write_client);
        return;
    }

    err = write_client->SendWriteRequest(sessionHandle);
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to Send Write Request");
        chip::Platform::Delete(cmd);
        chip::Platform::Delete(write_client);
    }
    return;
}

template <class T>
void write_command<T>::on_device_connection_failure_fcn(void *context, const ScopedNodeId &peerId, CHIP_ERROR error)
{
    write_command<T> *cmd = (write_command<T> *)context;
    delete cmd;
    return;
}

template <class T>
esp_err_t write_command<T>::send_command()
{
    if (CHIP_NO_ERROR ==
        commissioner::get_device_commissioner()->GetConnectedDevice(m_node_id, &on_device_connected_cb,
                                                                    &on_device_connection_failure_cb)) {
        return ESP_OK;
    }
    chip::Platform::Delete(this);
    return ESP_FAIL;
}

namespace clusters {
namespace on_off {

static esp_err_t write_attribute(uint64_t node_id, uint16_t endpoint_id, uint32_t attribute_id, char *attribute_val_str)
{
    esp_err_t err = ESP_OK;
    switch (attribute_id) {
    case OnOff::Attributes::OnTime::Id:
    case OnOff::Attributes::OffWaitTime::Id: {
        write_command<uint16_t> *cmd = chip::Platform::New<write_command<uint16_t>>(
            node_id, endpoint_id, OnOff::Id, attribute_id, (uint16_t)strtol(attribute_val_str, NULL, 10));
        if (!cmd) {
            ESP_LOGE(TAG, "Failed to alloc memory for write_command");
            return ESP_ERR_NO_MEM;
        }
        return cmd->send_command();
        break;
    }
    case OnOff::Attributes::StartUpOnOff::Id: {
        write_command<uint8_t> *cmd = chip::Platform::New<write_command<uint8_t>>(
            node_id, endpoint_id, OnOff::Id, attribute_id, (uint8_t)strtol(attribute_val_str, NULL, 10));
        if (!cmd) {
            ESP_LOGE(TAG, "Failed to alloc memory for write_command");
            return ESP_ERR_NO_MEM;
        }
        return cmd->send_command();
        break;
    }
    default:
        err = ESP_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

} // namespace on_off

namespace level_control {

static esp_err_t write_attribute(uint64_t node_id, uint16_t endpoint_id, uint32_t attribute_id, char *attribute_val_str)
{
    esp_err_t err = ESP_OK;
    switch (attribute_id) {
    case LevelControl::Attributes::OnOffTransitionTime::Id:
    case LevelControl::Attributes::OnTransitionTime::Id:
    case LevelControl::Attributes::OffTransitionTime::Id: {
        write_command<uint16_t> *cmd = chip::Platform::New<write_command<uint16_t>>(
            node_id, endpoint_id, LevelControl::Id, attribute_id, (uint16_t)strtol(attribute_val_str, NULL, 10));
        if (!cmd) {
            ESP_LOGE(TAG, "Failed to alloc memory for write_command");
            return ESP_ERR_NO_MEM;
        }
        return cmd->send_command();
        break;
    }
    case LevelControl::Attributes::OnLevel::Id:
    case LevelControl::Attributes::DefaultMoveRate::Id:
    case LevelControl::Attributes::Options::Id:
    case LevelControl::Attributes::StartUpCurrentLevel::Id: {
        write_command<uint8_t> *cmd = chip::Platform::New<write_command<uint8_t>>(
            node_id, endpoint_id, LevelControl::Id, attribute_id, (uint8_t)strtol(attribute_val_str, NULL, 10));
        if (!cmd) {
            ESP_LOGE(TAG, "Failed to alloc memory for write_command");
            return ESP_ERR_NO_MEM;
        }
        return cmd->send_command();
        break;
    }
    default:
        err = ESP_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

} // namespace level_control

namespace color_control {

static esp_err_t write_attribute(uint64_t node_id, uint16_t endpoint_id, uint32_t attribute_id, char *attribute_val_str)
{
    esp_err_t err = ESP_OK;
    switch (attribute_id) {
    case ColorControl::Attributes::StartUpColorTemperatureMireds::Id: {
        write_command<uint16_t> *cmd = chip::Platform::New<write_command<uint16_t>>(
            node_id, endpoint_id, ColorControl::Id, attribute_id, (uint16_t)strtol(attribute_val_str, NULL, 10));
        if (!cmd) {
            ESP_LOGE(TAG, "Failed to alloc memory for write_command");
            return ESP_ERR_NO_MEM;
        }
        return cmd->send_command();
        break;
    }
    case ColorControl::Attributes::Options::Id: {
        write_command<uint8_t> *cmd = chip::Platform::New<write_command<uint8_t>>(
            node_id, endpoint_id, ColorControl::Id, attribute_id, (uint8_t)strtol(attribute_val_str, NULL, 10));
        if (!cmd) {
            ESP_LOGE(TAG, "Failed to alloc memory for write_command");
            return ESP_ERR_NO_MEM;
        }
        return cmd->send_command();
        break;
    }
    default:
        err = ESP_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}
} // namespace color_control

} // namespace clusters

esp_err_t send_write_attr_command(uint64_t node_id, uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                                  char *attribute_val_str)
{
    switch (cluster_id) {
    case OnOff::Id:
        return clusters::on_off::write_attribute(node_id, endpoint_id, attribute_id, attribute_val_str);
        break;
    case LevelControl::Id:
        return clusters::level_control::write_attribute(node_id, endpoint_id, attribute_id, attribute_val_str);
        break;
    case ColorControl::Id:
        return clusters::color_control::write_attribute(node_id, endpoint_id, attribute_id, attribute_val_str);
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

} // namespace controller
} // namespace esp_matter
