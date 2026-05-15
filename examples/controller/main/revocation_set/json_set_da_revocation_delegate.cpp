/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "json_set_da_revocation_delegate.h"

#include <esp_matter_da_revocation_check.h>
#include <json_parser.h>
#include <string.h>

#include <crypto/CHIPCryptoPAL.h>
#include <lib/support/Base64.h>
#include <lib/support/BytesToHex.h>
#include <lib/support/Span.h>

using namespace esp_matter::controller::attestation_verification;

namespace chip {
namespace Credentials {

static bool check_issuer_in_revocation_set(jparse_ctx_t &jctx, const ByteSpan &da_cert)
{
    uint8_t akidBuf[Crypto::kAuthorityKeyIdentifierLength] = {0};
    uint8_t issuerBuf[Crypto::kMaxCertificateDistinguishedNameLength] = {0};
    MutableByteSpan akid(akidBuf);
    MutableByteSpan issuer(issuerBuf);
    Crypto::ExtractAKIDFromX509Cert(da_cert, akid);
    Crypto::ExtractIssuerFromX509Cert(da_cert, issuer);

    char issuer_skid_str[Crypto::kAuthorityKeyIdentifierLength * 2 + 1];
    if (json_obj_get_string(&jctx, "issuer_subject_key_id", issuer_skid_str, sizeof(issuer_skid_str)) != 0) {
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
    char issuer_name_base64[BASE64_ENCODED_LEN(Crypto::kMaxCertificateDistinguishedNameLength) + 1] = {0};
    if (json_obj_get_string(&jctx, "issuer_name", issuer_name_base64, sizeof(issuer_name_base64)) != 0) {
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

static void get_crl_signer_cert_and_delegator(jparse_ctx_t &jctx, MutableByteSpan &crl_signer_cert,
                                              MutableByteSpan &crl_signer_delegator)
{
    char cert_base64_buf[BASE64_ENCODED_LEN(Crypto::kMax_x509_Certificate_Length) + 1] = {0};
    if (json_obj_get_string(&jctx, "crl_signer_cert", cert_base64_buf, sizeof(cert_base64_buf)) == 0) {
        uint16_t cert_len = Base64Decode(cert_base64_buf, strlen(cert_base64_buf), crl_signer_cert.data());
        if (cert_len != UINT16_MAX) {
            crl_signer_cert.reduce_size(cert_len);
        }
    }
    memset(cert_base64_buf, 0, sizeof(cert_base64_buf));
    if (json_obj_get_string(&jctx, "crl_signer_delegator", cert_base64_buf, sizeof(cert_base64_buf)) == 0) {
        uint16_t cert_len = Base64Decode(cert_base64_buf, strlen(cert_base64_buf), crl_signer_delegator.data());
        if (cert_len != UINT16_MAX) {
            crl_signer_delegator.reduce_size(cert_len);
        } else {
            crl_signer_delegator.reduce_size(0);
        }
    } else {
        crl_signer_delegator.reduce_size(0);
    }
}

static bool check_serial_number(jparse_ctx_t &jctx, const ByteSpan &da_cert)
{
    uint8_t serial_num_buf[Crypto::kMaxCertificateSerialNumberLength] = {0};
    MutableByteSpan serial_num(serial_num_buf);
    Crypto::ExtractSerialNumberFromX509Cert(da_cert, serial_num);

    int revoked_num = 0;
    if (json_obj_get_array(&jctx, "revoked_serial_numbers", &revoked_num) != 0 || revoked_num <= 0) {
        return false;
    }
    for (int i = 0; i < revoked_num; ++i) {
        char serial_number_str[Crypto::kMaxCertificateSerialNumberLength * 2 + 1];
        if (json_arr_get_string(&jctx, i, serial_number_str, sizeof(serial_number_str)) == 0) {
            uint8_t serial_number[Crypto::kMaxCertificateSerialNumberLength];
            size_t serial_number_len = Encoding::HexToBytes(serial_number_str, strlen(serial_number_str), serial_number,
                                                            sizeof(serial_number));
            if (serial_num.data_equal(ByteSpan(serial_number, serial_number_len))) {
                return true;
            }
        }
    }
    json_obj_leave_array(&jctx);
    return false;
}

bool json_set_da_revocation_delegate::IsCertRevoked(const ByteSpan &certDer, bool isPAI)
{
    jparse_ctx_t jctx;
    if (json_parse_start(&jctx, m_revocation_json_set_start, m_revocation_json_set_len) == 0) {
        for (int index = 0; json_arr_get_object(&jctx, index) == 0; ++index) {
            // Find the RevocationSet with the certificate's AKID and issuer name
            if (!check_issuer_in_revocation_set(jctx, certDer)) {
                json_arr_leave_object(&jctx);
                continue;
            }
            uint8_t crl_signer_cert_buf[Crypto::kMax_x509_Certificate_Length];
            MutableByteSpan crl_signer_cert(crl_signer_cert_buf);
            uint8_t crl_signer_delegator_buf[Crypto::kMax_x509_Certificate_Length];
            MutableByteSpan crl_signer_delegator(crl_signer_delegator_buf);

            get_crl_signer_cert_and_delegator(jctx, crl_signer_cert, crl_signer_delegator);
            if (cross_validate_cert(isPAI, certDer, crl_signer_cert, crl_signer_delegator)) {
                // Check certificate's serial number are in the revoked serial number
                if (check_serial_number(jctx, certDer)) {
                    json_parse_end(&jctx);
                    return true;
                }
            }
            json_arr_leave_object(&jctx);
        }
        json_parse_end(&jctx);
    }
    return false;
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
