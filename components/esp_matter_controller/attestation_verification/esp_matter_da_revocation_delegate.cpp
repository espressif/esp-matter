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

#include <cstdio>
#include <cstring>
#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_matter_mem.h>
#include <esp_spiffs.h>
#include <json_parser.h>

#include <attestation_verification_utils.h>
#include <esp_matter_da_revocation_check.h>
#include <esp_matter_da_revocation_delegate.h>
#include <http_client_get.h>

#include <credentials/attestation_verifier/DeviceAttestationVerifier.h>
#include <crypto/CHIPCryptoPAL.h>
#include <lib/support/Span.h>

using namespace esp_matter::controller::attestation_verification;

const char TAG[] = "da_revocation";

namespace chip {
namespace Credentials {

#ifdef CONFIG_DCL_REVOCATION_POINTS_REVOKED_DAC_CHAIN_CHECK

typedef struct parse_revocation_points_ctx {
    esp_err_t err = ESP_OK;
    MutableByteSpan &out_crl_signer_cert;
    MutableByteSpan &out_crl_signer_delegator;
    MutableCharSpan &out_crl_url;
} parse_revocation_points_ctx_t;

static void parse_revocation_points_response(http_resp_t *resp, void *ctx)
{
    parse_revocation_points_ctx_t *resp_ctx = (parse_revocation_points_ctx_t *)ctx;
    if (resp_ctx == nullptr) {
        return;
    }
    ESP_LOGD(TAG, "response data: %s", (char *)resp->data);
    resp_ctx->err = ESP_FAIL;
    jparse_ctx_t jctx;
    if (json_parse_start(&jctx, (char *)resp->data, resp->size) != 0) {
        ESP_LOGE(TAG, "Failed to parse the http response json on json_parse_start");
        return;
    }
    if (json_obj_get_object(&jctx, "pkiRevocationDistributionPointsByIssuerSubjectKeyID") == 0) {
        int points_num = 0;
        bool get_signer_cert = false;
        // TODO: There might be multiple points
        if (json_obj_get_array(&jctx, "points", &points_num) == 0 && points_num == 1) {
            if (json_arr_get_object(&jctx, 0) == 0) {
                int crl_signer_cert_len, crl_signer_delegator_len, crl_url_len;
                if (json_obj_get_strlen(&jctx, "crlSignerCertificate", &crl_signer_cert_len) == 0) {
                    char *crl_signer_cert_pem = (char *)esp_matter_mem_calloc(crl_signer_cert_len + 1, 1);
                    if (crl_signer_cert_pem) {
                        if (json_obj_get_string(&jctx, "crlSignerCertificate", crl_signer_cert_pem,
                                                crl_signer_cert_len + 1) == 0) {
                            remove_backslash_n(crl_signer_cert_pem);
                            size_t der_cert_len = resp_ctx->out_crl_signer_cert.size();
                            if (convert_pem_to_der(crl_signer_cert_pem, resp_ctx->out_crl_signer_cert.data(),
                                                   &der_cert_len) == ESP_OK) {
                                resp_ctx->out_crl_signer_cert.reduce_size(der_cert_len);
                                get_signer_cert = true;
                            }
                        }
                        esp_matter_mem_free(crl_signer_cert_pem);
                    }
                }
                if (json_obj_get_strlen(&jctx, "dataURL", &crl_url_len) == 0) {
                    if (crl_url_len < resp_ctx->out_crl_url.size()) {
                        if (json_obj_get_string(&jctx, "dataURL", resp_ctx->out_crl_url.data(),
                                                resp_ctx->out_crl_url.size()) == 0) {
                            if (get_signer_cert) {
                                resp_ctx->err = ESP_OK;
                            }
                        }
                    }
                }
                if (json_obj_get_strlen(&jctx, "crlSignerDelegator", &crl_signer_delegator_len) == 0) {
                    if (crl_signer_delegator_len == 0) {
                        resp_ctx->out_crl_signer_delegator.reduce_size(0);
                    } else {
                        char *crl_signer_delegator_pem = (char *)esp_matter_mem_calloc(crl_signer_delegator_len, 1);
                        if (crl_signer_delegator_pem) {
                            if (json_obj_get_string(&jctx, "crlSignerDelegator", crl_signer_delegator_pem,
                                                    crl_signer_delegator_len) == 0) {
                                remove_backslash_n(crl_signer_delegator_pem);
                                size_t der_cert_len = resp_ctx->out_crl_signer_cert.size();
                                if (convert_pem_to_der(crl_signer_delegator_pem,
                                                       resp_ctx->out_crl_signer_delegator.data(),
                                                       &der_cert_len) == ESP_OK) {
                                    resp_ctx->out_crl_signer_delegator.reduce_size(der_cert_len);
                                }
                            }
                            esp_matter_mem_free(crl_signer_delegator_pem);
                        }
                    }
                }
                json_arr_leave_object(&jctx);
            }
            json_obj_leave_array(&jctx);
        }
        json_obj_leave_object(&jctx);
    }
    json_parse_end(&jctx);
}

typedef struct download_crl_ctx {
    esp_err_t err = ESP_OK;
    MutableByteSpan &out_crl;
} download_crl_ctx_t;

static void download_crl(http_resp_t *resp, void *ctx)
{
    download_crl_ctx_t *resp_ctx = (download_crl_ctx_t *)ctx;
    if (!resp_ctx) {
        return;
    }
    if (resp_ctx->out_crl.size() < resp->size) {
        resp_ctx->err = ESP_ERR_NO_MEM;
        return;
    }
    memcpy(resp_ctx->out_crl.data(), resp->data, resp->size);
    resp_ctx->out_crl.reduce_size(resp->size);
    resp_ctx->err = ESP_OK;
}

void dcl_revocation_point_da_revocation_delegate::CheckForRevokedDACChain(
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

bool dcl_revocation_point_da_revocation_delegate::IsCertRevoked(const ByteSpan &certDer, bool isPAI)
{
    uint8_t da_cert_akid_buf[Crypto::kAuthorityKeyIdentifierLength];
    MutableByteSpan da_cert_akid(da_cert_akid_buf);
    Crypto::ExtractAKIDFromX509Cert(certDer, da_cert_akid);
    uint8_t crl_signer_cert_buf[Crypto::kMax_x509_Certificate_Length];
    MutableByteSpan crl_signer_cert(crl_signer_cert_buf);
    uint8_t crl_signer_delegator_buf[Crypto::kMax_x509_Certificate_Length];
    MutableByteSpan crl_signer_delegator(crl_signer_delegator_buf);
    char crl_url_buf[k_crl_url_max_len];
    MutableCharSpan crl_url(crl_url_buf);
    uint8_t crl_der_buf[k_crl_max_len];
    MutableByteSpan crl_der(crl_der_buf);
    if (fetch_revocation_set_from_dcl(da_cert_akid, crl_signer_cert, crl_signer_delegator, crl_url) != ESP_OK) {
        ESP_LOGI(TAG, "Cannot fetch revocation set from DCL with the issuer SKID");
        return false;
    }
    if (fetch_crl(crl_url, crl_der) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to fetch CRL with the URL in DCL");
        return false;
    }
    mbedtls_x509_crl crl;
    mbedtls_x509_crl_init(&crl);
    int ret = mbedtls_x509_crl_parse(&crl, crl_der.data(), crl_der.size());
    if (ret == 0) {
        // Ensure that the CRL Signer Certificate is the issuer of the CRL.
        if (check_crl_signer_cert(&crl, crl_signer_cert)) {
            // Check the CRL Signer Certificate or CRL Signer Delegator with the DA certificate
            if (cross_validate_cert(isPAI, certDer, crl_signer_cert, crl_signer_delegator)) {
                // Check whether the serial number of the DA certificate is revoked in the CRL
                return is_da_cert_serial_number_revoked(certDer, &crl);
            }
        }
    } else {
        ESP_LOGE(TAG, "Failed to parse the downloaded CRL");
    }
    return false;
}

esp_err_t dcl_revocation_point_da_revocation_delegate::fetch_revocation_set_from_dcl(
    const ByteSpan &issuer_skid, MutableByteSpan &crl_signer_cert, MutableByteSpan &crl_signer_delegator,
    MutableCharSpan &crl_url)
{
    ESP_RETURN_ON_FALSE(issuer_skid.size() == Crypto::kSubjectKeyIdentifierLength, ESP_ERR_INVALID_ARG, TAG,
                        "Invalid Issuer Subject Key Identifier");
    char url[128] = {0};
    int offset = snprintf(url, sizeof(url), "%s/dcl/pki/revocation-points/", m_dcl_net_base_url);
    for (size_t i = 0; i < issuer_skid.size(); ++i) {
        offset += snprintf(url + offset, sizeof(url) - offset, "%02X", issuer_skid[i]);
    }
    ESP_LOGI(TAG, "Query revocation-points from %s", url);

    char *header = strdup("accept: application/json");
    if (!header) {
        return ESP_ERR_NO_MEM;
    }
    parse_revocation_points_ctx_t ctx = {
        .out_crl_signer_cert = crl_signer_cert,
        .out_crl_signer_delegator = crl_signer_delegator,
        .out_crl_url = crl_url,
    };
    esp_err_t err = http_send_get_request(url, &header, 1, parse_revocation_points_response, (void *)&ctx);
    free(header);
    if (err != ESP_OK) {
        return err;
    }
    return ctx.err;
}

esp_err_t dcl_revocation_point_da_revocation_delegate::fetch_crl(const CharSpan &crl_url, MutableByteSpan &crl)
{
    ESP_RETURN_ON_FALSE(crl_url.size() > 0, ESP_ERR_INVALID_ARG, TAG, "Invalid CRL URL");
    download_crl_ctx_t ctx = {
        .out_crl = crl,
    };
    ESP_LOGI(TAG, "Query CRL from %s", crl_url.data());
    esp_err_t err = http_send_get_request(crl_url.data(), nullptr, 0, download_crl, (void *)&ctx);
    if (err != ESP_OK) {
        return err;
    }
    return ctx.err;
}
#endif // CONFIG_DCL_REVOCATION_POINTS_REVOKED_DAC_CHAIN_CHECK

#ifdef CONFIG_CUSTOM_REVOKED_DAC_CHAIN_CHECK
static DeviceAttestationRevocationDelegate *s_custom_da_revocation_delegate = nullptr;

void set_custom_da_revocation_delegate(DeviceAttestationRevocationDelegate *delegate)
{
    s_custom_da_revocation_delegate = delegate;
}
#endif // CONFIG_CUSTOM_REVOKED_DAC_CHAIN_CHECK

DeviceAttestationRevocationDelegate *get_da_revocation_delegate()
{
#ifdef CONFIG_DCL_REVOCATION_POINTS_REVOKED_DAC_CHAIN_CHECK
    return &dcl_revocation_point_da_revocation_delegate::get_instance();
#elif defined(CONFIG_CUSTOM_REVOKED_DAC_CHAIN_CHECK)
    return s_custom_da_revocation_delegate;
#endif
    return nullptr;
}

} // namespace Credentials
} // namespace chip
