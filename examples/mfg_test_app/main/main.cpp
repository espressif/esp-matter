/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_flash_encrypt.h>
#include <esp_log.h>
#include <esp_matter_providers.h>
#include <esp_random.h>
#include <esp_secure_boot.h>
#include <nvs_flash.h>

#include <crypto/CHIPCryptoPAL.h>
#include <credentials/attestation_verifier/DeviceAttestationVerifier.h>
#include <credentials/CHIPCert.h>
#include <lib/support/Span.h>
#include <lib/core/CHIPError.h>
#include <lib/support/logging/CHIPLogging.h>

using namespace chip;
using namespace chip::Crypto;
using namespace chip::Credentials;

namespace {
const char *TAG = "MFG-TEST-APP";

uint8_t s_dac_cert_buffer[kMaxDERCertLength];  // 600 bytes
uint8_t s_pai_cert_buffer[kMaxDERCertLength];  // 600 bytes
uint8_t s_paa_cert_buffer[kMaxDERCertLength];  // 600 bytes

extern const uint8_t paa_cert_start[] asm("_binary_paa_cert_der_start");
extern const uint8_t paa_cert_end[]   asm("_binary_paa_cert_der_end");

MutableByteSpan paa_span;
MutableByteSpan pai_span;
MutableByteSpan dac_span;

uint8_t s_garbage_buffer[128];

CHIP_ERROR read_certs_in_spans()
{
    // DAC Provider implementation
    DeviceAttestationCredentialsProvider * dac_provider = GetDeviceAttestationCredentialsProvider();
    VerifyOrReturnError(dac_provider, CHIP_ERROR_INTERNAL, ESP_LOGE(TAG, "ERROR: Failed to get the DAC provider impl"));

    // Read DAC
    dac_span = MutableByteSpan(s_dac_cert_buffer);
    CHIP_ERROR err = dac_provider->GetDeviceAttestationCert(dac_span);
    VerifyOrReturnError(err == CHIP_NO_ERROR, err, ESP_LOGE(TAG, "ERROR: Failed to read the DAC, %" CHIP_ERROR_FORMAT, err.Format()));

    // Read PAI
    pai_span = MutableByteSpan(s_pai_cert_buffer);
    err = dac_provider->GetProductAttestationIntermediateCert(pai_span);
    VerifyOrReturnError(err == CHIP_NO_ERROR, err, ESP_LOGE(TAG, "ERROR: Failed to read the PAI Certificate %" CHIP_ERROR_FORMAT, err.Format()));

    // Read PAA
    uint16_t paa_len = paa_cert_end - paa_cert_start;
    memcpy(s_paa_cert_buffer, paa_cert_start, paa_len);
    paa_span = MutableByteSpan(s_paa_cert_buffer, paa_len);

    return CHIP_NO_ERROR;
}

CHIP_ERROR dump_cert_details(const char *type, ByteSpan cert_span)
{
    ESP_LOGI(TAG, "---------- %s ----------", type);

    // Get VID, PID from the certificate
    AttestationCertVidPid vidpid;
    CHIP_ERROR err = ExtractVIDPIDFromX509Cert(cert_span, vidpid);
    VerifyOrReturnError(err == CHIP_NO_ERROR, err, ESP_LOGE(TAG, "ERROR: Failed to extract VID and PID, error: %" CHIP_ERROR_FORMAT, err.Format()));

    if (vidpid.mVendorId.HasValue()) {
        ESP_LOGI(TAG, "Vendor ID: 0x%04X", vidpid.mVendorId.Value());
        assert(CONFIG_DEVICE_VENDOR_ID == vidpid.mVendorId.Value());
    } else if (strncmp(type, "PAA", sizeof("PAA")) != 0) {
        ESP_LOGE(TAG, "ERROR: Vendor ID: Unspecified");
    }

    if (vidpid.mProductId.HasValue()) {
        ESP_LOGI(TAG, "Product ID: 0x%04X", vidpid.mProductId.Value());
        assert(CONFIG_DEVICE_PRODUCT_ID == vidpid.mProductId.Value());
    } else if ((strncmp(type, "PAA", sizeof("PAA")) != 0) && (strncmp(type, "PAI", sizeof("PAI")) != 0)) {
        ESP_LOGE(TAG, "ERROR: Product ID: Unspecified");
    }

    // Get Public key from the certificate
    P256PublicKey pubkey;
    err = ExtractPubkeyFromX509Cert(cert_span, pubkey);
    VerifyOrReturnError(err == CHIP_NO_ERROR, err, ESP_LOGE(TAG, "ERROR: Failed to extract public key, error: %" CHIP_ERROR_FORMAT, err.Format()));

    // Print public key
    ESP_LOGI(TAG, "Public Key encoded as hex string:");
    for (uint8_t i = 0; i < pubkey.Length(); i++) {
        printf("%02x", pubkey.ConstBytes()[i]);
    }
    printf("\n\n");

    // Get AKID from the certificate
    uint8_t akid_buffer[64];
    MutableByteSpan akid_span(akid_buffer);
    err = ExtractAKIDFromX509Cert(cert_span, akid_span);
    VerifyOrReturnError(err == CHIP_NO_ERROR, err, ESP_LOGE(TAG, "ERROR: Failed to extract AKID, error: %" CHIP_ERROR_FORMAT, err.Format()));

    // Print AKID
    ESP_LOGI(TAG, "X509v3 Authority Key Identifier:");
    printf("%02x", akid_span.data()[0]);
    for (uint8_t i = 1; i < akid_span.size(); i++) {
        printf(":%02X", akid_span.data()[i]);
    }
    printf("\n\n");

    // Get SKID from the certificate
    uint8_t skid_buffer[64];
    MutableByteSpan skid_span(skid_buffer);
    err = ExtractSKIDFromX509Cert(cert_span, skid_span);
    VerifyOrReturnError(err == CHIP_NO_ERROR, err, ESP_LOGE(TAG, "ERROR: Failed to extract SKID, error: %" CHIP_ERROR_FORMAT, err.Format()));

    // Print SKID
    ESP_LOGI(TAG, "X509v3 Subject Key Identifier:");
    printf("%02x", skid_span.data()[0]);
    for (uint8_t i = 1; i < skid_span.size(); i++) {
        printf(":%02X", skid_span.data()[i]);
    }
    printf("\n\n");

    ESP_LOGI(TAG, "------------------------------");
    return CHIP_NO_ERROR;
}

CHIP_ERROR test_dac(ByteSpan dac)
{
    // DAC Provider implementation
    DeviceAttestationCredentialsProvider * dac_provider = GetDeviceAttestationCredentialsProvider();
    VerifyOrReturnError(dac_provider, CHIP_ERROR_INTERNAL, ESP_LOGE(TAG, "ERROR: Failed to get the DAC provider impl"));

    // Get Public key from the certificate
    P256PublicKey pubkey;
    CHIP_ERROR err = ExtractPubkeyFromX509Cert(dac, pubkey);
    VerifyOrReturnError(err == CHIP_NO_ERROR, err, ESP_LOGE(TAG, "ERROR: Failed to get DAC public key, error: %" CHIP_ERROR_FORMAT, err.Format()));

    // Garbage
    esp_fill_random(s_garbage_buffer, sizeof(s_garbage_buffer));
    ByteSpan mts_span(s_garbage_buffer);

    // signature
    P256ECDSASignature signature;
    MutableByteSpan signature_span{ signature.Bytes(), signature.Capacity() };

    // Generate attestation signature
    err = dac_provider->SignWithDeviceAttestationKey(mts_span, signature_span);
    VerifyOrReturnError(err == CHIP_NO_ERROR, err, ESP_LOGE(TAG, "ERROR: Failed to sign the message with DAC key, error: %" CHIP_ERROR_FORMAT, err.Format()));

    ESP_LOGI(TAG, "Message signed with DAC key: OK");
    ESP_LOG_BUFFER_HEX(TAG, signature_span.data(), signature_span.size());

    P256ECDSASignature signature_to_verify;

    ReturnErrorOnFailure(signature_to_verify.SetLength(signature_span.size()));
    memcpy(signature_to_verify.Bytes(), signature_span.data(), signature_span.size());

    err = pubkey.ECDSA_validate_msg_signature(s_garbage_buffer, sizeof(s_garbage_buffer), signature_to_verify);
    VerifyOrReturnError(err == CHIP_NO_ERROR, err, ESP_LOGE(TAG, "ERROR: Failed to validate signature, error: %" CHIP_ERROR_FORMAT, err.Format()));

    ESP_LOGI(TAG, "Signature Verification: OK");
    return CHIP_NO_ERROR;
}

AttestationVerificationResult MapError(CertificateChainValidationResult certificateChainValidationResult)
{
    switch (certificateChainValidationResult)
    {
    case CertificateChainValidationResult::kRootFormatInvalid:
        return AttestationVerificationResult::kPaaFormatInvalid;

    case CertificateChainValidationResult::kRootArgumentInvalid:
        return AttestationVerificationResult::kPaaArgumentInvalid;

    case CertificateChainValidationResult::kICAFormatInvalid:
        return AttestationVerificationResult::kPaiFormatInvalid;

    case CertificateChainValidationResult::kICAArgumentInvalid:
        return AttestationVerificationResult::kPaiArgumentInvalid;

    case CertificateChainValidationResult::kLeafFormatInvalid:
        return AttestationVerificationResult::kDacFormatInvalid;

    case CertificateChainValidationResult::kLeafArgumentInvalid:
        return AttestationVerificationResult::kDacArgumentInvalid;

    case CertificateChainValidationResult::kChainInvalid:
        return AttestationVerificationResult::kDacSignatureInvalid;

    case CertificateChainValidationResult::kNoMemory:
        return AttestationVerificationResult::kNoMemory;

    case CertificateChainValidationResult::kInternalFrameworkError:
        return AttestationVerificationResult::kInternalError;

    default:
        return AttestationVerificationResult::kInternalError;
    }
}

bool test_cert_chain(ByteSpan paa, ByteSpan pai, ByteSpan dac)
{
    AttestationVerificationResult err = AttestationVerificationResult::kSuccess;

    // Validate Proper Certificate Format
    VerifyOrExit(VerifyAttestationCertificateFormat(paa, AttestationCertType::kPAA) == CHIP_NO_ERROR,
                 err = AttestationVerificationResult::kPaaFormatInvalid);
    VerifyOrExit(VerifyAttestationCertificateFormat(pai, AttestationCertType::kPAI) == CHIP_NO_ERROR,
                 err = AttestationVerificationResult::kPaiFormatInvalid);
    VerifyOrExit(VerifyAttestationCertificateFormat(dac, AttestationCertType::kDAC) == CHIP_NO_ERROR,
                 err = AttestationVerificationResult::kDacFormatInvalid);

    // Verify that VID and PID in the certificates match.
    {
        AttestationCertVidPid dacVidPid;
        AttestationCertVidPid paiVidPid;
        AttestationCertVidPid paaVidPid;

        VerifyOrExit(ExtractVIDPIDFromX509Cert(dac, dacVidPid) == CHIP_NO_ERROR,
                     err = AttestationVerificationResult::kDacFormatInvalid);
        VerifyOrExit(ExtractVIDPIDFromX509Cert(pai, paiVidPid) == CHIP_NO_ERROR,
                     err = AttestationVerificationResult::kPaiFormatInvalid);
        VerifyOrExit(ExtractVIDPIDFromX509Cert(paa, paaVidPid) == CHIP_NO_ERROR,
                     err = AttestationVerificationResult::kPaaFormatInvalid);

        VerifyOrExit(dacVidPid.mVendorId.HasValue() && dacVidPid.mVendorId == paiVidPid.mVendorId,
                     err = AttestationVerificationResult::kDacVendorIdMismatch);

        if (paaVidPid.mVendorId.HasValue())
        {
            VerifyOrExit(dacVidPid.mVendorId == paaVidPid.mVendorId,
                         err = AttestationVerificationResult::kPaiVendorIdMismatch);
        }

        if (paiVidPid.mProductId.HasValue())
        {
            VerifyOrExit(dacVidPid.mProductId == paiVidPid.mProductId,
                         err = AttestationVerificationResult::kDacProductIdMismatch);
        }

        VerifyOrExit(!paaVidPid.mProductId.HasValue(), err = AttestationVerificationResult::kPaaFormatInvalid);
    }

    // Validate certificate chain.
    CertificateChainValidationResult chainValidationResult;
    VerifyOrExit(ValidateCertificateChain(paa.data(), paa.size(), pai.data(), pai.size(), dac.data(), dac.size(),
                                          chainValidationResult) == CHIP_NO_ERROR,
                 err = MapError(chainValidationResult));

exit:
    if (err != AttestationVerificationResult::kSuccess) {
        ESP_LOGE(TAG, "ERROR: Certificates Chain Validation Failed with Error Code: %d\n", static_cast<int>(err));
        ESP_LOGE(TAG, "Find the error enum here:https://github.com/project-chip/connectedhomeip/blob/c5216d1/src/credentials/attestation_verifier/DeviceAttestationVerifier.h#L30");
        return false;
    } else {
        ESP_LOGI(TAG, "DAC->PAI->PAA Certificate chain validation: OK");
    }

    return true;
}

void test_security_bits()
{
    if (esp_flash_encryption_cfg_verify_release_mode()) {
        ESP_LOGI(TAG, "Flash encryption in release mode: OK");
    }

    if (esp_secure_boot_cfg_verify_release_mode()) {
        ESP_LOGI(TAG, "Secure Boot in release mode: OK");
    }
}

}

extern "C" void app_main()
{
    // Setup providers based on configuration options
    esp_matter::setup_providers();

    // read PAA, PAI, and DAC in spans
    CHIP_ERROR err = read_certs_in_spans();
    VerifyOrReturn(err == CHIP_NO_ERROR, ESP_LOGE(TAG, "ERROR: Reading certificates failed, error: %" CHIP_ERROR_FORMAT, err.Format()));

    // Dump PAI details
    err = dump_cert_details("PAI", pai_span);
    VerifyOrReturn(err == CHIP_NO_ERROR, ESP_LOGE(TAG, "ERROR: Failed to dump PAI certificate details, error: %" CHIP_ERROR_FORMAT, err.Format()));

    // Dump DAC details
    err = dump_cert_details("DAC", dac_span);
    VerifyOrReturn(err == CHIP_NO_ERROR, ESP_LOGE(TAG, "ERROR: Failed to dump DAC certificate details, error: %" CHIP_ERROR_FORMAT, err.Format()));

    // Sign the message with DAC key and verify with public key in DAC certificate
    err = test_dac(dac_span);
    VerifyOrReturn(err == CHIP_NO_ERROR, ESP_LOGE(TAG, "ERROR: Failed to Sign and Verify using DAC keypair, error: %" CHIP_ERROR_FORMAT, err.Format()));

    // Test DAC -> PAI -> PAA chain validation
    bool status = test_cert_chain(paa_span, pai_span, dac_span);
    VerifyOrReturn(status, ESP_LOGE(TAG, "ERROR: Failed to validate attestation cert chain (DAC -> PAI -> PAA)"));

    test_security_bits();
}
