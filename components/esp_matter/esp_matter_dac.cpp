// Copyright 2022 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_matter_dac.h>
#include <esp_matter_factory.h>

#include <crypto/CHIPCryptoPAL.h>
#include <lib/core/CHIPError.h>
#include <lib/support/Span.h>

namespace chip {
namespace Credentials {
namespace esp {

namespace {

static constexpr uint32_t kDACPrivateKeySize = 32;
static constexpr uint32_t kDACPublicKeySize  = 65;

// TODO: This should be moved to a method of P256Keypair
CHIP_ERROR LoadKeypairFromRaw(ByteSpan privateKey, ByteSpan publicKey, Crypto::P256Keypair & keypair)
{
    Crypto::P256SerializedKeypair serializedKeypair;
    ReturnErrorOnFailure(serializedKeypair.SetLength(privateKey.size() + publicKey.size()));
    memcpy(serializedKeypair.Bytes(), publicKey.data(), publicKey.size());
    memcpy(serializedKeypair.Bytes() + publicKey.size(), privateKey.data(), privateKey.size());
    return keypair.Deserialize(serializedKeypair);
}

class ESPMatterDAC : public DeviceAttestationCredentialsProvider
{
public:
    CHIP_ERROR GetCertificationDeclaration(MutableByteSpan & outBuffer) override
    {
        size_t certSize = outBuffer.size();
        VerifyOrReturnError(esp_matter_factory_get_cert_declrn(outBuffer.data(), &certSize) == ESP_OK, CHIP_ERROR_READ_FAILED);
        outBuffer.reduce_size(certSize);
        return CHIP_NO_ERROR;
    }

    CHIP_ERROR GetFirmwareInformation(MutableByteSpan & out_firmware_info_buffer) override
    {
        // TODO: We need a real example FirmwareInformation to be populated.
        out_firmware_info_buffer.reduce_size(0);
        return CHIP_NO_ERROR;
    }

    CHIP_ERROR GetDeviceAttestationCert(MutableByteSpan & outBuffer) override
    {
        size_t dacCertSz = outBuffer.size();
        VerifyOrReturnError(esp_matter_factory_get_dac_cert(outBuffer.data(), &dacCertSz) == ESP_OK, CHIP_ERROR_READ_FAILED);
        outBuffer.reduce_size(dacCertSz);
        return CHIP_NO_ERROR;
    }

    CHIP_ERROR GetProductAttestationIntermediateCert(MutableByteSpan & outBuffer) override
    {
        size_t paiCertSz = outBuffer.size();
        VerifyOrReturnError(esp_matter_factory_get_pai_cert(outBuffer.data(), &paiCertSz) == ESP_OK, CHIP_ERROR_READ_FAILED);
        outBuffer.reduce_size(paiCertSz);
        return CHIP_NO_ERROR;
    }

    CHIP_ERROR SignWithDeviceAttestationKey(const ByteSpan & digestToSign, MutableByteSpan & outSignBuffer) override
    {
        Crypto::P256ECDSASignature signature;
        Crypto::P256Keypair keypair;

        VerifyOrReturnError(IsSpanUsable(outSignBuffer), CHIP_ERROR_INVALID_ARGUMENT);
        VerifyOrReturnError(IsSpanUsable(digestToSign), CHIP_ERROR_INVALID_ARGUMENT);
        VerifyOrReturnError(outSignBuffer.size() >= signature.Capacity(), CHIP_ERROR_BUFFER_TOO_SMALL);

        uint8_t privKeyBuf[kDACPrivateKeySize];
        uint8_t pubKeyBuf[kDACPublicKeySize];
        size_t privKeyLen = sizeof(privKeyBuf);
        size_t pubKeyLen  = sizeof(pubKeyBuf);

        VerifyOrReturnError(esp_matter_factory_get_dac_private_key(privKeyBuf, &privKeyLen) == ESP_OK, CHIP_ERROR_READ_FAILED);
        VerifyOrReturnError(esp_matter_factory_get_dac_public_key(pubKeyBuf, &pubKeyLen) == ESP_OK, CHIP_ERROR_READ_FAILED);

        // In a non-exemplary implementation, the public key is not needed here. It is used here merely because
        // Crypto::P256Keypair is only (currently) constructable from raw keys if both private/public keys are present.
        ReturnErrorOnFailure(LoadKeypairFromRaw(ByteSpan(privKeyBuf, privKeyLen), ByteSpan(pubKeyBuf, pubKeyLen), keypair));
        ReturnErrorOnFailure(keypair.ECDSA_sign_hash(digestToSign.data(), digestToSign.size(), signature));

        return CopySpanToMutableSpan(ByteSpan{ signature.ConstBytes(), signature.Length() }, outSignBuffer);
    }
};

} // namespace

DeviceAttestationCredentialsProvider * esp_matter_dac_provider_get(void)
{
    static ESPMatterDAC dacProvider;
    return &dacProvider;
}

} // namespace esp
} // namespace Credentials
} // namespace chip
