// Copyright 2025 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_err.h>
#include <esp_log.h>
#include <esp_matter_da_revocation_check.h>
#include <mbedtls/asn1.h>
#include <mbedtls/oid.h>
#include <mbedtls/x509.h>
#include <mbedtls/x509_crl.h>
#include <mbedtls/x509_crt.h>
#include <sys/time.h>

#include <crypto/CHIPCryptoPAL.h>

constexpr char *TAG = "attestation_verification";

using namespace chip;

namespace esp_matter {
namespace controller {
namespace attestation_verification {

// This function will get the current time. If the commissioner doesn't synchronize the time with the NTP server.
// it will use 2025-1-1 as the current time.
static esp_err_t get_current_time(mbedtls_x509_time &time)
{
    struct timeval tv;
    if (gettimeofday(&tv, nullptr) == 0) {
        ESP_LOGW(TAG, "Failed to get current time, using 2025-1-1 as the current time");
        time.year = 2025;
        time.mon = 1;
        time.day = 1;
        return ESP_FAIL;
    }
    const time_t timep = tv.tv_sec;
    struct tm calendar;
    localtime_r(&timep, &calendar);
    if (calendar.tm_year + 1900 < 2025) {
        ESP_LOGW(TAG, "Invalid UNIX time, using 2025-1-1 as the current time");
        time.year = 2025;
        time.mon = 1;
        time.day = 1;
        return ESP_FAIL;
    }
    time.year = calendar.tm_year + 1900;
    time.mon = calendar.tm_mon + 1;
    time.day = calendar.tm_mday;
    time.hour = calendar.tm_hour;
    time.min = calendar.tm_min;
    time.sec = calendar.tm_sec;
    return ESP_OK;
}

// This function will check whether the AKID of the CRL is the same as the inputting akid.
static bool check_crl_akid(const mbedtls_x509_crl *crl, const ByteSpan &akid)
{
    int ret = 0;
    unsigned char *p = crl->crl_ext.p;
    const unsigned char *end = p + crl->crl_ext.len;
    size_t len;
    if ((ret = mbedtls_asn1_get_tag(&p, end, &len, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE)) != 0) {
        ESP_LOGE(TAG, "Failed to parse CRL extensions sequence: -0x%04x", -ret);
        return false;
    }
    end = p + len;

    while (p < end) {
        size_t ext_len;
        mbedtls_asn1_buf extn_oid, extn_value;
        if ((ret = mbedtls_asn1_get_tag(&p, end, &ext_len, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE)) != 0) {
            ESP_LOGE(TAG, "Failed to parse extension entry: -0x%04x", -ret);
            break;
        }
        const unsigned char *ext_end = p + ext_len;
        extn_oid.tag = *p;
        if ((ret = mbedtls_asn1_get_tag(&p, ext_end, &extn_oid.len, MBEDTLS_ASN1_OID)) != 0) {
            ESP_LOGE(TAG, "Failed to get extension OID: -0x%04x", -ret);
            break;
        }
        extn_oid.p = p;
        p += extn_oid.len;
        if (p < ext_end && *p == MBEDTLS_ASN1_BOOLEAN) {
            int critical;
            if ((ret = mbedtls_asn1_get_bool(&p, ext_end, &critical)) != 0) {
                ESP_LOGE(TAG, "Failed to read critical flag: -0x%04x", -ret);
                break;
            }
        }

        if ((ret = mbedtls_asn1_get_tag(&p, ext_end, &extn_value.len, MBEDTLS_ASN1_OCTET_STRING)) != 0) {
            ESP_LOGE(TAG, "Failed to get extension value: -0x%04x", -ret);
            break;
        }
        extn_value.p = p;
        p += extn_value.len;
        if (MBEDTLS_OID_CMP(MBEDTLS_OID_AUTHORITY_KEY_IDENTIFIER, &extn_oid) == 0) {
            unsigned char *akid_p = extn_value.p;
            size_t akid_len = extn_value.len;
            size_t seq_len;
            if ((ret = mbedtls_asn1_get_tag(&akid_p, akid_p + akid_len, &seq_len,
                                            MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE)) != 0) {
                ESP_LOGE(TAG, "Failed to parse AuthorityKeyIdentifier: -0x%04x", -ret);
                break;
            }
            if ((ret = mbedtls_asn1_get_tag(&akid_p, akid_p + seq_len, &len, MBEDTLS_ASN1_CONTEXT_SPECIFIC | 0)) == 0) {
                return akid.data_equal(ByteSpan(akid_p, len));
            }
        }
    }
    return false;
}

bool cross_validate_cert(bool is_pai, const ByteSpan &da_cert, const ByteSpan &crl_signer_cert,
                         const ByteSpan &crl_signer_delegator)
{
    uint8_t da_cert_issuer_buf[Crypto::kMaxCertificateDistinguishedNameLength] = {0};
    uint8_t da_cert_akid_buf[Crypto::kAuthorityKeyIdentifierLength] = {0};
    MutableByteSpan da_cert_issuer(da_cert_issuer_buf);
    MutableByteSpan da_cert_akid(da_cert_akid_buf);
    Crypto::ExtractIssuerFromX509Cert(da_cert, da_cert_issuer);
    Crypto::ExtractAKIDFromX509Cert(da_cert, da_cert_akid);

    if (is_pai) {
        uint8_t subject_or_issuer_buf[Crypto::kMaxCertificateDistinguishedNameLength];
        uint8_t skid_or_akid_buf[Crypto::kSubjectKeyIdentifierLength];
        MutableByteSpan subject(subject_or_issuer_buf);
        MutableByteSpan skid(skid_or_akid_buf);
        Crypto::ExtractSubjectFromX509Cert(crl_signer_cert, subject);
        Crypto::ExtractSKIDFromX509Cert(crl_signer_cert, skid);
        if (da_cert_issuer.data_equal(subject) && da_cert_akid.data_equal(skid)) {
            return true;
        } else {
            MutableByteSpan issuer(subject_or_issuer_buf);
            MutableByteSpan akid(skid_or_akid_buf);
            Crypto::ExtractIssuerFromX509Cert(crl_signer_cert, issuer);
            Crypto::ExtractAKIDFromX509Cert(crl_signer_cert, akid);
            return da_cert_issuer.data_equal(issuer) && da_cert_akid.data_equal(akid);
        }
    } else {
        uint8_t subject_buf[Crypto::kMaxCertificateDistinguishedNameLength];
        uint8_t skid_buf[Crypto::kSubjectKeyIdentifierLength];
        MutableByteSpan subject(subject_buf);
        MutableByteSpan skid(skid_buf);
        if (crl_signer_delegator.size() > 0) {
            Crypto::ExtractSubjectFromX509Cert(crl_signer_delegator, subject);
            Crypto::ExtractSKIDFromX509Cert(crl_signer_delegator, skid);
        } else {
            Crypto::ExtractSubjectFromX509Cert(crl_signer_cert, subject);
            Crypto::ExtractSKIDFromX509Cert(crl_signer_cert, skid);
        }
        return da_cert_issuer.data_equal(subject) && da_cert_akid.data_equal(skid);
    }

    return false;
}

bool check_crl_signer_cert(const mbedtls_x509_crl *crl, const ByteSpan &crl_signer_cert)
{
    uint8_t signer_cert_subject_buf[Crypto::kMaxCertificateDistinguishedNameLength];
    uint8_t signer_cert_skid_buf[Crypto::kSubjectKeyIdentifierLength];
    MutableByteSpan signer_cert_subject(signer_cert_subject_buf);
    MutableByteSpan signer_cert_skid(signer_cert_skid_buf);
    Crypto::ExtractSubjectFromX509Cert(crl_signer_cert, signer_cert_subject);
    Crypto::ExtractSKIDFromX509Cert(crl_signer_cert, signer_cert_skid);
    if (!signer_cert_subject.data_equal(ByteSpan((uint8_t *)crl->issuer_raw.p, crl->issuer_raw.len))) {
        ESP_LOGE(TAG, "The CRLSignerCertificate is not the issuer of CRL");
        return false;
    }
    if (!check_crl_akid(crl, signer_cert_skid)) {
        ESP_LOGE(TAG, "The CRLSignerCertificate is not the issuer of CRL");
        return false;
    }
    return true;
}

bool is_da_cert_serial_number_revoked(const ByteSpan &da_cert, const mbedtls_x509_crl *crl)
{
    uint8_t serialNumberBuf[Crypto::kMaxCertificateSerialNumberLength] = {0};
    MutableByteSpan serialNumber(serialNumberBuf);
    Crypto::ExtractSerialNumberFromX509Cert(da_cert, serialNumber);
    for (const mbedtls_x509_crl_entry *entry = &crl->entry; entry != NULL; entry = entry->next) {
        // If the certificate's serial number is in the CRL's list and current time is after the revocation date,
        // the certificate is revoked
        if (serialNumber.data_equal(ByteSpan(entry->serial.p, entry->serial.len))) {
            mbedtls_x509_time current_time;
            get_current_time(current_time);
            if (mbedtls_x509_time_cmp(&current_time, &entry->revocation_date) >= 0) {
                return true;
            } else {
                break;
            }
        }
    }
    return false;
}
} // namespace attestation_verification
} // namespace controller
} // namespace esp_matter
