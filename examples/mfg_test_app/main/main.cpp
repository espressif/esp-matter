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
#include <platform/ESP32/ESP32SecureCertDataProvider.h>

using namespace chip;
using namespace chip::Crypto;
using namespace chip::Credentials;
using namespace chip::DeviceLayer;

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

// This reads the certificates DAC and PAI from the esp-secure-cert partition into global buffers.
// It also stores the PAA certificate provided by the user in the global buffer.
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

// This function prints following details from the certificate:
// - Vendor ID
// - Product ID
// - Public key as hex string
// - AKID as hex string
// - SKID as hex string
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

// Validates the DAC by signing a message with the DAC key and
// verifying the signature with the public key in the DAC certificate.
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

// Verifies DAC->PAI->PAA Certificate chain
// Validates VID/PID per Matter spec
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

// Verifies if the flash encryption and secure boot are enabled in release mode
void test_security_bits()
{
    if (esp_flash_encryption_cfg_verify_release_mode()) {
        ESP_LOGI(TAG, "Flash encryption in release mode: OK");
    }
    if (esp_secure_boot_cfg_verify_release_mode()) {
        ESP_LOGI(TAG, "Secure Boot in release mode: OK");
    }
}

// reads the commissionable data and unique id for rotating device id from the esp-secure-cert partition
// and prints them
void read_and_print_matter_unique_data_from_secure_cert_partition()
{
#if defined(CONFIG_SEC_CERT_COMMISSIONABLE_DATA_PROVIDER) && defined(CONFIG_SEC_CERT_DEVICE_INSTANCE_INFO_PROVIDER)

    uint16_t setup_discriminator;
    CHIP_ERROR err = GetCommissionableDataProvider()->GetSetupDiscriminator(setup_discriminator);
    VerifyOrReturn(err == CHIP_NO_ERROR,
        ESP_LOGE(TAG, "ERROR: Failed to get setup discriminator, error: %" CHIP_ERROR_FORMAT, err.Format()));
    ESP_LOGI(TAG, "Setup discriminator: %d", setup_discriminator);

    uint32_t spake2p_iteration_count;
    err = GetCommissionableDataProvider()->GetSpake2pIterationCount(spake2p_iteration_count);
    VerifyOrReturn(err == CHIP_NO_ERROR,
        ESP_LOGE(TAG, "ERROR: Failed to get spake2p iteration count, error: %" CHIP_ERROR_FORMAT, err.Format()));
    ESP_LOGI(TAG, "Spake2p iteration count: %d", spake2p_iteration_count);

    uint8_t spake2p_salt[kSpake2p_Max_PBKDF_Salt_Length];
    MutableByteSpan spake2p_salt_span(spake2p_salt);
    err = GetCommissionableDataProvider()->GetSpake2pSalt(spake2p_salt_span);
    VerifyOrReturn(err == CHIP_NO_ERROR,
        ESP_LOGE(TAG, "ERROR: Failed to get spake2p salt, error: %" CHIP_ERROR_FORMAT, err.Format()));
    printf("Spake2p salt: ");
    for (uint8_t i = 0; i < spake2p_salt_span.size(); i++) {
        printf("%02x", spake2p_salt_span.data()[i]);
    }
    printf("\n");

    uint8_t spake2p_verifier[kSpake2p_VerifierSerialized_Length];
    MutableByteSpan spake2p_verifier_span(spake2p_verifier);
    size_t spake2p_verifier_len = 0;
    err = GetCommissionableDataProvider()->GetSpake2pVerifier(spake2p_verifier_span, spake2p_verifier_len);
    VerifyOrReturn(err == CHIP_NO_ERROR,
        ESP_LOGE(TAG, "ERROR: Failed to get spake2p verifier, error: %" CHIP_ERROR_FORMAT, err.Format()));
    printf("Spake2p verifier: ");
    for (uint8_t i = 0; i < spake2p_verifier_span.size(); i++) {
        printf("%02x", spake2p_verifier_span.data()[i]);
    }
    printf("\n");

#if CHIP_DEVICE_CONFIG_ENABLE_DEVICE_INSTANCE_INFO_PROVIDER && CHIP_ENABLE_ROTATING_DEVICE_ID
    uint8_t unique_id_for_rotating_device_id[ConfigurationManager::kRotatingDeviceIDUniqueIDLength];
    MutableByteSpan unique_id_for_rotating_device_id_span(unique_id_for_rotating_device_id);
    err = GetDeviceInstanceInfoProvider()->GetRotatingDeviceIdUniqueId(unique_id_for_rotating_device_id_span);
    VerifyOrReturn(err == CHIP_NO_ERROR,
        ESP_LOGE(TAG, "ERROR: Failed to get unique id for rotating device id, error: %" CHIP_ERROR_FORMAT, err.Format()));
    printf("Unique id for rotating device id: ");
    for (uint8_t i = 0; i < unique_id_for_rotating_device_id_span.size(); i++) {
        printf("%02x", unique_id_for_rotating_device_id_span.data()[i]);
    }
    printf("\n");
#endif // CHIP_DEVICE_CONFIG_ENABLE_DEVICE_INSTANCE_INFO_PROVIDER && CHIP_ENABLE_ROTATING_DEVICE_ID

    uint8_t fixed_random1[ESP32SecureCertDataProvider::kFixedRandomValueLength];
    MutableByteSpan fixed_random1_span(fixed_random1);
    err = ESP32SecureCertDataProvider::GetFixedRandom1(fixed_random1_span);
    VerifyOrReturn(err == CHIP_NO_ERROR,
        ESP_LOGE(TAG, "ERROR: Failed to get fixed random 1, error: %" CHIP_ERROR_FORMAT, err.Format()));
    printf("Fixed random 1: ");
    for (uint8_t i = 0; i < fixed_random1_span.size(); i++) {
        printf("%02x", fixed_random1_span.data()[i]);
    }
    printf("\n");

    uint8_t fixed_random2[ESP32SecureCertDataProvider::kFixedRandomValueLength];
    MutableByteSpan fixed_random2_span(fixed_random2);
    err = ESP32SecureCertDataProvider::GetFixedRandom2(fixed_random2_span);
    VerifyOrReturn(err == CHIP_NO_ERROR,
        ESP_LOGE(TAG, "ERROR: Failed to get fixed random 2, error: %" CHIP_ERROR_FORMAT, err.Format()));
    printf("Fixed random 2: ");
    for (uint8_t i = 0; i < fixed_random2_span.size(); i++) {
        printf("%02x", fixed_random2_span.data()[i]);
    }
    printf("\n");

#endif // defined(CONFIG_SEC_CERT_COMMISSIONABLE_DATA_PROVIDER) && defined(CONFIG_SEC_CERT_DEVICE_INSTANCE_INFO_PROVIDER)
}

}

extern "C" void app_main()
{
    // Setup providers based on configuration options
    esp_matter::setup_providers();

    // read PAA, PAI, and DAC in spans
    CHIP_ERROR err = read_certs_in_spans();
    VerifyOrReturn(err == CHIP_NO_ERROR,
        ESP_LOGE(TAG, "ERROR: Reading certificates failed, error: %" CHIP_ERROR_FORMAT, err.Format()));

    // Dump PAI details
    err = dump_cert_details("PAI", pai_span);
    VerifyOrReturn(err == CHIP_NO_ERROR,
        ESP_LOGE(TAG, "ERROR: Failed to dump PAI certificate details, error: %" CHIP_ERROR_FORMAT, err.Format()));

    // Dump DAC details
    err = dump_cert_details("DAC", dac_span);
    VerifyOrReturn(err == CHIP_NO_ERROR,
        ESP_LOGE(TAG, "ERROR: Failed to dump DAC certificate details, error: %" CHIP_ERROR_FORMAT, err.Format()));

    // Sign the message with DAC key and verify with public key in DAC certificate
    err = test_dac(dac_span);
    VerifyOrReturn(err == CHIP_NO_ERROR,
        ESP_LOGE(TAG, "ERROR: Failed to Sign and Verify using DAC keypair, error: %" CHIP_ERROR_FORMAT, err.Format()));

    // Test DAC -> PAI -> PAA chain validation
    bool status = test_cert_chain(paa_span, pai_span, dac_span);
    VerifyOrReturn(status,
        ESP_LOGE(TAG, "ERROR: Failed to validate attestation cert chain (DAC -> PAI -> PAA)"));

    test_security_bits();
    read_and_print_matter_unique_data_from_secure_cert_partition();
}
