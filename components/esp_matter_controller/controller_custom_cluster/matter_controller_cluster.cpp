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
#include <esp_matter_controller_utils.h>
#include <matter_controller_cluster.h>
#include <matter_controller_device_mgr.h>
#include <nvs_flash.h>

#include <app/ConcreteCommandPath.h>
#include <app/server/Server.h>
#include <app/util/attribute-storage.h>
#include <lib/core/TLVReader.h>
#include <lib/support/Span.h>
#include <lib/support/TypeTraits.h>
#include <stdint.h>
#include <string.h>

#include <rmaker_rest_apis.h>

#define TAG "controller_custom_cluster"

using chip::ByteSpan;
using chip::MutableByteSpan;
using chip::to_underlying;
using chip::app::ConcreteCommandPath;
using chip::Platform::ScopedMemoryBufferWithSize;
using chip::TLV::TLVReader;
using namespace chip::DeviceLayer;
using chip::System::Clock::Seconds32;

constexpr char *controller_namespace = "controller";

namespace esp_matter {
namespace cluster {
namespace matter_controller {

namespace attribute {

namespace refresh_token {

static esp_err_t update(uint16_t endpoint_id, const char *refresh_token)
{
    ESP_RETURN_ON_FALSE(refresh_token, ESP_ERR_INVALID_ARG, TAG, "refresh_token cannot be NULL");
    nvs_handle_t handle;
    esp_err_t err =
        nvs_open_from_partition(CONFIG_ESP_MATTER_NVS_PART_NAME, controller_namespace, NVS_READWRITE, &handle);
    ESP_RETURN_ON_ERROR(err, TAG, "Failed to open controller namespace");
    char nvs_key[16];
    snprintf(nvs_key, 16, "rf_tk/%x", endpoint_id);
    if ((err = nvs_set_str(handle, nvs_key, refresh_token)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set refresh_token");
        nvs_close(handle);
        return err;
    }
    if ((err = nvs_commit(handle)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit nvs");
    }
    nvs_close(handle);
    return err;
}

esp_err_t get(uint16_t endpoint_id, char *refresh_token)
{
    ESP_RETURN_ON_FALSE(refresh_token, ESP_ERR_INVALID_ARG, TAG, "refresh_token cannot be NULL");
    nvs_handle_t handle;
    esp_err_t err =
        nvs_open_from_partition(CONFIG_ESP_MATTER_NVS_PART_NAME, controller_namespace, NVS_READONLY, &handle);
    ESP_RETURN_ON_ERROR(err, TAG, "Failed to open controller namespace");
    char nvs_key[16];
    snprintf(nvs_key, 16, "rf_tk/%x", endpoint_id);
    size_t refresh_token_len = ESP_MATTER_RAINMAKER_MAX_REFRESH_TOKEN_LEN;
    err = nvs_get_str(handle, nvs_key, refresh_token, &refresh_token_len);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        refresh_token_len = 0;
        err = ESP_OK;
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get refresh_token");
    }
    refresh_token[refresh_token_len] = 0;
    nvs_close(handle);
    return err;
}

attribute_t *create(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, refresh_token::Id, ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_long_char_str(value, length));
}

} // namespace refresh_token

namespace refresh_token_verified {

static esp_err_t update(uint16_t endpoint_id, bool refresh_token_verified)
{
    esp_matter_attr_val_t val = esp_matter_bool(refresh_token_verified);
    uint32_t cluster_id = matter_controller::Id;
    uint32_t attribute_id = refresh_token_verified::Id;
    return esp_matter::attribute::update(endpoint_id, cluster_id, attribute_id, &val);
}

esp_err_t get(uint16_t endpoint_id, bool &refresh_token_verified)
{
    node_t *node = node::get();
    endpoint_t *endpoint = endpoint::get(node, endpoint_id);
    cluster_t *cluster = cluster::get(endpoint, matter_controller::Id);
    attribute_t *attribute = esp_matter::attribute::get(cluster, refresh_token_verified::Id);
    ESP_RETURN_ON_FALSE(attribute, ESP_FAIL, TAG, "Could not find refresh_token_verified attribue");
    esp_matter_attr_val_t raw_val = esp_matter_invalid(NULL);
    esp_matter::attribute::get_val(attribute, &raw_val);
    ESP_RETURN_ON_FALSE(raw_val.type == ESP_MATTER_VAL_TYPE_BOOLEAN, ESP_FAIL, TAG, "Invalid Attribute type");
    refresh_token_verified = raw_val.val.b;
    return ESP_OK;
}

attribute_t *create(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, refresh_token_verified::Id, ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_bool(value));
}

} // namespace refresh_token_verified

namespace authorized {

esp_err_t update(uint16_t endpoint_id, bool authorized)
{
    esp_matter_attr_val_t val = esp_matter_bool(authorized);
    uint32_t cluster_id = matter_controller::Id;
    uint32_t attribute_id = authorized::Id;
    return esp_matter::attribute::update(endpoint_id, cluster_id, attribute_id, &val);
}

esp_err_t get(uint16_t endpoint_id, bool &authorized)
{
    node_t *node = node::get();
    endpoint_t *endpoint = endpoint::get(node, endpoint_id);
    cluster_t *cluster = cluster::get(endpoint, matter_controller::Id);
    attribute_t *attribute = esp_matter::attribute::get(cluster, authorized::Id);
    ESP_RETURN_ON_FALSE(attribute, ESP_FAIL, TAG, "Could not find authorized attribue");
    esp_matter_attr_val_t raw_val = esp_matter_invalid(NULL);
    esp_matter::attribute::get_val(attribute, &raw_val);
    ESP_RETURN_ON_FALSE(raw_val.type == ESP_MATTER_VAL_TYPE_BOOLEAN, ESP_FAIL, TAG, "Invalid Attribute type");
    authorized = raw_val.val.b;
    return ESP_OK;
}

attribute_t *create(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, authorized::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_bool(value));
}

} // namespace authorized

namespace user_noc_installed {

static esp_err_t update(uint16_t endpoint_id, bool user_noc_installed)
{
    esp_matter_attr_val_t val = esp_matter_bool(user_noc_installed);
    uint32_t cluster_id = matter_controller::Id;
    uint32_t attribute_id = user_noc_installed::Id;
    return esp_matter::attribute::update(endpoint_id, cluster_id, attribute_id, &val);
}

esp_err_t get(uint16_t endpoint_id, bool &user_noc_installed)
{
    node_t *node = node::get();
    endpoint_t *endpoint = endpoint::get(node, endpoint_id);
    cluster_t *cluster = cluster::get(endpoint, matter_controller::Id);
    attribute_t *attribute = esp_matter::attribute::get(cluster, user_noc_installed::Id);
    ESP_RETURN_ON_FALSE(attribute, ESP_FAIL, TAG, "Could not find user_noc_installed attribue");
    esp_matter_attr_val_t raw_val = esp_matter_invalid(NULL);
    esp_matter::attribute::get_val(attribute, &raw_val);
    ESP_RETURN_ON_FALSE(raw_val.type == ESP_MATTER_VAL_TYPE_BOOLEAN, ESP_FAIL, TAG, "Invalid Attribute type");
    user_noc_installed = raw_val.val.b;
    return ESP_OK;
}

attribute_t *create(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, user_noc_installed::Id, ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_bool(value));
}

} // namespace user_noc_installed

namespace endpoint_url {

static esp_err_t update(uint16_t endpoint_id, const char *endpoint_url)
{
    ESP_RETURN_ON_FALSE(endpoint_url, ESP_ERR_INVALID_ARG, TAG, "endpoint_url cannot be NULL");
    nvs_handle_t handle;
    esp_err_t err =
        nvs_open_from_partition(CONFIG_ESP_MATTER_NVS_PART_NAME, controller_namespace, NVS_READWRITE, &handle);
    ESP_RETURN_ON_ERROR(err, TAG, "Failed to open controller namespace");
    char nvs_key[16];
    snprintf(nvs_key, 16, "ep_url/%x", endpoint_id);
    if ((err = nvs_set_str(handle, nvs_key, endpoint_url)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set endpoint_url");
        nvs_close(handle);
        return err;
    }
    if ((err = nvs_commit(handle)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit nvs");
    }
    nvs_close(handle);
    return err;
}

esp_err_t get(uint16_t endpoint_id, char *endpoint_url)
{
    ESP_RETURN_ON_FALSE(endpoint_url, ESP_ERR_INVALID_ARG, TAG, "endpoint_url cannot be NULL");
    nvs_handle_t handle;
    esp_err_t err =
        nvs_open_from_partition(CONFIG_ESP_MATTER_NVS_PART_NAME, controller_namespace, NVS_READONLY, &handle);
    ESP_RETURN_ON_ERROR(err, TAG, "Failed to open controller namespace");
    char nvs_key[16];
    snprintf(nvs_key, 16, "ep_url/%x", endpoint_id);
    size_t endpoint_url_len = ESP_MATTER_RAINMAKER_MAX_ENDPOINT_URL_LEN;
    if ((err = nvs_get_str(handle, nvs_key, endpoint_url, &endpoint_url_len)) != ESP_OK) {
        endpoint_url_len = 0;
        ESP_LOGE(TAG, "Failed to get endpoint_url");
    }
    nvs_close(handle);
    endpoint_url[endpoint_url_len] = 0;
    return ESP_OK;
}

attribute_t *create(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, endpoint_url::Id, ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_char_str(value, length));
}

} // namespace endpoint_url

namespace rainmaker_group_id {

static esp_err_t update(uint16_t endpoint_id, const char *group_id)
{
    ESP_RETURN_ON_FALSE(group_id, ESP_ERR_INVALID_ARG, TAG, "group_id cannot be NULL");
    nvs_handle_t handle;
    esp_err_t err =
        nvs_open_from_partition(CONFIG_ESP_MATTER_NVS_PART_NAME, controller_namespace, NVS_READWRITE, &handle);
    ESP_RETURN_ON_ERROR(err, TAG, "Failed to open controller namespace");
    char nvs_key[16];
    snprintf(nvs_key, 16, "rmk_gid/%x", endpoint_id);
    if ((err = nvs_set_str(handle, nvs_key, group_id)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set rmaker group_id");
        nvs_close(handle);
        return err;
    }
    if ((err = nvs_commit(handle)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit nvs");
    }
    nvs_close(handle);
    return err;
}

esp_err_t get(uint16_t endpoint_id, char *group_id)
{
    ESP_RETURN_ON_FALSE(group_id, ESP_ERR_INVALID_ARG, TAG, "endpoint_url cannot be NULL");
    nvs_handle_t handle;
    esp_err_t err =
        nvs_open_from_partition(CONFIG_ESP_MATTER_NVS_PART_NAME, controller_namespace, NVS_READONLY, &handle);
    ESP_RETURN_ON_ERROR(err, TAG, "Failed to open controller namespace");
    char nvs_key[16];
    snprintf(nvs_key, 16, "rmk_gid/%x", endpoint_id);
    size_t group_id_len = ESP_MATTER_RAINMAKER_MAX_GROUP_ID_LEN;
    if ((err = nvs_get_str(handle, nvs_key, group_id, &group_id_len)) != ESP_OK) {
        group_id_len = 0;
        ESP_LOGE(TAG, "Failed to get rmaker group_id");
    }
    nvs_close(handle);
    group_id[group_id_len] = 0;
    return ESP_OK;
}

attribute_t *create(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, rainmaker_group_id::Id, ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_char_str(value, length));
}

} // namespace rainmaker_group_id

namespace user_noc_fabric_index {

static esp_err_t update(uint16_t endpoint_id, uint8_t fabric_index)
{
    esp_matter_attr_val_t val = esp_matter_uint8(fabric_index);
    uint32_t cluster_id = matter_controller::Id;
    uint32_t attribute_id = user_noc_fabric_index::Id;
    return esp_matter::attribute::update(endpoint_id, cluster_id, attribute_id, &val);
}

esp_err_t get(uint16_t endpoint_id, uint8_t &fabric_index)
{
    node_t *node = node::get();
    endpoint_t *endpoint = endpoint::get(node, endpoint_id);
    cluster_t *cluster = cluster::get(endpoint, matter_controller::Id);
    attribute_t *attribute = esp_matter::attribute::get(cluster, user_noc_fabric_index::Id);
    ESP_RETURN_ON_FALSE(attribute, ESP_FAIL, TAG, "Could not find group_id attribue");
    esp_matter_attr_val_t raw_val = esp_matter_invalid(NULL);
    esp_matter::attribute::get_val(attribute, &raw_val);
    ESP_RETURN_ON_FALSE(raw_val.type == ESP_MATTER_VAL_TYPE_UINT8, ESP_FAIL, TAG, "Invalid Attribute type");
    fabric_index = raw_val.val.u8;
    return ESP_OK;
}

attribute_t *create(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, user_noc_fabric_index::Id, ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_uint8(value));
}

} // namespace user_noc_fabric_index

} // namespace attribute

namespace command {

esp_err_t parse_string_from_tlv(TLVReader &tlv_data, ScopedMemoryBufferWithSize<char> &str)
{
    chip::TLV::TLVType outer;
    chip::CharSpan str_span;
    if (chip::TLV::kTLVType_Structure != tlv_data.GetType()) {
        return ESP_FAIL;
    }
    if (tlv_data.EnterContainer(outer) != CHIP_NO_ERROR) {
        return ESP_FAIL;
    }
    while (tlv_data.Next() == CHIP_NO_ERROR) {
        if (!chip::TLV::IsContextTag(tlv_data.GetTag())) {
            continue;
        }
        if (chip::TLV::TagNumFromTag(tlv_data.GetTag()) == 0) {
            chip::app::DataModel::Decode(tlv_data, str_span);
        }
    }
    tlv_data.ExitContainer(outer);
    ESP_RETURN_ON_FALSE(str_span.data() && str_span.size() > 0, ESP_FAIL, TAG, "Failed to decode the tlv_data");
    strncpy(str.Get(), str_span.data(), str_span.size());
    str[str_span.size()] = 0;
    return ESP_OK;
}

static esp_err_t append_refresh_token_command_callback(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                       void *opaque_ptr)
{
    uint16_t endpoint_id = command_path.mEndpointId;
    uint32_t cluster_id = command_path.mClusterId;
    uint32_t command_id = command_path.mCommandId;

    // Return if this is not the controller authorize command
    if (cluster_id != matter_controller::Id || command_id != matter_controller::command::append_refresh_token::Id) {
        ESP_LOGE(TAG, "Got matter_controller command callback for some other command. This should not happen.");
        return ESP_FAIL;
    }

    ScopedMemoryBufferWithSize<char> append_str;
    append_str.Calloc(1025);
    ESP_RETURN_ON_FALSE(append_str.Get(), ESP_ERR_NO_MEM, TAG, "Failed to allocate memory for append_str");
    ESP_RETURN_ON_ERROR(parse_string_from_tlv(tlv_data, append_str), TAG,
                        "Failed to parse appended_refresh_token from tlv_data");

    ScopedMemoryBufferWithSize<char> refresh_token;
    refresh_token.Calloc(ESP_MATTER_RAINMAKER_MAX_REFRESH_TOKEN_LEN);
    ESP_RETURN_ON_FALSE(refresh_token.Get(), ESP_ERR_NO_MEM, TAG, "Failed to allocate memory for refresh_token");
    ESP_RETURN_ON_ERROR(attribute::refresh_token::get(endpoint_id, refresh_token.Get()), TAG,
                        "Failed to get refresh_token");

    size_t current_len = strnlen(refresh_token.Get(), refresh_token.AllocatedSize() - 1);
    size_t append_len = strnlen(append_str.Get(), 1024);
    strncpy(&refresh_token[current_len], append_str.Get(), append_len);
    refresh_token[current_len + append_len] = 0;
    ESP_RETURN_ON_ERROR(attribute::refresh_token::update(endpoint_id, refresh_token.Get()), TAG,
                        "Failed to update refresh_token");
    return ESP_OK;
}

static esp_err_t reset_refresh_token_command_callback(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                      void *opaque_ptr)
{
    uint16_t endpoint_id = command_path.mEndpointId;
    uint32_t cluster_id = command_path.mClusterId;
    uint32_t command_id = command_path.mCommandId;

    // Return if this is not the controller authorize command
    if (cluster_id != matter_controller::Id || command_id != matter_controller::command::reset_refresh_token::Id) {
        ESP_LOGE(TAG, "Got matter_controller command callback for some other command. This should not happen.");
        return ESP_FAIL;
    }
    const char *empty_str = "";
    attribute::refresh_token::update(endpoint_id, empty_str);
    attribute::refresh_token_verified::update(endpoint_id, false);
    return ESP_OK;
}

static esp_err_t authorize_command_callback(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                            void *opaque_ptr)
{
    uint16_t endpoint_id = command_path.mEndpointId;
    uint32_t cluster_id = command_path.mClusterId;
    uint32_t command_id = command_path.mCommandId;
    chip::app::CommandHandler *command_obj = (chip::app::CommandHandler *)opaque_ptr;

    // Return if this is not the controller authorize command
    if (cluster_id != matter_controller::Id || command_id != matter_controller::command::authorize::Id) {
        ESP_LOGE(TAG, "Got matter_controller command callback for some other command. This should not happen.");
        return ESP_FAIL;
    }
    ScopedMemoryBufferWithSize<char> endpoint_url;

    // Alloc memory for ScopedMemoryBuffers
    endpoint_url.Calloc(ESP_MATTER_RAINMAKER_MAX_ENDPOINT_URL_LEN);
    ESP_RETURN_ON_FALSE(endpoint_url.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for endpoint_url");

    // Parse the tlv data
    ESP_RETURN_ON_ERROR(parse_string_from_tlv(tlv_data, endpoint_url), TAG,
                        "Failed to parse authorize command tlv data");
    // Flush acks before really slow work
    command_obj->FlushAcksRightAwayOnSlowCommand();
    // Update the endpoint URL
    ESP_RETURN_ON_ERROR(attribute::endpoint_url::update(endpoint_id, endpoint_url.Get()), TAG,
                        "Failed to update endpoint_url");

    ESP_RETURN_ON_ERROR(controller::controller_authorize(endpoint_id), TAG, "Failed to authorize the controller");

    return ESP_OK;
}

static esp_err_t install_user_noc(uint16_t endpoint_id, uint8_t fabric_index)
{
    auto &fabric_table = chip::Server::GetInstance().GetFabricTable();
    const chip::FabricInfo *fabric_info = fabric_table.FindFabricWithIndex(fabric_index);
    uint64_t fabric_id = fabric_info->GetFabricId();
    uint8_t csr_der_buf[chip::Crypto::kMIN_CSR_Buffer_Size];
    MutableByteSpan csr_span(csr_der_buf);
    ScopedMemoryBufferWithSize<char> rainmaker_group_id;
    ScopedMemoryBufferWithSize<char> endpoint_url;
    ScopedMemoryBufferWithSize<unsigned char> noc_der;
    ScopedMemoryBufferWithSize<unsigned char> noc_matter_cert;
    MutableByteSpan matter_cert_noc;
    size_t noc_der_len;
    const char *access_token = controller::get_current_access_token();
    esp_err_t err = ESP_OK;

    // Alloc memory for ScopedMemoryBuffers
    rainmaker_group_id.Calloc(ESP_MATTER_RAINMAKER_MAX_GROUP_ID_LEN);
    ESP_RETURN_ON_FALSE(rainmaker_group_id.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for rainmaker_group_id");
    endpoint_url.Calloc(ESP_MATTER_RAINMAKER_MAX_ENDPOINT_URL_LEN);
    ESP_RETURN_ON_FALSE(endpoint_url.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for endpoint_url");
    ESP_RETURN_ON_ERROR(attribute::endpoint_url::get(endpoint_id, endpoint_url.Get()), TAG,
                        "Failed to get endpoint_url");
    noc_der.Calloc(chip::Credentials::kMaxDERCertLength);
    ESP_RETURN_ON_FALSE(noc_der.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for noc_der");
    noc_der_len = noc_der.AllocatedSize();
    noc_matter_cert.Calloc(chip::Credentials::kMaxCHIPCertLength);
    ESP_RETURN_ON_FALSE(noc_matter_cert.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for noc_matter_cert");
    matter_cert_noc = MutableByteSpan(noc_matter_cert.Get(), noc_matter_cert.AllocatedSize());

    // Fetch ther rainmaker_group_id
    err = fetch_rainmaker_group_id(endpoint_url.Get(), access_token, fabric_id, rainmaker_group_id.Get(),
                                   rainmaker_group_id.AllocatedSize());
    if (err == ESP_ERR_TIMEOUT) {
        attribute::authorized::update(endpoint_id, false);
    }
    ESP_RETURN_ON_ERROR(err, TAG, "Failed to fetch rainmaker_group_id");
    // Update the rainmaker_group_id
    ESP_RETURN_ON_ERROR(attribute::rainmaker_group_id::update(endpoint_id, rainmaker_group_id.Get()), TAG,
                        "Failed to update rainmaker_group_id");
    // Allocate Pending CSR
    chip::Server::GetInstance().GetFabricTable().AllocatePendingOperationalKey(chip::MakeOptional(fabric_index),
                                                                               csr_span);
    // Issue Controller NOC
    uint64_t matter_node_id = fabric_info->GetNodeId();
    err = issue_noc_with_csr(endpoint_url.Get(), access_token, CSR_TYPE_CONTROLLER, csr_span.data(), csr_span.size(),
                             rainmaker_group_id.Get(), &matter_node_id, noc_der.Get(), &noc_der_len);
    if (err == ESP_ERR_TIMEOUT) {
        attribute::authorized::update(endpoint_id, false);
    }
    ESP_RETURN_ON_ERROR(err, TAG, "Failed to issue user NOC");

    // Convert DER-Formated NOC to Matter Cert
    ESP_RETURN_ON_FALSE(chip::Credentials::ConvertX509CertToChipCert(
                            ByteSpan{reinterpret_cast<const unsigned char *>(noc_der.Get()), noc_der_len},
                            matter_cert_noc) == CHIP_NO_ERROR,
                        ESP_FAIL, TAG, "Failed to convert DER-Formated NOC to Matter Cert");

    // Update NOC
    ESP_RETURN_ON_FALSE(fabric_table.UpdatePendingFabricWithOperationalKeystore(fabric_index, matter_cert_noc,
                                                                                ByteSpan{}) == CHIP_NO_ERROR,
                        ESP_FAIL, TAG, "Failed to update the Fabric NOC");
    ESP_RETURN_ON_FALSE(fabric_table.CommitPendingFabricData() == CHIP_NO_ERROR, ESP_FAIL, TAG,
                        "Failed to commit the pending Fabric data");

    // Start DNS server to advertise the new node-id of the new NOC
    chip::app::DnssdServer::Instance().StartServer();

    // Update attribute
    ESP_RETURN_ON_ERROR(attribute::user_noc_installed::update(endpoint_id, true), TAG,
                        "Failed to update user_noc_installed");
    ESP_RETURN_ON_ERROR(attribute::user_noc_fabric_index::update(endpoint_id, fabric_index), TAG,
                        "Failed to update user_noc_fabric_index");
    // Update controller fabric index
    esp_matter::controller::set_fabric_index(fabric_index);

    return ESP_OK;
}

static esp_err_t update_user_noc_command_callback(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                  void *opaque_ptr)
{
    uint16_t endpoint_id = command_path.mEndpointId;
    uint32_t cluster_id = command_path.mClusterId;
    uint32_t command_id = command_path.mCommandId;
    chip::app::CommandHandler *command_obj = (chip::app::CommandHandler *)opaque_ptr;
    uint8_t fabric_index = command_obj->GetAccessingFabricIndex();

    // Return if this is not the controller authorize command
    if (cluster_id != matter_controller::Id || command_id != matter_controller::command::update_user_noc::Id) {
        ESP_LOGE(TAG, "Got matter_controller command callback for some other command. This should not happen.");
        return ESP_FAIL;
    }

    // Get the authorized
    bool authorized = false;
    ESP_RETURN_ON_ERROR(attribute::authorized::get(endpoint_id, authorized), TAG, "Failed to get authorized attribute");
    if (!authorized) {
        return ESP_ERR_INVALID_STATE;
    }

    // Flush acks before really slow work
    command_obj->FlushAcksRightAwayOnSlowCommand();
    // Install user NOC
    ESP_RETURN_ON_ERROR(install_user_noc(endpoint_id, fabric_index), TAG, "Failed to install user NOC");

    return ESP_OK;
}

static esp_err_t update_device_list_command_callback(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                     void *opaque_ptr)
{
    uint16_t endpoint_id = command_path.mEndpointId;
    uint32_t cluster_id = command_path.mClusterId;
    uint32_t command_id = command_path.mCommandId;
    chip::app::CommandHandler *command_obj = (chip::app::CommandHandler *)opaque_ptr;

    // Return if this is not the controller authorize command
    if (cluster_id != matter_controller::Id || command_id != matter_controller::command::update_device_list::Id) {
        ESP_LOGE(TAG, "Got matter_controller command callback for some other command. This should not happen.");
        return ESP_FAIL;
    }
    // Flush acks before really slow work
    command_obj->FlushAcksRightAwayOnSlowCommand();

    ESP_RETURN_ON_ERROR(controller::device_mgr::update_device_list(endpoint_id), TAG, "Failed to update device list");

    return ESP_OK;
}

namespace append_refresh_token {
command_t *create(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, append_refresh_token::Id, COMMAND_FLAG_ACCEPTED | COMMAND_FLAG_CUSTOM,
                                       append_refresh_token_command_callback);
}
} // namespace append_refresh_token

namespace reset_refresh_token {
command_t *create(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, reset_refresh_token::Id, COMMAND_FLAG_ACCEPTED | COMMAND_FLAG_CUSTOM,
                                       reset_refresh_token_command_callback);
}
} // namespace reset_refresh_token

namespace authorize {
command_t *create(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, authorize::Id, COMMAND_FLAG_ACCEPTED | COMMAND_FLAG_CUSTOM,
                                       authorize_command_callback);
}
} // namespace authorize

namespace update_user_noc {
command_t *create(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, update_user_noc::Id, COMMAND_FLAG_ACCEPTED | COMMAND_FLAG_CUSTOM,
                                       update_user_noc_command_callback);
}
} // namespace update_user_noc

namespace update_device_list {
command_t *create(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, update_device_list::Id, COMMAND_FLAG_ACCEPTED | COMMAND_FLAG_CUSTOM,
                                       update_device_list_command_callback);
}
} // namespace update_device_list

} // namespace command

using chip::app::AttributeAccessInterface;
using chip::app::AttributeValueDecoder;
using chip::app::AttributeValueEncoder;
using chip::app::ConcreteDataAttributePath;
using chip::app::ConcreteReadAttributePath;

class MatterControllerAttrAccess : public AttributeAccessInterface {
public:
    MatterControllerAttrAccess()
        : AttributeAccessInterface(chip::Optional<chip::EndpointId>::Missing(), cluster::matter_controller::Id)
    {
    }

    CHIP_ERROR Read(const ConcreteReadAttributePath &aPath, AttributeValueEncoder &aEncoder) override;
    CHIP_ERROR Write(const ConcreteDataAttributePath &aPath, AttributeValueDecoder &aDecoder) override;
};

static esp_err_t encode_string_on_success(esp_err_t err, AttributeValueEncoder encoder, char *str, size_t max_buf_size)
{
    ESP_RETURN_ON_ERROR(err, TAG, "error before encode string");
    ESP_RETURN_ON_FALSE(encoder.Encode(chip::CharSpan(str, strnlen(str, max_buf_size))) == CHIP_NO_ERROR, ESP_FAIL, TAG,
                        "Failed to encode string");
    return ESP_OK;
}

CHIP_ERROR MatterControllerAttrAccess::Read(const ConcreteReadAttributePath &aPath, AttributeValueEncoder &aEncoder)
{
    if (aPath.mClusterId != cluster::matter_controller::Id) {
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
    uint16_t endpoint_id = aPath.mEndpointId;
    esp_err_t err = ESP_OK;
    switch (aPath.mAttributeId) {
    case attribute::refresh_token::Id: {
        ScopedMemoryBufferWithSize<char> refresh_token;
        refresh_token.Alloc(ESP_MATTER_RAINMAKER_MAX_REFRESH_TOKEN_LEN);
        if (!refresh_token.Get()) {
            return CHIP_ERROR_NO_MEMORY;
        }
        err = attribute::refresh_token::get(endpoint_id, refresh_token.Get());
        err = encode_string_on_success(err, aEncoder, refresh_token.Get(), refresh_token.AllocatedSize());
        break;
    }
    case attribute::endpoint_url::Id: {
        ScopedMemoryBufferWithSize<char> endpoint_url;
        endpoint_url.Alloc(ESP_MATTER_RAINMAKER_MAX_ENDPOINT_URL_LEN);
        if (!endpoint_url.Get()) {
            return CHIP_ERROR_NO_MEMORY;
        }
        err = attribute::endpoint_url::get(endpoint_id, endpoint_url.Get());
        err = encode_string_on_success(err, aEncoder, endpoint_url.Get(), endpoint_url.AllocatedSize());
        break;
    }
    case attribute::rainmaker_group_id::Id: {
        ScopedMemoryBufferWithSize<char> rainmaker_group_id;
        rainmaker_group_id.Alloc(ESP_MATTER_RAINMAKER_MAX_GROUP_ID_LEN);
        if (!rainmaker_group_id.Get()) {
            return CHIP_ERROR_NO_MEMORY;
        }
        err = attribute::rainmaker_group_id::get(endpoint_id, rainmaker_group_id.Get());
        err = encode_string_on_success(err, aEncoder, rainmaker_group_id.Get(), rainmaker_group_id.AllocatedSize());
        break;
    }
    default:
        break;
    }
    if (err != ESP_OK) {
        return CHIP_ERROR_INTERNAL;
    }
    return CHIP_NO_ERROR;
}

CHIP_ERROR MatterControllerAttrAccess::Write(const ConcreteDataAttributePath &aPath, AttributeValueDecoder &aDecoder)
{
    if (aPath.mClusterId != cluster::matter_controller::Id) {
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
    return CHIP_NO_ERROR;
}

MatterControllerAttrAccess g_attr_access;

void controller_cluster_plugin_server_init_callback()
{
    registerAttributeAccessOverride(&g_attr_access);
}

cluster_t *create(endpoint_t *endpoint, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, Id, CLUSTER_FLAG_SERVER);
    if (!cluster) {
        ESP_LOGE(TAG, "Could not create cluster");
        return NULL;
    }

    set_plugin_server_init_callback(cluster, controller_cluster_plugin_server_init_callback);
    add_function_list(cluster, NULL, 0);

    global::attribute::create_cluster_revision(cluster, 2);
    global::attribute::create_feature_map(cluster, 0);
    attribute::authorized::create(cluster, false);
    attribute::user_noc_installed::create(cluster, false);
    attribute::user_noc_fabric_index::create(cluster, 0);
    // Attributes managed internally
    attribute::refresh_token::create(cluster, NULL, 0);
    attribute::refresh_token_verified::create(cluster, false);
    attribute::endpoint_url::create(cluster, NULL, 0);
    attribute::rainmaker_group_id::create(cluster, NULL, 0);

    command::append_refresh_token::create(cluster);
    command::reset_refresh_token::create(cluster);
    command::authorize::create(cluster);
    command::update_user_noc::create(cluster);
    command::update_device_list::create(cluster);

    return cluster;
}

} // namespace matter_controller
} // namespace cluster

namespace controller {

static uint16_t s_access_token_expired_timer_endpoint_id = chip::kInvalidEndpointId;
static char s_access_token[ESP_MATTER_RAINMAKER_MAX_ACCESS_TOKEN_LEN];

static void access_token_expired_callback(chip::System::Layer *systemLayer, void *appState)
{
    uint16_t endpoint_id = s_access_token_expired_timer_endpoint_id;
    if (cluster::matter_controller::attribute::authorized::update(endpoint_id, false) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to update authorized attribute");
        return;
    }
    if (controller_authorize(endpoint_id) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to do authorizing");
    }
}

esp_err_t controller_authorize(uint16_t endpoint_id)
{
    ScopedMemoryBufferWithSize<char> refresh_token;
    ScopedMemoryBufferWithSize<char> endpoint_url;

    // Alloc memory for ScopedMemoryBuffers
    refresh_token.Calloc(ESP_MATTER_RAINMAKER_MAX_REFRESH_TOKEN_LEN);
    ESP_RETURN_ON_FALSE(refresh_token.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for refresh_token");
    endpoint_url.Calloc(ESP_MATTER_RAINMAKER_MAX_ENDPOINT_URL_LEN);
    ESP_RETURN_ON_FALSE(endpoint_url.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for endpoint_url");

    ESP_RETURN_ON_ERROR(cluster::matter_controller::attribute::endpoint_url::get(endpoint_id, endpoint_url.Get()), TAG,
                        "Failed to get the endpoint_url");
    ESP_RETURN_ON_ERROR(cluster::matter_controller::attribute::refresh_token::get(endpoint_id, refresh_token.Get()),
                        TAG, "Failed to get the refresh_token");

    // Fetch the access_token
    ESP_RETURN_ON_ERROR(
        fetch_access_token(endpoint_url.Get(), refresh_token.Get(), s_access_token, sizeof(s_access_token)),
        TAG, "Failed to fetch access_token for authorizing");
    // Update the authorized attribute
    ESP_RETURN_ON_ERROR(cluster::matter_controller::attribute::authorized::update(endpoint_id, true), TAG,
                        "Failed to update authorized attribute");
    ESP_RETURN_ON_ERROR(cluster::matter_controller::attribute::refresh_token_verified::update(endpoint_id, true),
                        TAG, "Failed to update refresh_token_verified attribute");
    // The access token will be expired after one hour
    s_access_token_expired_timer_endpoint_id = endpoint_id;
    SystemLayer().CancelTimer(access_token_expired_callback, NULL);
    SystemLayer().StartTimer(Seconds32(3550), access_token_expired_callback, NULL);

    return ESP_OK;
}

const char *get_current_access_token()
{
    return s_access_token;
}

} // namespace controller
} // namespace esp_matter
