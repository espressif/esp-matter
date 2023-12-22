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
#include <esp_crt_bundle.h>
#include <esp_http_client.h>
#include <esp_matter_controller_utils.h>
#include <json_generator.h>
#include <json_parser.h>
#include <matter_controller_cluster.h>
#include <matter_controller_device_mgr.h>
#include <mbedtls/base64.h>
#include <mbedtls/pem.h>
#include <nvs_flash.h>

#include <app/ConcreteCommandPath.h>
#include <app/server/Server.h>
#include <app/util/attribute-storage.h>
#include <lib/core/TLVReader.h>
#include <lib/support/Span.h>
#include <lib/support/TypeTraits.h>
#include <string.h>

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
static esp_err_t refresh_token_attribute_update(uint16_t endpoint_id, const char *refresh_token)
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

esp_err_t refresh_token_attribute_get(uint16_t endpoint_id, char *refresh_token)
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

attribute_t *create_refresh_token(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, refresh_token::Id, ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_long_char_str(value, length));
}

static esp_err_t access_token_attribute_update(uint16_t endpoint_id, const char *access_token)
{
    ESP_RETURN_ON_FALSE(access_token, ESP_ERR_INVALID_ARG, TAG, "access_token cannot be NULL");
    nvs_handle_t handle;
    esp_err_t err =
        nvs_open_from_partition(CONFIG_ESP_MATTER_NVS_PART_NAME, controller_namespace, NVS_READWRITE, &handle);
    ESP_RETURN_ON_ERROR(err, TAG, "Failed to open controller namespace");
    char nvs_key[16];
    snprintf(nvs_key, 16, "ac_tk/%x", endpoint_id);
    if ((err = nvs_set_str(handle, nvs_key, access_token)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set access_token");
        nvs_close(handle);
        return err;
    }
    if ((err = nvs_commit(handle)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit nvs");
    }
    nvs_close(handle);
    return err;
}

esp_err_t access_token_attribute_get(uint16_t endpoint_id, char *access_token)
{
    ESP_RETURN_ON_FALSE(access_token, ESP_ERR_INVALID_ARG, TAG, "access_token cannot be NULL");
    nvs_handle_t handle;
    esp_err_t err =
        nvs_open_from_partition(CONFIG_ESP_MATTER_NVS_PART_NAME, controller_namespace, NVS_READONLY, &handle);
    ESP_RETURN_ON_ERROR(err, TAG, "Failed to open controller namespace");
    char nvs_key[16];
    snprintf(nvs_key, 16, "ac_tk/%x", endpoint_id);

    size_t access_token_len = ESP_MATTER_RAINMAKER_MAX_ACCESS_TOKEN_LEN;
    err = nvs_get_str(handle, nvs_key, access_token, &access_token_len);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        access_token_len = 0;
        err = ESP_OK;
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get access_token");
    }
    nvs_close(handle);
    access_token[access_token_len] = 0;
    return err;
}

attribute_t *create_access_token(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, access_token::Id, ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_long_char_str(value, length));
}

esp_err_t authorized_attribute_update(uint16_t endpoint_id, bool authorized)
{
    esp_matter_attr_val_t val = esp_matter_bool(authorized);
    uint32_t cluster_id = matter_controller::Id;
    uint32_t attribute_id = authorized::Id;
    return esp_matter::attribute::update(endpoint_id, cluster_id, attribute_id, &val);
}

esp_err_t authorized_attribute_get(uint16_t endpoint_id, bool &authorized)
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

attribute_t *create_authorized(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, authorized::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_bool(value));
}

static esp_err_t user_noc_installed_attribute_update(uint16_t endpoint_id, bool user_noc_installed)
{
    esp_matter_attr_val_t val = esp_matter_bool(user_noc_installed);
    uint32_t cluster_id = matter_controller::Id;
    uint32_t attribute_id = user_noc_installed::Id;
    return esp_matter::attribute::update(endpoint_id, cluster_id, attribute_id, &val);
}

esp_err_t user_noc_installed_attribute_get(uint16_t endpoint_id, bool &user_noc_installed)
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

attribute_t *create_user_noc_installed(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, user_noc_installed::Id, ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_bool(value));
}

static esp_err_t endpoint_url_attribute_update(uint16_t endpoint_id, const char *endpoint_url)
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

esp_err_t endpoint_url_attribute_get(uint16_t endpoint_id, char *endpoint_url)
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

attribute_t *create_endpoint_url(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, endpoint_url::Id, ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_char_str(value, length));
}

static esp_err_t rainmaker_group_id_attribute_update(uint16_t endpoint_id, const char *group_id)
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

esp_err_t rainmaker_group_id_attribute_get(uint16_t endpoint_id, char *group_id)
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

attribute_t *create_rainmaker_group_id(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, rainmaker_group_id::Id, ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_char_str(value, length));
}

static esp_err_t user_noc_fabric_index_attribute_update(uint16_t endpoint_id, uint8_t fabric_index)
{
    esp_matter_attr_val_t val = esp_matter_uint8(fabric_index);
    uint32_t cluster_id = matter_controller::Id;
    uint32_t attribute_id = user_noc_fabric_index::Id;
    return esp_matter::attribute::update(endpoint_id, cluster_id, attribute_id, &val);
}

esp_err_t user_noc_fabric_index_attribute_get(uint16_t endpoint_id, uint8_t &fabric_index)
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

attribute_t *create_user_noc_fabric_index(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, user_noc_fabric_index::Id, ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_uint8(value));
}

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

static esp_err_t fetch_rainmaker_group_id(uint16_t endpoint_id, ScopedMemoryBufferWithSize<char> &rainmaker_group_id,
                                          uint64_t fabric_id, ScopedMemoryBufferWithSize<char> &start_id,
                                          const int num_records, int &group_count)
{
    esp_err_t ret = ESP_OK;
    char url[256] = {0};
    esp_http_client_config_t config = {
        .url = url,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .buffer_size = 1526,
        .buffer_size_tx = 2048,
        .skip_cert_common_name_check = false,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    esp_http_client_handle_t client = NULL;
    ScopedMemoryBufferWithSize<char> endpoint_url;
    ScopedMemoryBufferWithSize<char> access_token;
    ScopedMemoryBufferWithSize<char> http_payload;
    int http_len, http_status_code;
    jparse_ctx_t jctx;
    int group_index;
    char fabric_id_str[17];
    int rainmaker_group_id_len = 0;
    int start_id_len = 0;

    ESP_RETURN_ON_FALSE(rainmaker_group_id.Get(), ESP_ERR_INVALID_ARG, TAG, "rainmaker_group_id cannot be NULL");

    endpoint_url.Calloc(ESP_MATTER_RAINMAKER_MAX_ENDPOINT_URL_LEN);
    ESP_RETURN_ON_FALSE(endpoint_url.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for endpoint_url");

    access_token.Calloc(ESP_MATTER_RAINMAKER_MAX_ACCESS_TOKEN_LEN);
    ESP_RETURN_ON_FALSE(access_token.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for access_token");

    http_payload.Calloc(1024);
    ESP_RETURN_ON_FALSE(http_payload.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for http_payload");

    ESP_RETURN_ON_ERROR(attribute::endpoint_url_attribute_get(endpoint_id, endpoint_url.Get()), TAG,
                        "Failed to get endpoint_url attribute value");
    ESP_RETURN_ON_ERROR(attribute::access_token_attribute_get(endpoint_id, access_token.Get()), TAG,
                        "Failed to get access_token attribute value");

    snprintf(url, sizeof(url), "%s/%s/%s?%s&start_id=%s&num_records=%d", endpoint_url.Get(), HTTP_API_VERSION,
             "user/node_group",
             "node_list=false&sub_groups=false&node_details=false&is_matter=true&fabric_details=false", start_id.Get(),
             num_records);
    client = esp_http_client_init(&config);
    ESP_RETURN_ON_FALSE(client, ESP_FAIL, TAG, "Failed to initialise HTTP Client.");

    ESP_GOTO_ON_ERROR(esp_http_client_set_header(client, "accept", "application/json"), cleanup, TAG,
                      "Failed to set http header accept");
    ESP_GOTO_ON_ERROR(esp_http_client_set_header(client, "Authorization", access_token.Get()), cleanup, TAG,
                      "Failed to set http header Authorization");
    ESP_GOTO_ON_ERROR(esp_http_client_set_method(client, HTTP_METHOD_GET), cleanup, TAG, "Failed to set http method");

    // HTTP GET
    ESP_GOTO_ON_ERROR(esp_http_client_open(client, 0), cleanup, TAG, "Failed to open http connection");

    // Read response
    http_len = esp_http_client_fetch_headers(client);
    http_status_code = esp_http_client_get_status_code(client);
    if ((http_len > 0) && (http_status_code == 200)) {
        http_len = esp_http_client_read_response(client, http_payload.Get(), http_payload.AllocatedSize() - 1 );
        http_payload[http_len] = 0;
    } else {
        http_len = esp_http_client_read_response(client, http_payload.Get(), http_payload.AllocatedSize() - 1);
        http_payload[http_len] = 0;
        ESP_LOGE(TAG, "Invalid response for %s", url);
        ESP_LOGE(TAG, "Status = %d, Data = %s", http_status_code, http_len > 0 ? http_payload.Get() : "None");
        ret = ESP_FAIL;
        goto close;
    }
    // Parse the response payload
    ESP_LOGI(TAG, "http response:%s", http_payload.Get());
    ESP_GOTO_ON_FALSE(json_parse_start(&jctx, http_payload.Get(), http_len) == 0, ESP_FAIL, close, TAG,
                      "Failed to parse the http response json on json_parse_start");
    if (json_obj_get_array(&jctx, "groups", &group_count) != 0) {
        ESP_LOGE(TAG, "Failed to parse the groups array from the http response");
        json_parse_end(&jctx);
        ret = ESP_FAIL;
        goto close;
    }

    for (group_index = 0; group_index < group_count; ++group_index) {
        if (json_arr_get_object(&jctx, group_index) == 0) {
            if (json_obj_get_string(&jctx, "fabric_id", fabric_id_str, sizeof(fabric_id_str)) == 0) {
                if (strtoull(fabric_id_str, NULL, 16) == fabric_id) {
                    if (json_obj_get_strlen(&jctx, "group_id", &rainmaker_group_id_len) != 0 ||
                        json_obj_get_string(&jctx, "group_id", rainmaker_group_id.Get(),
                                            rainmaker_group_id.AllocatedSize()) != 0) {
                        ESP_LOGE(TAG, "Failed to parse the group_id for fabric: 0x%llu", fabric_id);
                        ret = ESP_FAIL;
                    } else {
                        rainmaker_group_id[rainmaker_group_id_len] = 0;
                    }
                    json_arr_leave_object(&jctx);
                    break;
                }
            }
            json_arr_leave_object(&jctx);
        }
    }
    json_obj_leave_array(&jctx);
    // Get the next_id for the next fetch
    if (group_index == group_count) {
        ret = ESP_ERR_NOT_FOUND;
        if (json_obj_get_strlen(&jctx, "next_id", &start_id_len) == 0 &&
            json_obj_get_string(&jctx, "next_id", start_id.Get(), start_id.AllocatedSize()) == 0) {
            start_id[start_id_len] = 0;
        }
    }
    json_parse_end(&jctx);

close:
    esp_http_client_close(client);
cleanup:
    esp_http_client_cleanup(client);
    return ret;
}

#define PEM_BEGIN_CSR "-----BEGIN CERTIFICATE REQUEST-----\n"
#define PEM_END_CSR "-----END CERTIFICATE REQUEST-----\n"

static esp_err_t generate_user_noc_csr(ScopedMemoryBufferWithSize<char> &csr_pem, uint8_t fabric_index, size_t &out_len)
{
    uint8_t csr_der_buf[chip::Crypto::kMIN_CSR_Buffer_Size];
    MutableByteSpan csr_span(csr_der_buf);
    chip::Server::GetInstance().GetFabricTable().AllocatePendingOperationalKey(chip::MakeOptional(fabric_index),
                                                                               csr_span);
    // copy the csr_der to the end of the csr_pem buffer.
    memcpy(csr_pem.Get() + csr_pem.AllocatedSize() - csr_span.size(), csr_span.data(), csr_span.size());
    ESP_RETURN_ON_FALSE(
        mbedtls_pem_write_buffer(PEM_BEGIN_CSR, PEM_END_CSR,
                                 reinterpret_cast<uint8_t *>(csr_pem.Get() + csr_pem.AllocatedSize() - csr_span.size()),
                                 csr_span.size(), reinterpret_cast<uint8_t *>(csr_pem.Get()), csr_pem.AllocatedSize(),
                                 &out_len) == 0,
        ESP_FAIL, TAG, "Failed to generate PEM-Encoded CSR");

    return ESP_OK;
}

int convert_pem_to_der(const char *input, size_t ilen, unsigned char *output, size_t *olen)
{
    int ret;
    const char *s1, *s2, *end = input + ilen;
    size_t len = 0;

    s1 = (char *)strstr(input, "-----BEGIN");
    if (s1 == NULL) {
        return -1;
    }

    s2 = (char *)strstr((char *)input, "-----END");
    if (s2 == NULL) {
        return -1;
    }

    s1 += 10;
    while (s1 < end && *s1 != '-') {
        s1++;
    }
    while (s1 < end && *s1 == '-') {
        s1++;
    }
    if (*s1 == '\r') {
        s1++;
    }
    if (*s1 == '\n') {
        s1++;
    }

    if (s2 <= s1 || s2 > end) {
        return -1;
    }

    ret = mbedtls_base64_decode(NULL, 0, &len, (const unsigned char *)s1, s2 - s1);
    if (ret == MBEDTLS_ERR_BASE64_INVALID_CHARACTER) {
        return ret;
    }

    if (len > *olen) {
        return -1;
    }

    if ((ret = mbedtls_base64_decode(output, len, &len, (const unsigned char *)s1, s2 - s1)) != 0) {
        return ret;
    }

    *olen = len;

    return 0;
}

static void format_csr(const char *input, char *output)
{
    while (*input) {
        if (*input != '\n') {
            *output = *input;
        } else {
            *output = '\\', output++;
            *output = 'n';
        }
        output++;
        input++;
    }
    *output = 0;
}

static void format_noc(const char *input, char *output)
{
    while (*input) {
        if (*input == '\\' && *(input + 1) == 'n') {
            *output = '\n';
            input = input + 2;
        } else {
            *output = *input;
            input++;
        }
        output++;
    }
    *output = 0;
}

static esp_err_t issue_user_noc_request(ScopedMemoryBufferWithSize<char> &csr_pem,
                                        ScopedMemoryBufferWithSize<char> &rainmaker_group_id, size_t csr_pem_len,
                                        uint8_t fabric_index, uint16_t endpoint_id)
{
    esp_err_t ret = ESP_OK;
    char url[256];
    esp_http_client_config_t config = {
        .url = url,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .buffer_size = 1024,
        .buffer_size_tx = 2048,
        .skip_cert_common_name_check = false,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    esp_http_client_handle_t client = NULL;
    json_gen_str_t jstr;
    int http_len, http_status_code;
    jparse_ctx_t jctx;
    int cert_count;
    int noc_pem_len;
    size_t noc_der_len;
    char matter_node_id[17] = {0};
    char noc_user_id[17] = {0};
    ScopedMemoryBufferWithSize<char> noc_pem;
    ScopedMemoryBufferWithSize<char> noc_pem_formatted;
    ScopedMemoryBufferWithSize<unsigned char> noc_der;
    ScopedMemoryBufferWithSize<unsigned char> noc_matter_cert;
    MutableByteSpan matter_cert_noc;
    auto &fabric_table = chip::Server::GetInstance().GetFabricTable();
    const chip::FabricInfo *fabric_info = fabric_table.FindFabricWithIndex(fabric_index);
    snprintf(matter_node_id, 17, "%016llX", fabric_info->GetNodeId());
    matter_node_id[16] = 0;

    ScopedMemoryBufferWithSize<char> endpoint_url;
    ScopedMemoryBufferWithSize<char> access_token;
    ScopedMemoryBufferWithSize<char> http_payload;
    ScopedMemoryBufferWithSize<char> csr_pem_formatted;

    endpoint_url.Calloc(ESP_MATTER_RAINMAKER_MAX_ENDPOINT_URL_LEN);
    ESP_RETURN_ON_FALSE(endpoint_url.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for endpoint_url");

    access_token.Calloc(ESP_MATTER_RAINMAKER_MAX_ACCESS_TOKEN_LEN);
    ESP_RETURN_ON_FALSE(access_token.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for access_token");

    http_payload.Calloc(2048);
    ESP_RETURN_ON_FALSE(http_payload.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for http_payload");

    csr_pem_formatted.Calloc(csr_pem.AllocatedSize());
    ESP_RETURN_ON_FALSE(csr_pem_formatted.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for csr_pem_formatted");

    format_csr(csr_pem.Get(), csr_pem_formatted.Get());

    ESP_RETURN_ON_ERROR(attribute::endpoint_url_attribute_get(endpoint_id, endpoint_url.Get()), TAG,
                        "Failed to get endpoint_url attribute value");
    ESP_RETURN_ON_ERROR(attribute::access_token_attribute_get(endpoint_id, access_token.Get()), TAG,
                        "Failed to get access_token attribute value");

    snprintf(url, sizeof(url), "%s/%s/%s", endpoint_url.Get(), HTTP_API_VERSION, "user/node_group");
    client = esp_http_client_init(&config);
    ESP_RETURN_ON_FALSE(client, ESP_FAIL, TAG, "Failed to initialise HTTP Client.");

    ESP_GOTO_ON_ERROR(esp_http_client_set_header(client, "accept", "application/json"), cleanup, TAG,
                      "Failed to set http header accept");
    ESP_GOTO_ON_ERROR(esp_http_client_set_header(client, "Authorization", access_token.Get()), cleanup, TAG,
                      "Failed to set http header Authorization");
    ESP_GOTO_ON_ERROR(esp_http_client_set_header(client, "Content-Type", "application/json"), cleanup, TAG,
                      "Failed to set http header Content-Type");
    ESP_GOTO_ON_ERROR(esp_http_client_set_method(client, HTTP_METHOD_PUT), cleanup, TAG, "Failed to set http method");

    json_gen_str_start(&jstr, http_payload.Get(), http_payload.AllocatedSize(), NULL, NULL);
    json_gen_start_object(&jstr);
    json_gen_obj_set_string(&jstr, "operation", "add");
    json_gen_push_array(&jstr, "csr_requests");
    json_gen_start_object(&jstr);
    json_gen_obj_set_string(&jstr, "role", "secondary");
    json_gen_obj_set_string(&jstr, "matter_node_id", matter_node_id);
    json_gen_obj_set_string(&jstr, "group_id", rainmaker_group_id.Get());
    json_gen_obj_set_string(&jstr, "csr", csr_pem_formatted.Get());
    json_gen_end_object(&jstr);
    json_gen_pop_array(&jstr);
    json_gen_obj_set_string(&jstr, "csr_type", "controller");
    json_gen_end_object(&jstr);
    json_gen_str_end(&jstr);
    ESP_LOGI(TAG, "http write payload: %s", http_payload.Get());

    // Send POST data
    ESP_GOTO_ON_ERROR(esp_http_client_open(client, strlen(http_payload.Get())), cleanup, TAG,
                      "Failed to open http connection");
    http_len = esp_http_client_write(client, http_payload.Get(), strlen(http_payload.Get()));
    if (http_len != strlen(http_payload.Get())) {
        ESP_LOGE(TAG, "Failed to write Payload. Returned len = %d.", http_len);
        ret = ESP_FAIL;
        goto close;
    }
    http_len = esp_http_client_fetch_headers(client);
    http_status_code = esp_http_client_get_status_code(client);
    // Read response
    if ((http_len > 0) && (http_status_code == 200)) {
        http_len = esp_http_client_read_response(client, http_payload.Get(), http_payload.AllocatedSize() - 1);
        http_payload[http_len] = 0;
    } else {
        http_len = esp_http_client_read_response(client, http_payload.Get(), http_payload.AllocatedSize() - 1);
        http_payload[http_len] = 0;
        ESP_LOGE(TAG, "Invalid response for %s", url);
        ESP_LOGE(TAG, "Status = %d, Data = %s", http_status_code, http_len > 0 ? http_payload.Get() : "None");
        ret = ESP_FAIL;
    }

    // Parse http response
    noc_pem.Calloc(1024);
    ESP_GOTO_ON_FALSE(noc_pem.Get(), ESP_ERR_NO_MEM, close, TAG, "Failed to allocate memory for noc_pem");

    noc_pem_formatted.Calloc(1024);
    ESP_GOTO_ON_FALSE(noc_pem_formatted.Get(), ESP_ERR_NO_MEM, close, TAG,
                      "Failed to allocate memory for noc_pem_formatted");

    ESP_GOTO_ON_FALSE(json_parse_start(&jctx, http_payload.Get(), strlen(http_payload.Get())) == 0, ESP_FAIL, close,
                      TAG, "Failed to parse the http response json on json_parse_start");
    if (json_obj_get_array(&jctx, "certificates", &cert_count) == 0 && cert_count == 1) {
        if (json_arr_get_object(&jctx, 0) == 0) {
            if (json_obj_get_strlen(&jctx, "user_noc", &noc_pem_len) == 0 &&
                json_obj_get_string(&jctx, "user_noc", noc_pem.Get(), noc_pem.AllocatedSize()) == 0) {
                noc_pem[noc_pem_len] = 0;
                if (json_obj_get_string(&jctx, "matter_user_id", noc_user_id, 17) == 0) {
                    ESP_LOGI(TAG, "NOC user id : 0x%s", noc_user_id);
                }
            }
            json_arr_leave_object(&jctx);
        }
        json_obj_leave_array(&jctx);
    }
    json_parse_end(&jctx);

    format_noc(noc_pem.Get(), noc_pem_formatted.Get());
    noc_pem.Free();

    ESP_LOGD(TAG, "new noc_pem :\n%s", noc_pem_formatted.Get());

    noc_der.Calloc(chip::Credentials::kMaxDERCertLength);
    ESP_GOTO_ON_FALSE(noc_der.Get(), ESP_ERR_NO_MEM, close, TAG, "Failed to allocate memory for noc_der");
    noc_der_len = chip::Credentials::kMaxDERCertLength;

    // Convert PEM-encoded NOC to DER-encoded NOC
    ESP_GOTO_ON_FALSE(convert_pem_to_der(noc_pem_formatted.Get(),
                                         strnlen(noc_pem_formatted.Get(), noc_pem_formatted.AllocatedSize()),
                                         noc_der.Get(), &noc_der_len) == 0,
                      ESP_FAIL, close, TAG, "Failed to convert PEM-encoded NOC to DER-encoded NOC");
    noc_pem_formatted.Free();

    noc_matter_cert.Calloc(chip::Credentials::kMaxCHIPCertLength);
    ESP_GOTO_ON_FALSE(noc_matter_cert.Get(), ESP_ERR_NO_MEM, close, TAG,
                      "Failed to allocate memory for noc_matter_cert");

    matter_cert_noc = MutableByteSpan(noc_matter_cert.Get(), noc_matter_cert.AllocatedSize());
    chip::Credentials::ConvertX509CertToChipCert(
        ByteSpan{reinterpret_cast<const unsigned char *>(noc_der.Get()), noc_der_len}, matter_cert_noc);

    // Update NOC
    ESP_GOTO_ON_FALSE(fabric_table.UpdatePendingFabricWithOperationalKeystore(fabric_index, matter_cert_noc,
                                                                              ByteSpan{}) == CHIP_NO_ERROR,
                      ESP_FAIL, close, TAG, "Failed to update the Fabric NOC");
    ESP_GOTO_ON_FALSE(fabric_table.CommitPendingFabricData() == CHIP_NO_ERROR, ESP_FAIL, close, TAG,
                      "Failed to commit the pending Fabric data");
    // Start DNS server to advertise the new node-id of the new NOC
    chip::app::DnssdServer::Instance().StartServer();

close:
    esp_http_client_close(client);
cleanup:
    esp_http_client_cleanup(client);
    return ret;
}

static esp_err_t install_user_noc(uint16_t endpoint_id, uint8_t fabric_index)
{
    const chip::FabricInfo *fabric_info =
        chip::Server::GetInstance().GetFabricTable().FindFabricWithIndex(fabric_index);
    uint64_t fabric_id = fabric_info->GetFabricId();
    ScopedMemoryBufferWithSize<char> rainmaker_group_id;
    ScopedMemoryBufferWithSize<char> next_group_id;
    ScopedMemoryBufferWithSize<char> csr_pem;
    size_t csr_pem_len = 1024;
    esp_err_t err = ESP_OK;

    // Alloc memory for ScopedMemoryBuffers
    rainmaker_group_id.Calloc(ESP_MATTER_RAINMAKER_MAX_GROUP_ID_LEN);
    ESP_RETURN_ON_FALSE(rainmaker_group_id.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for rainmaker_group_id");
    next_group_id.Calloc(ESP_MATTER_RAINMAKER_MAX_GROUP_ID_LEN);
    ESP_RETURN_ON_FALSE(next_group_id.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for next_group_id");
    csr_pem.Calloc(csr_pem_len);
    ESP_RETURN_ON_FALSE(csr_pem.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for csr_buf");

    // Fetch ther rainmaker_group_id
    int group_count = 0;
    const int num_records = 4;
    strcpy(next_group_id.Get(), "0");
    do {
        err = fetch_rainmaker_group_id(endpoint_id, rainmaker_group_id, fabric_id, next_group_id, num_records,
                                       group_count);
    } while (err == ESP_ERR_NOT_FOUND && group_count == num_records);
    ESP_RETURN_ON_ERROR(err, TAG, "Failed to fetch rainmaker_group_id");
    // Update the rainmaker_group_id
    ESP_RETURN_ON_ERROR(attribute::rainmaker_group_id_attribute_update(endpoint_id, rainmaker_group_id.Get()), TAG,
                        "Failed to update rainmaker_group_id");
    // Generate CSR for User NOC
    ESP_RETURN_ON_ERROR(generate_user_noc_csr(csr_pem, fabric_index, csr_pem_len), TAG,
                        "Failed to generate User NOC CSR");
    // Request to issue User NOC
    ESP_RETURN_ON_ERROR(issue_user_noc_request(csr_pem, rainmaker_group_id, csr_pem_len, fabric_index, endpoint_id),
                        TAG, "Failed to request to issue User NOC");
    // Update attribute
    ESP_RETURN_ON_ERROR(attribute::user_noc_installed_attribute_update(endpoint_id, true), TAG,
                        "Failed to update user_noc_installed");
    ESP_RETURN_ON_ERROR(attribute::user_noc_fabric_index_attribute_update(endpoint_id, fabric_index), TAG,
                        "Failed to update user_noc_fabric_index");
    // Update controller fabric index
    esp_matter::controller::set_fabric_index(fabric_index);

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
    ESP_RETURN_ON_ERROR(attribute::refresh_token_attribute_get(endpoint_id, refresh_token.Get()), TAG,
                        "Failed to get refresh_token");

    size_t current_len = strnlen(refresh_token.Get(), refresh_token.AllocatedSize() - 1);
    size_t append_len = strnlen(append_str.Get(), 1024);
    strncpy(&refresh_token[current_len], append_str.Get(), append_len);
    refresh_token[current_len + append_len] = 0;
    ESP_RETURN_ON_ERROR(attribute::refresh_token_attribute_update(endpoint_id, refresh_token.Get()), TAG,
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
    attribute::refresh_token_attribute_update(endpoint_id, empty_str);
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
    ESP_RETURN_ON_ERROR(attribute::endpoint_url_attribute_update(endpoint_id, endpoint_url.Get()), TAG,
                        "Failed to update endpoint_url");

    ESP_RETURN_ON_ERROR(controller::controller_authorize(endpoint_id), TAG, "Failed to authorize the controller");

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
    ESP_RETURN_ON_ERROR(attribute::authorized_attribute_get(endpoint_id, authorized), TAG,
                        "Failed to get authorized attribute");
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

command_t *create_append_refresh_token(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, append_refresh_token::Id, COMMAND_FLAG_ACCEPTED | COMMAND_FLAG_CUSTOM,
                                       append_refresh_token_command_callback);
}

command_t *create_reset_refresh_token(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, reset_refresh_token::Id, COMMAND_FLAG_ACCEPTED | COMMAND_FLAG_CUSTOM,
                                       reset_refresh_token_command_callback);
}

command_t *create_authorize(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, authorize::Id, COMMAND_FLAG_ACCEPTED | COMMAND_FLAG_CUSTOM,
                                       authorize_command_callback);
}

command_t *create_update_user_noc(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, update_user_noc::Id, COMMAND_FLAG_ACCEPTED | COMMAND_FLAG_CUSTOM,
                                       update_user_noc_command_callback);
}

command_t *create_update_device_list(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, update_device_list::Id, COMMAND_FLAG_ACCEPTED | COMMAND_FLAG_CUSTOM,
                                       update_device_list_command_callback);
}

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
    case attribute::access_token::Id: {
        ScopedMemoryBufferWithSize<char> access_token;
        access_token.Alloc(ESP_MATTER_RAINMAKER_MAX_ACCESS_TOKEN_LEN);
        if (!access_token.Get()) {
            return CHIP_ERROR_NO_MEMORY;
        }
        err = attribute::access_token_attribute_get(endpoint_id, access_token.Get());
        err = encode_string_on_success(err, aEncoder, access_token.Get(), access_token.AllocatedSize());
        break;
    }
    case attribute::refresh_token::Id: {
        ScopedMemoryBufferWithSize<char> refresh_token;
        refresh_token.Alloc(ESP_MATTER_RAINMAKER_MAX_REFRESH_TOKEN_LEN);
        if (!refresh_token.Get()) {
            return CHIP_ERROR_NO_MEMORY;
        }
        err = attribute::refresh_token_attribute_get(endpoint_id, refresh_token.Get());
        err = encode_string_on_success(err, aEncoder, refresh_token.Get(), refresh_token.AllocatedSize());
        break;
    }
    case attribute::endpoint_url::Id: {
        ScopedMemoryBufferWithSize<char> endpoint_url;
        endpoint_url.Alloc(ESP_MATTER_RAINMAKER_MAX_ENDPOINT_URL_LEN);
        if (!endpoint_url.Get()) {
            return CHIP_ERROR_NO_MEMORY;
        }
        err = attribute::endpoint_url_attribute_get(endpoint_id, endpoint_url.Get());
        err = encode_string_on_success(err, aEncoder, endpoint_url.Get(), endpoint_url.AllocatedSize());
        break;
    }
    case attribute::rainmaker_group_id::Id: {
        ScopedMemoryBufferWithSize<char> rainmaker_group_id;
        rainmaker_group_id.Alloc(ESP_MATTER_RAINMAKER_MAX_GROUP_ID_LEN);
        if (!rainmaker_group_id.Get()) {
            return CHIP_ERROR_NO_MEMORY;
        }
        err = attribute::rainmaker_group_id_attribute_get(endpoint_id, rainmaker_group_id.Get());
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
    attribute::create_authorized(cluster, false);
    attribute::create_user_noc_installed(cluster, false);
    attribute::create_user_noc_fabric_index(cluster, 0);
    // Attributes managed internally
    attribute::create_refresh_token(cluster, NULL, 0);
    attribute::create_access_token(cluster, NULL, 0);
    attribute::create_endpoint_url(cluster, NULL, 0);
    attribute::create_rainmaker_group_id(cluster, NULL, 0);

    command::create_append_refresh_token(cluster);
    command::create_reset_refresh_token(cluster);
    command::create_authorize(cluster);
    command::create_update_user_noc(cluster);
    command::create_update_device_list(cluster);

    return cluster;
}

} // namespace matter_controller
} // namespace cluster
namespace controller {

static esp_err_t fetch_access_token(ScopedMemoryBufferWithSize<char> &refresh_token,
                                    ScopedMemoryBufferWithSize<char> &endpoint_url,
                                    ScopedMemoryBufferWithSize<char> &access_token)
{
    esp_err_t ret = ESP_OK;
    char url[100];
    snprintf(url, sizeof(url), "%s/%s/%s", endpoint_url.Get(), HTTP_API_VERSION, "login2");
    esp_http_client_config_t config = {
        .url = url,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .buffer_size = 2048,
        .skip_cert_common_name_check = false,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    esp_http_client_handle_t client = NULL;
    ScopedMemoryBufferWithSize<char> http_payload;
    constexpr size_t http_payload_size =
        ESP_MATTER_RAINMAKER_MAX_REFRESH_TOKEN_LEN + ESP_MATTER_RAINMAKER_MAX_ACCESS_TOKEN_LEN + 256;
    int http_len, http_status_code;
    int access_token_len = 0;
    json_gen_str_t jstr;
    jparse_ctx_t jctx;

    ESP_RETURN_ON_FALSE(access_token.Get(), ESP_ERR_INVALID_ARG, TAG, "access_token pointer cannot be NULL");
    client = esp_http_client_init(&config);
    ESP_RETURN_ON_FALSE(client, ESP_FAIL, TAG, "Failed to initialise HTTP Client.");

    ESP_GOTO_ON_ERROR(esp_http_client_set_header(client, "accept", "application/json"), cleanup, TAG,
                      "Failed to set http header accept");
    ESP_GOTO_ON_ERROR(esp_http_client_set_header(client, "Content-Type", "application/json"), cleanup, TAG,
                      "Failed to set http header Content-Type");
    ESP_GOTO_ON_ERROR(esp_http_client_set_method(client, HTTP_METHOD_POST), cleanup, TAG, "Failed to set http method");

    // Prepare the payload for http write and read
    http_payload.Calloc(http_payload_size);
    ESP_GOTO_ON_FALSE(http_payload.Get(), ESP_ERR_NO_MEM, cleanup, TAG, "Failed to alloc memory for http_payload");
    json_gen_str_start(&jstr, http_payload.Get(), http_payload.AllocatedSize(), NULL, NULL);
    json_gen_start_object(&jstr);
    json_gen_obj_set_string(&jstr, "refreshtoken", refresh_token.Get());
    json_gen_end_object(&jstr);
    json_gen_str_end(&jstr);
    ESP_LOGD(TAG, "http write payload: %s", http_payload.Get());

    // Send POST data
    ESP_GOTO_ON_ERROR(esp_http_client_open(client, strnlen(http_payload.Get(), http_payload_size)), cleanup, TAG,
                      "Failed to open http connection");
    http_len = esp_http_client_write(client, http_payload.Get(), strnlen(http_payload.Get(), http_payload_size));
    if (http_len != strnlen(http_payload.Get(), http_payload_size)) {
        ESP_LOGE(TAG, "Failed to write Payload. Returned len = %d.", http_len);
        ret = ESP_FAIL;
        goto close;
    }
    http_len = esp_http_client_fetch_headers(client);
    http_status_code = esp_http_client_get_status_code(client);
    // Read response
    if ((http_len > 0) && (http_status_code == 200)) {
        http_len = esp_http_client_read_response(client, http_payload.Get(), http_payload_size - 1);
        http_payload[http_len] = 0;
    } else {
        http_len = esp_http_client_read_response(client, http_payload.Get(), http_payload_size - 1);
        http_payload[http_len] = 0;
        ESP_LOGE(TAG, "Invalid response for %s", url);
        ESP_LOGE(TAG, "Status = %d, Data = %s", http_status_code, http_len > 0 ? http_payload.Get() : "None");
        ret = ESP_FAIL;
        goto close;
    }
    // Parse the response payload
    ESP_GOTO_ON_FALSE(json_parse_start(&jctx, http_payload.Get(), strnlen(http_payload.Get(), http_payload_size)) == 0,
                      ESP_FAIL, close, TAG, "Failed to parse the http response json on json_parse_start");
    if (json_obj_get_strlen(&jctx, "accesstoken", &access_token_len) != 0 ||
        json_obj_get_string(&jctx, "accesstoken", access_token.Get(), access_token.AllocatedSize()) != 0) {
        ESP_LOGE(TAG, "Failed to parse the access token from the http response json");
        ret = ESP_FAIL;
    } else {
        access_token[access_token_len] = 0;
    }
    json_parse_end(&jctx);

close:
    esp_http_client_close(client);
cleanup:
    esp_http_client_cleanup(client);
    return ret;
}

static uint16_t s_access_token_expired_timer_endpoint_id = chip::kInvalidEndpointId;

static void access_token_expired_callback(chip::System::Layer *systemLayer, void *appState)
{
    uint16_t endpoint_id = s_access_token_expired_timer_endpoint_id;
    if (cluster::matter_controller::attribute::authorized_attribute_update(endpoint_id, false) != ESP_OK) {
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
    ScopedMemoryBufferWithSize<char> access_token;

    // Alloc memory for ScopedMemoryBuffers
    refresh_token.Calloc(ESP_MATTER_RAINMAKER_MAX_REFRESH_TOKEN_LEN);
    ESP_RETURN_ON_FALSE(refresh_token.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for refresh_token");
    endpoint_url.Calloc(ESP_MATTER_RAINMAKER_MAX_ENDPOINT_URL_LEN);
    ESP_RETURN_ON_FALSE(endpoint_url.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for endpoint_url");
    access_token.Calloc(ESP_MATTER_RAINMAKER_MAX_ACCESS_TOKEN_LEN);
    ESP_RETURN_ON_FALSE(access_token.Get(), ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for access_token");

    ESP_RETURN_ON_ERROR(
        cluster::matter_controller::attribute::endpoint_url_attribute_get(endpoint_id, endpoint_url.Get()), TAG,
        "Failed to get the endpoint_url");
    ESP_RETURN_ON_ERROR(
        cluster::matter_controller::attribute::refresh_token_attribute_get(endpoint_id, refresh_token.Get()), TAG,
        "Failed to get the refresh_token");

    // Fetch the access_token
    ESP_RETURN_ON_ERROR(fetch_access_token(refresh_token, endpoint_url, access_token), TAG,
                        "Failed to fetch access_token for authorizing");
    // Update the access token
    ESP_RETURN_ON_ERROR(
        cluster::matter_controller::attribute::access_token_attribute_update(endpoint_id, access_token.Get()), TAG,
        "Failed to update access_token");
    // Update the authorized attribute
    ESP_RETURN_ON_ERROR(cluster::matter_controller::attribute::authorized_attribute_update(endpoint_id, true), TAG,
                        "Failed to update authorized attribute");
    // The access token will be expired after one hour
    s_access_token_expired_timer_endpoint_id = endpoint_id;
    SystemLayer().CancelTimer(access_token_expired_callback, NULL);
    SystemLayer().StartTimer(Seconds32(3600), access_token_expired_callback, NULL);

    return ESP_OK;
}

} // namespace controller
} // namespace esp_matter
