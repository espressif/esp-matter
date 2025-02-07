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

#include "esp_matter_controller_utils.h"
#include <credentials/attestation_verifier/DefaultDeviceAttestationVerifier.h>
#include <esp_check.h>
#include <esp_crt_bundle.h>
#include <esp_http_client.h>
#include <esp_log.h>
#include <esp_matter_attestation_trust_store.h>
#include <esp_spiffs.h>
#include <json_parser.h>
#include <mbedtls/base64.h>

const char TAG[] = "spiffs_attestation";

namespace chip {
namespace Credentials {

static const char *get_filename_extension(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) {
        return "";
    }
    return dot + 1;
}

paa_der_cert_iterator::paa_der_cert_iterator(const char *path)
{
    strncpy(m_path, path, strnlen(path, 16));
    m_path[15] = 0;
    m_dir = opendir(path);
    if (!m_dir) {
        ESP_LOGE(TAG, "Failed to open the directory");
        return;
    }
    m_count = 0;
    m_index = 0;
    dirent *entry = NULL;
    while ((entry = readdir(m_dir)) != NULL) {
        const char *extension = get_filename_extension(entry->d_name);
        if (strncmp(extension, "der", strlen("der")) == 0) {
            m_count++;
        }
    }
    if (m_count == 0) {
        ESP_LOGE(TAG, "No DER file in the directory");
        closedir(m_dir);
        m_dir = NULL;
    } else {
        rewinddir(m_dir);
    }
}

bool paa_der_cert_iterator::next(paa_der_cert_t &item)
{
    dirent *entry = NULL;
    if (m_index >= m_count) {
        return false;
    }
    while ((entry = readdir(m_dir)) != NULL) {
        const char *extension = get_filename_extension(entry->d_name);
        if (strncmp(extension, "der", strlen("der")) == 0) {
            break;
        }
    }
    if (!entry) {
        return false;
    }
    m_index++;
    char filename[280] = {0};
    snprintf(filename, sizeof(filename), "%s/%s", m_path, entry->d_name);
    FILE *file = fopen(filename, "rb");
    if (file == nullptr) {
        item.m_len = 0;
        return true;
    }
    item.m_len = fread(item.m_buffer, sizeof(uint8_t), kMaxDERCertLength, file);
    fclose(file);
    return true;
}

void paa_der_cert_iterator::release()
{
    if (m_dir) {
        closedir(m_dir);
    }
}

esp_err_t spiffs_attestation_trust_store::init()
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/paa", .partition_label = nullptr, .max_files = 5, .format_if_mount_failed = false};
    ESP_RETURN_ON_ERROR(esp_vfs_spiffs_register(&conf), TAG, "Failed to initialize SPIFFS");
    size_t total = 0, used = 0;
    ESP_RETURN_ON_ERROR(esp_spiffs_info(conf.partition_label, &total, &used), TAG, "Failed to get SPIFFS info");
    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    m_is_initialized = true;
    return ESP_OK;
}

CHIP_ERROR spiffs_attestation_trust_store::GetProductAttestationAuthorityCert(const ByteSpan &skid,
                                                                              MutableByteSpan &outPaaDerBuffer) const
{
    if (m_is_initialized) {
        paa_der_cert_iterator iter("/paa");
        paa_der_cert_t paa_cert;
        while (iter.next(paa_cert)) {
            if (paa_cert.m_len == 0) {
                continue;
            }
            uint8_t skid_buf[Crypto::kSubjectKeyIdentifierLength] = {0};
            MutableByteSpan skid_span{skid_buf};
            if (CHIP_NO_ERROR !=
                Crypto::ExtractSKIDFromX509Cert(ByteSpan{paa_cert.m_buffer, paa_cert.m_len}, skid_span)) {
                continue;
            }

            if (skid.data_equal(skid_span)) {
                return CopySpanToMutableSpan(ByteSpan{paa_cert.m_buffer, paa_cert.m_len}, outPaaDerBuffer);
            }
        }
        return CHIP_ERROR_CA_CERT_NOT_FOUND;
    }
    return CHIP_ERROR_INCORRECT_STATE;
}

#if CONFIG_DCL_ATTESTATION_TRUST_STORE
static void remove_backslash_n(char *str)
{
    char *src = str, *dst = str;
    while (*src) {
        if (*src == '\\' && *(src + 1) == 'n' && *(src + 1) != '\0') {
            src += 2;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

static esp_err_t convert_pem_to_der(const char *pem, uint8_t *der_buf, size_t *der_len)
{
    ESP_RETURN_ON_FALSE(pem && strlen(pem) > 0, ESP_ERR_INVALID_ARG, TAG, "pem cannot be NULL");
    ESP_RETURN_ON_FALSE(der_buf && der_len, ESP_ERR_INVALID_ARG, TAG, "der_buf cannot be NULL");
    size_t pem_len = strlen(pem);
    size_t len = 0;
    const char *s1, *s2, *end = pem + pem_len;
    constexpr char *begin_mark = "-----BEGIN";
    constexpr char *end_mark = "-----END";
    s1 = (char *)strstr(pem, begin_mark);
    if (s1 == NULL) {
        return ESP_FAIL;
    }
    s2 = (char *)strstr(pem, end_mark);
    if (s2 == NULL) {
        return ESP_FAIL;
    }
    s1 += strlen(begin_mark);
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
    int ret = mbedtls_base64_decode(NULL, 0, &len, (const unsigned char *)s1, s2 - s1);
    if (ret == MBEDTLS_ERR_BASE64_INVALID_CHARACTER) {
        return ESP_FAIL;
    }
    if (len > *der_len) {
        return ESP_FAIL;
    }
    if ((ret = mbedtls_base64_decode(der_buf, len, &len, (const unsigned char *)s1, s2 - s1)) != 0) {
        return ESP_FAIL;
    }
    *der_len = len;
    return ESP_OK;
}

CHIP_ERROR dcl_attestation_trust_store::GetProductAttestationAuthorityCert(const ByteSpan &skid,
                                                                           MutableByteSpan &outPaaDerBuffer) const
{
    VerifyOrReturnError(skid.size() == Crypto::kSubjectKeyIdentifierLength, CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(outPaaDerBuffer.size() > 0 && outPaaDerBuffer.size() <= kMaxDERCertLength,
                        CHIP_ERROR_INVALID_ARGUMENT);
    char url[200];
    int offset = 0;
    esp_err_t ret = ESP_OK;
    if (dcl_net_type == DCL_MAIN_NET) {
        offset += snprintf(url, sizeof(url), "%s", "https://on.dcl.csa-iot.org/dcl/pki/certificates?subjectKeyId=");
    } else {
        // DCL_TEST_NET
        offset +=
            snprintf(url, sizeof(url), "%s", "https://on.test-net.dcl.csa-iot.org/dcl/pki/certificates?subjectKeyId=");
    }

    for (size_t i = 0; i < skid.size(); ++i) {
        if (i == skid.size() - 1) {
            offset += snprintf(url + offset, sizeof(url) - offset, "%02X", skid[i]);
        } else {
            offset += snprintf(url + offset, sizeof(url) - offset, "%02X%%3a", skid[i]);
        }
    }

    ChipLogProgress(Controller, "DCL Attestation URL: %s", url);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .timeout_ms = 10000,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .buffer_size = 1024,
        .skip_cert_common_name_check = false,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    esp_http_client_handle_t client = NULL;
    ScopedMemoryBufferWithSize<char> http_payload;
    int http_len, http_status_code;
    int certificates_count, certs_count, paa_str_len;
    jparse_ctx_t jctx;
    const size_t paa_pem_size = 1024;
    size_t paa_der_len = outPaaDerBuffer.size();

    client = esp_http_client_init(&config);
    if (!client) {
        ESP_LOGE(TAG, "Failed to initialise HTTP Client.");
        return CHIP_ERROR_NO_MEMORY;
    }

    char *paa_pem_buffer = (char *)malloc(paa_pem_size);
    ESP_GOTO_ON_FALSE(paa_pem_buffer, ESP_ERR_NO_MEM, cleanup, TAG, "Failed to alloc memory for paa_pem_buffer");
    ESP_GOTO_ON_ERROR(esp_http_client_set_header(client, "accept", "application/json"), cleanup, TAG,
                      "Failed to set http header accept");
    ESP_GOTO_ON_ERROR(esp_http_client_open(client, 0), cleanup, TAG, "Failed to open http connection");

    // Read Response
    http_len = esp_http_client_fetch_headers(client);
    http_status_code = esp_http_client_get_status_code(client);
    http_payload.Calloc(2400);
    ESP_GOTO_ON_FALSE(http_payload.Get(), ESP_ERR_NO_MEM, close, TAG, "Failed to alloc memory for http_payload");
    if (http_status_code == HttpStatus_Ok) {
        http_len = esp_http_client_read_response(client, http_payload.Get(), http_payload.AllocatedSize());
        http_payload[http_len] = '\0';
    } else {
        ESP_LOGE(TAG, "Status = %d. Invalid response for %s", http_status_code, url);
        ret = ESP_FAIL;
        goto close;
    }

    // Parse the response payload
    ESP_GOTO_ON_FALSE(json_parse_start(&jctx, http_payload.Get(), http_len) == 0, ESP_FAIL, close, TAG,
                      "Failed to parse the http response json on json_parse_start");
    if (json_obj_get_array(&jctx, "approvedCertificates", &certificates_count) == 0 && certificates_count == 1) {
        if (json_arr_get_object(&jctx, 0) == 0) {
            if (json_obj_get_array(&jctx, "certs", &certs_count) == 0 && certs_count == 1) {
                if (json_arr_get_object(&jctx, 0) == 0) {
                    if (json_obj_get_strlen(&jctx, "pemCert", &paa_str_len) == 0 &&
                        json_obj_get_string(&jctx, "pemCert", paa_pem_buffer, paa_pem_size) == 0) {
                        paa_str_len = paa_str_len < paa_pem_size - 1 ? paa_str_len : paa_pem_size - 1;
                        paa_pem_buffer[paa_str_len] = 0;
                        remove_backslash_n(paa_pem_buffer);
                        ret = convert_pem_to_der(paa_pem_buffer, outPaaDerBuffer.data(), &paa_der_len);
                        if (ret == ESP_OK) {
                            outPaaDerBuffer.reduce_size(paa_der_len);
                        }
                    }
                    json_obj_leave_object(&jctx);
                }
                json_obj_leave_array(&jctx);
            } else {
                ret = ESP_FAIL;
            }
            json_obj_leave_object(&jctx);
        } else {
            ret = ESP_FAIL;
        }
        json_obj_leave_array(&jctx);
    } else {
        ret = ESP_FAIL;
    }
    json_parse_end(&jctx);

close:
    esp_http_client_close(client);
cleanup:
    free(paa_pem_buffer);
    esp_http_client_cleanup(client);
    return ret == ESP_OK ? CHIP_NO_ERROR : CHIP_ERROR_INTERNAL;
}
#endif // CONFIG_DCL_ATTESTATION_TRUST_STORE

const AttestationTrustStore *get_attestation_trust_store()
{
#if CONFIG_TEST_ATTESTATION_TRUST_STORE
    return GetTestAttestationTrustStore();
#elif CONFIG_SPIFFS_ATTESTATION_TRUST_STORE
    spiffs_attestation_trust_store::get_instance().init();
    return &spiffs_attestation_trust_store::get_instance();
#elif CONFIG_DCL_ATTESTATION_TRUST_STORE
    return &dcl_attestation_trust_store::get_instance();
#endif
}

} // namespace Credentials
} // namespace chip
