/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "json_set_da_revocation_delegate.h"

#include <cJSON.h>
#include <esp_matter_da_revocation_check.h>
#include <string.h>

#include <crypto/CHIPCryptoPAL.h>
#include <lib/support/Base64.h>
#include <lib/support/BytesToHex.h>
#include <lib/support/Span.h>

using namespace esp_matter::controller::attestation_verification;

namespace chip {
namespace Credentials {

// Returns the string value of item, or nullptr if item is not a string or is longer than max_len.
static const char *string_value(const cJSON *item, size_t max_len)
{
    const char *str = cJSON_GetStringValue(item);
    return (str && strlen(str) <= max_len) ? str : nullptr;
}

static bool check_issuer_in_revocation_set(const cJSON *set, const ByteSpan &da_cert)
{
    uint8_t akidBuf[Crypto::kAuthorityKeyIdentifierLength] = {0};
    uint8_t issuerBuf[Crypto::kMaxCertificateDistinguishedNameLength] = {0};
    MutableByteSpan akid(akidBuf);
    MutableByteSpan issuer(issuerBuf);
    Crypto::ExtractAKIDFromX509Cert(da_cert, akid);
    Crypto::ExtractIssuerFromX509Cert(da_cert, issuer);

    const char *issuer_skid_str = string_value(cJSON_GetObjectItemCaseSensitive(set, "issuer_subject_key_id"),
                                               Crypto::kAuthorityKeyIdentifierLength * 2);
    if (!issuer_skid_str) {
        return false;
    }
    uint8_t issuer_skid[Crypto::kAuthorityKeyIdentifierLength];
    if (Encoding::HexToBytes(issuer_skid_str, strlen(issuer_skid_str), issuer_skid, sizeof(issuer_skid)) !=
            sizeof(issuer_skid)) {
        return false;
    }
    if (!akid.data_equal(ByteSpan(issuer_skid))) {
        return false;
    }
    const char *issuer_name_base64 = string_value(cJSON_GetObjectItemCaseSensitive(set, "issuer_name"),
                                                  BASE64_ENCODED_LEN(Crypto::kMaxCertificateDistinguishedNameLength));
    if (!issuer_name_base64) {
        return false;
    }
    uint8_t issuer_name[Crypto::kMaxCertificateDistinguishedNameLength];
    uint16_t issuer_len = Base64Decode(issuer_name_base64, strlen(issuer_name_base64), issuer_name);
    if (issuer_len == UINT16_MAX) {
        return false;
    }
    if (!issuer.data_equal(ByteSpan(issuer_name, issuer_len))) {
        return false;
    }
    return true;
}

static void get_crl_signer_cert_and_delegator(const cJSON *set, MutableByteSpan &crl_signer_cert,
                                              MutableByteSpan &crl_signer_delegator)
{
    const size_t max_len = BASE64_ENCODED_LEN(Crypto::kMax_x509_Certificate_Length);
    const char *cert_base64 = string_value(cJSON_GetObjectItemCaseSensitive(set, "crl_signer_cert"), max_len);
    if (cert_base64) {
        uint16_t cert_len = Base64Decode(cert_base64, strlen(cert_base64), crl_signer_cert.data());
        if (cert_len != UINT16_MAX) {
            crl_signer_cert.reduce_size(cert_len);
        }
    }
    const char *delegator_base64 = string_value(cJSON_GetObjectItemCaseSensitive(set, "crl_signer_delegator"),
                                                max_len);
    if (delegator_base64) {
        uint16_t cert_len = Base64Decode(delegator_base64, strlen(delegator_base64), crl_signer_delegator.data());
        if (cert_len != UINT16_MAX) {
            crl_signer_delegator.reduce_size(cert_len);
        } else {
            crl_signer_delegator.reduce_size(0);
        }
    } else {
        crl_signer_delegator.reduce_size(0);
    }
}

static bool check_serial_number(const cJSON *set, const ByteSpan &da_cert)
{
    uint8_t serial_num_buf[Crypto::kMaxCertificateSerialNumberLength] = {0};
    MutableByteSpan serial_num(serial_num_buf);
    Crypto::ExtractSerialNumberFromX509Cert(da_cert, serial_num);

    const cJSON *revoked = cJSON_GetObjectItemCaseSensitive(set, "revoked_serial_numbers");
    if (!cJSON_IsArray(revoked)) {
        return false;
    }
    const cJSON *item = nullptr;
    cJSON_ArrayForEach(item, revoked) {
        const char *serial_number_str = string_value(item, Crypto::kMaxCertificateSerialNumberLength * 2);
        if (serial_number_str) {
            uint8_t serial_number[Crypto::kMaxCertificateSerialNumberLength];
            size_t serial_number_len = Encoding::HexToBytes(serial_number_str, strlen(serial_number_str), serial_number,
                                                            sizeof(serial_number));
            if (serial_num.data_equal(ByteSpan(serial_number, serial_number_len))) {
                return true;
            }
        }
    }
    return false;
}

bool json_set_da_revocation_delegate::IsCertRevoked(const ByteSpan &certDer, bool isPAI)
{
    bool revoked = false;
    cJSON *root = cJSON_ParseWithLength(m_revocation_json_set_start, m_revocation_json_set_len);
    if (cJSON_IsArray(root)) {
        const cJSON *set = nullptr;
        cJSON_ArrayForEach(set, root) {
            // Find the RevocationSet with the certificate's AKID and issuer name
            if (!check_issuer_in_revocation_set(set, certDer)) {
                continue;
            }
            uint8_t crl_signer_cert_buf[Crypto::kMax_x509_Certificate_Length];
            MutableByteSpan crl_signer_cert(crl_signer_cert_buf);
            uint8_t crl_signer_delegator_buf[Crypto::kMax_x509_Certificate_Length];
            MutableByteSpan crl_signer_delegator(crl_signer_delegator_buf);

            get_crl_signer_cert_and_delegator(set, crl_signer_cert, crl_signer_delegator);
            // Check certificate's serial number are in the revoked serial number
            if (cross_validate_cert(isPAI, certDer, crl_signer_cert, crl_signer_delegator) &&
                    check_serial_number(set, certDer)) {
                revoked = true;
                break;
            }
        }
    }
    cJSON_Delete(root);
    return revoked;
}

void json_set_da_revocation_delegate::CheckForRevokedDACChain(
    const DeviceAttestationVerifier::AttestationInfo &info,
    Callback::Callback<DeviceAttestationVerifier::OnAttestationInformationVerification> *onCompletion)
{
    AttestationVerificationResult attestationError = AttestationVerificationResult::kSuccess;
    if (IsCertRevoked(info.dacDerBuffer, false)) {
        attestationError = AttestationVerificationResult::kDacRevoked;
    }
    if (IsCertRevoked(info.paiDerBuffer, true)) {
        if (attestationError == AttestationVerificationResult::kDacRevoked) {
            attestationError = AttestationVerificationResult::kPaiAndDacRevoked;
        } else {
            attestationError = AttestationVerificationResult::kPaiRevoked;
        }
    }
    onCompletion->mCall(onCompletion->mContext, info, attestationError);
}
} // namespace Credentials
} // namespace chip
