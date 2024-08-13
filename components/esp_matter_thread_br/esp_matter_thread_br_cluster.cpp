// Copyright 2023 Espressif Systems (Shanghai) PTE LTD
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
#include <esp_log.h>
#include <esp_matter_thread_br_cluster.h>
#include <esp_matter_thread_br_launcher.h>

#include <app/util/attribute-storage.h>
#include <app/AttributeValueDecoder.h>
#include <app/AttributeAccessInterface.h>
#include <app/AttributeAccessInterfaceRegistry.h>

#define TAG "thread_br_custom_cluster"

namespace esp_matter {

static int hex_digit_to_int(char hex)
{
    if ('A' <= hex && hex <= 'F') {
        return 10 + hex - 'A';
    } else if ('a' <= hex && hex <= 'f') {
        return 10 + hex - 'a';
    } else if ('0' <= hex && hex <= '9') {
        return hex - '0';
    }
    return -1;
}

static size_t hex_str_to_binary(const char *str, size_t str_len, uint8_t *buf, uint8_t buf_size)
{
    if (str_len % 2 != 0 || str_len / 2 > buf_size) {
        return 0;
    }
    for (size_t index = 0; index < str_len / 2; ++index) {
        int byte_h = hex_digit_to_int(str[2 * index]);
        int byte_l = hex_digit_to_int(str[2 * index + 1]);
        if (byte_h < 0 || byte_l < 0) {
            return 0;
        }
        buf[index] = (byte_h << 4) + byte_l;
    }
    return str_len / 2;
}

namespace cluster {
namespace thread_br {

namespace attribute {

attribute_t *create_dataset_tlvs(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, dataset_tlvs::Id, ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_octet_str(value, length));
}

attribute_t *create_role(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, role::Id, ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_border_agent_id(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, border_agent_id::Id, ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_octet_str(value, length));
}

} // namespace attribute

namespace command {

static esp_err_t configure_dataset_tlvs_command_callback(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                         void *opaque_ptr)
{
    uint32_t cluster_id = command_path.mClusterId;
    uint32_t command_id = command_path.mCommandId;

    if (cluster_id != cluster::thread_br::Id || command_id != cluster::thread_br::command::configure_dataset_tlvs::Id) {
        ESP_LOGE(TAG, "Got thread_br command callback for some other command. This should not happen.");
        return ESP_FAIL;
    }

    chip::CharSpan dataset_tlvs_str;
    chip::TLV::TLVType outer;
    ESP_RETURN_ON_FALSE(tlv_data.GetType() == chip::TLV::kTLVType_Structure, ESP_FAIL, TAG,
                        "TLV data is not a structure");
    ESP_RETURN_ON_FALSE(tlv_data.EnterContainer(outer) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to enter container");
    while (tlv_data.Next() == CHIP_NO_ERROR) {
        if (!chip::TLV::IsContextTag(tlv_data.GetTag())) {
            continue;
        }
        if (chip::TLV::TagNumFromTag(tlv_data.GetTag()) == 0 && tlv_data.GetType() == chip::TLV::kTLVType_UTF8String) {
            chip::app::DataModel::Decode(tlv_data, dataset_tlvs_str);
        }
    }
    ESP_RETURN_ON_FALSE(tlv_data.ExitContainer(outer) == CHIP_NO_ERROR, ESP_FAIL, TAG, "Failed to exit container");
    const char *data = dataset_tlvs_str.data();
    size_t size = dataset_tlvs_str.size();
    otOperationalDatasetTlvs dataset_tlvs;
    dataset_tlvs.mLength = hex_str_to_binary(data, size, dataset_tlvs.mTlvs, sizeof(dataset_tlvs.mTlvs));
    ESP_RETURN_ON_FALSE(dataset_tlvs.mLength > 0, ESP_ERR_INVALID_ARG, TAG, "Failed to parse dataset tlvs");
    ESP_RETURN_ON_ERROR(set_thread_dataset_tlvs(&dataset_tlvs), TAG, "Failed to set Thread DatasetTlvs");

    return ESP_OK;
}

static esp_err_t start_thread_command_callback(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                               void *opaque_ptr)
{
    uint32_t cluster_id = command_path.mClusterId;
    uint32_t command_id = command_path.mCommandId;

    if (cluster_id != cluster::thread_br::Id || command_id != cluster::thread_br::command::start_thread::Id) {
        ESP_LOGE(TAG, "Got thread_br command callback for some other command. This should not happen.");
        return ESP_FAIL;
    }

    return set_thread_enabled(true);
}

static esp_err_t stop_thread_command_callback(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                              void *opaque_ptr)
{
    uint32_t cluster_id = command_path.mClusterId;
    uint32_t command_id = command_path.mCommandId;

    if (cluster_id != cluster::thread_br::Id || command_id != cluster::thread_br::command::stop_thread::Id) {
        ESP_LOGE(TAG, "Got thread_br command callback for some other command. This should not happen.");
        return ESP_FAIL;
    }

    return set_thread_enabled(false);
}

command_t *create_configure_dataset_tlvs(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, configure_dataset_tlvs::Id, COMMAND_FLAG_ACCEPTED | COMMAND_FLAG_CUSTOM,
                                       configure_dataset_tlvs_command_callback);
}

command_t *create_start_thread(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, start_thread::Id, COMMAND_FLAG_ACCEPTED | COMMAND_FLAG_CUSTOM,
                                       start_thread_command_callback);
}

command_t *create_stop_thread(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, stop_thread::Id, COMMAND_FLAG_ACCEPTED | COMMAND_FLAG_CUSTOM,
                                       stop_thread_command_callback);
}

} // namespace command

using chip::app::AttributeAccessInterface;
using chip::app::AttributeValueDecoder;
using chip::app::AttributeValueEncoder;
using chip::app::ConcreteDataAttributePath;
using chip::app::ConcreteReadAttributePath;

class ThreadBRAttrAccess : public AttributeAccessInterface {
public:
    ThreadBRAttrAccess()
        : AttributeAccessInterface(chip::Optional<chip::EndpointId>::Missing(), cluster::thread_br::Id)
    {
    }

    CHIP_ERROR Read(const ConcreteReadAttributePath &aPath, AttributeValueEncoder &aEncoder) override
    {
        if (aPath.mClusterId != cluster::thread_br::Id) {
            return CHIP_ERROR_INVALID_ARGUMENT;
        }
        esp_err_t err = ESP_OK;
        if (aPath.mAttributeId == cluster::thread_br::attribute::dataset_tlvs::Id) {
            otOperationalDatasetTlvs dataset_tlvs;
            err = get_thread_dataset_tlvs(&dataset_tlvs);
            if (err != ESP_OK) {
                return CHIP_ERROR_INTERNAL;
            }
            return aEncoder.Encode(chip::ByteSpan(dataset_tlvs.mTlvs, dataset_tlvs.mLength));
        } else if (aPath.mAttributeId == cluster::thread_br::attribute::role::Id) {
            uint8_t role = get_thread_role();
            return aEncoder.Encode(role);
        } else if (aPath.mAttributeId == cluster::thread_br::attribute::border_agent_id::Id) {
            otBorderAgentId border_agent_id;
            err = get_border_agent_id(&border_agent_id);
            if (err != ESP_OK) {
                return CHIP_ERROR_INTERNAL;
            }
            return aEncoder.Encode(chip::ByteSpan(border_agent_id.mId, sizeof(border_agent_id.mId)));
        }
        return CHIP_NO_ERROR;
    }

    CHIP_ERROR Write(const ConcreteDataAttributePath &aPath, AttributeValueDecoder &aDecoder) override
    {
        return CHIP_NO_ERROR;
    }
};

ThreadBRAttrAccess g_attr_access;

void thread_br_cluster_plugin_server_init_callback()
{
    registerAttributeAccessOverride(&g_attr_access);
}

const function_generic_t function_list[] = {};
const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, Id, CLUSTER_FLAG_SERVER);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    set_plugin_server_init_callback(cluster, thread_br_cluster_plugin_server_init_callback);
    add_function_list(cluster, function_list, function_flags);

    global::attribute::create_cluster_revision(cluster, 1);
    global::attribute::create_feature_map(cluster, 0);

    // Attribute managed internally
    attribute::create_dataset_tlvs(cluster, NULL, 0);
    attribute::create_role(cluster, 0);
    attribute::create_border_agent_id(cluster, NULL, 0);

    command::create_configure_dataset_tlvs(cluster);
    command::create_start_thread(cluster);
    command::create_stop_thread(cluster);

    return cluster;
}

} // namespace thread_br
} // namespace cluster

} // namespace esp_matter
