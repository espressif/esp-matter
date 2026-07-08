// Copyright 2023-2025 Espressif Systems (Shanghai) PTE LTD
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

#include <cJSON.h>
#include <esp_check.h>
#include <esp_crt_bundle.h>
#include <esp_http_client.h>
#include <esp_log.h>
#include <esp_matter_attestation_trust_store.h>
#include <esp_matter_controller_utils.h>
#include <esp_matter_mem.h>
#include <esp_spiffs.h>
#include <mbedtls/base64.h>

#include <attestation_verification_utils.h>
#include <credentials/attestation_verifier/DefaultDeviceAttestationVerifier.h>
#include <http_client_get.h>

const char TAG[] = "attestation_verification";

using namespace esp_matter::controller::attestation_verification;

namespace chip {
namespace Credentials {

#ifdef CONFIG_SPIFFS_ATTESTATION_TRUST_STORE
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
    if (path == nullptr) {
        assert(false);
        return;
    }
    strlcpy(m_path, path, sizeof(m_path));
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
    esp_vfs_spiffs_conf_t conf = {.base_path = "/paa",
#ifdef CONFIG_SPIFFS_ATTESTATION_TRUST_STORE_PARTITION_LABEL
                                  .partition_label = CONFIG_SPIFFS_ATTESTATION_TRUST_STORE_PARTITION_LABEL,
#else
                                  .partition_label = nullptr,
#endif
                                  .max_files = 5,
                                  .format_if_mount_failed = false
                                 };
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
#endif // CONFIG_SPIFFS_ATTESTATION_TRUST_STORE

#if CONFIG_DCL_ATTESTATION_TRUST_STORE
typedef struct {
    CHIP_ERROR err;
    MutableByteSpan &outPaaDerBuffer;
} parse_paa_ctx_t;

static void parse_paa_response(http_resp_t *resp, void *ctx)
{
    parse_paa_ctx_t *resp_ctx = (parse_paa_ctx_t *)ctx;
    if (resp_ctx == nullptr) {
        return;
    }
    size_t paa_der_len = resp_ctx->outPaaDerBuffer.size();
    cJSON *root = nullptr;

    resp_ctx->err = CHIP_ERROR_CA_CERT_NOT_FOUND;
    // Parse the response payload
    ESP_LOGD(TAG, "response data: %s", (char *)resp->data);
    root = cJSON_ParseWithLength((char *)resp->data, resp->size);
    if (!root) {
        ESP_LOGE(TAG, "Failed to parse the http response json");
        return;
    }
    cJSON *approved_certificates = cJSON_GetObjectItemCaseSensitive(root, "approvedCertificates");
    if (!cJSON_IsArray(approved_certificates) || cJSON_GetArraySize(approved_certificates) != 1) {
        ESP_LOGE(TAG, "Failed to get the PAA response");
        cJSON_Delete(root);
        return;
    }
    cJSON *approved_certificate = cJSON_GetArrayItem(approved_certificates, 0);
    cJSON *certs = cJSON_GetObjectItemCaseSensitive(approved_certificate, "certs");
    if (cJSON_IsObject(approved_certificate) && cJSON_IsArray(certs) && cJSON_GetArraySize(certs) == 1) {
        cJSON *cert = cJSON_GetArrayItem(certs, 0);
        cJSON *pem_cert = cJSON_GetObjectItemCaseSensitive(cert, "pemCert");
        const char *pem_cert_value = cJSON_GetStringValue(pem_cert);
        if (cJSON_IsObject(cert) && pem_cert_value) {
            size_t paa_str_len = strlen(pem_cert_value);
            char *paa_pem_buffer = (char *)esp_matter_mem_calloc(paa_str_len + 1, 1);
            if (paa_pem_buffer) {
                memcpy(paa_pem_buffer, pem_cert_value, paa_str_len);
                remove_backslash_n(paa_pem_buffer);
                esp_err_t ret = convert_pem_to_der(paa_pem_buffer, resp_ctx->outPaaDerBuffer.data(), &paa_der_len);
                if (ret == ESP_OK) {
                    resp_ctx->outPaaDerBuffer.reduce_size(paa_der_len);
                    resp_ctx->err = CHIP_NO_ERROR;
                }
                esp_matter_mem_free(paa_pem_buffer);
            }
        }
    }
    cJSON_Delete(root);
}

CHIP_ERROR dcl_attestation_trust_store::GetProductAttestationAuthorityCert(const ByteSpan &skid,
                                                                           MutableByteSpan &outPaaDerBuffer) const
{
    VerifyOrReturnError(skid.size() == Crypto::kSubjectKeyIdentifierLength, CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(outPaaDerBuffer.size() > 0 && outPaaDerBuffer.size() <= kMaxDERCertLength,
                        CHIP_ERROR_INVALID_ARGUMENT);
    char url[200];
    int offset = snprintf(url, sizeof(url), "%s/dcl/pki/certificates?subjectKeyId=", m_dcl_net_base_url);
    for (size_t i = 0; i < skid.size(); ++i) {
        offset += snprintf(url + offset, sizeof(url) - offset, "%02X", skid[i]);
        if (i < skid.size() - 1) {
            offset += snprintf(url + offset, sizeof(url) - offset, "%%3A");
        }
    }
    url[offset] = 0;
    ChipLogProgress(Controller, "DCL Attestation URL: %s", url);

    char *header = strdup("accept: application/json");
    if (!header) {
        return CHIP_ERROR_NO_MEMORY;
    }
    parse_paa_ctx_t ctx = {
        .err = CHIP_ERROR_CA_CERT_NOT_FOUND,
        .outPaaDerBuffer = outPaaDerBuffer,
    };
    esp_err_t err = http_send_get_request(url, &header, 1, parse_paa_response, (void *)&ctx);
    free(header);
    if (err != ESP_OK) {
        return CHIP_ERROR_INTERNAL;
    }
    return ctx.err;
}
#endif // CONFIG_DCL_ATTESTATION_TRUST_STORE

#ifdef CONFIG_CUSTOM_ATTESTATION_TRUST_STORE
static AttestationTrustStore *s_custom_store = nullptr;

void set_custom_attestation_trust_store(AttestationTrustStore *store)
{
    s_custom_store = store;
}
#endif // CONFIG_CUSTOM_ATTESTATION_TRUST_STORE

const AttestationTrustStore *get_attestation_trust_store()
{
#if CONFIG_TEST_ATTESTATION_TRUST_STORE
    return GetTestAttestationTrustStore();
#elif CONFIG_SPIFFS_ATTESTATION_TRUST_STORE
    spiffs_attestation_trust_store::get_instance().init();
    return &spiffs_attestation_trust_store::get_instance();
#elif CONFIG_DCL_ATTESTATION_TRUST_STORE
    return &dcl_attestation_trust_store::get_instance();
#elif CONFIG_CUSTOM_ATTESTATION_TRUST_STORE
    return s_custom_store;
#else
    return nullptr;
#endif
}

} // namespace Credentials
} // namespace chip
