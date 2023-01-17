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

#include <credentials/attestation_verifier/DefaultDeviceAttestationVerifier.h>
#include <esp_check.h>
#include <esp_log.h>
#include <esp_matter_attestation_trust_store.h>
#include <esp_spiffs.h>

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

const AttestationTrustStore *get_attestation_trust_store()
{
    // TODO: Fetch the PAA certificates from DCL
#if CONFIG_TEST_ATTESTATION_TRUST_STORE
    return GetTestAttestationTrustStore();
#elif CONFIG_SPIFFS_ATTESTATION_TRUST_STORE
    spiffs_attestation_trust_store::get_instance().init();
    return &spiffs_attestation_trust_store::get_instance();
#endif
}

} // namespace Credentials
} // namespace chip
