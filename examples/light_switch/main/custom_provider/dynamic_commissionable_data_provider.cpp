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

#include <crypto/CHIPCryptoPAL.h>
#include <custom_provider/dynamic_commissionable_data_provider.h>
#include <esp_log.h>
#include <lib/support/Base64.h>
#include <platform/ESP32/ESP32Config.h>
#include <setup_payload/SetupPayload.h>

using namespace ::chip::DeviceLayer::Internal;
using namespace ::chip;

constexpr char *TAG = "custom_provider";

CHIP_ERROR dynamic_commissionable_data_provider::GetSetupDiscriminator(uint16_t &setupDiscriminator)
{
    setupDiscriminator = CONFIG_DYNAMIC_PASSCODE_PROVIDER_DISCRIMINATOR;
    return CHIP_NO_ERROR;
}

CHIP_ERROR dynamic_commissionable_data_provider::GetSpake2pIterationCount(uint32_t &iterationCount)
{
    iterationCount = CONFIG_DYNAMIC_PASSCODE_PROVIDER_ITERATIONS;
    return CHIP_NO_ERROR;
}

static bool is_valid_base64_str(const char *str)
{
    const char *base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    if (!str) {
        return false;
    }
    size_t len = strlen(str);
    if (len % 4 != 0) {
        return false;
    }
    size_t padding_len = 0;
    if (str[len - 1] == '=') {
        padding_len++;
        if (str[len - 2] == '=') {
            padding_len++;
        }
    }
    for (size_t i = 0; i < len - padding_len; ++i) {
        if (strchr(base64_chars, str[i]) == NULL) {
            return false;
        }
    }
    return true;
}

CHIP_ERROR dynamic_commissionable_data_provider::GetSpake2pSalt(MutableByteSpan &saltBuf)
{
    const char *saltB64 = CONFIG_DYNAMIC_PASSCODE_PROVIDER_SALT_BASE64;
    ReturnErrorCodeIf(!is_valid_base64_str(saltB64), CHIP_ERROR_INVALID_ARGUMENT);
    size_t saltB64Len = strlen(saltB64);
    uint8_t salt[chip::Crypto::kSpake2p_Max_PBKDF_Salt_Length];
    size_t saltLen = chip::Base64Decode32(saltB64, saltB64Len, salt);
    ReturnErrorCodeIf(saltLen < chip::Crypto::kSpake2p_Min_PBKDF_Salt_Length, CHIP_ERROR_INVALID_ARGUMENT);
    ReturnErrorCodeIf(saltLen > saltBuf.size(), CHIP_ERROR_BUFFER_TOO_SMALL);

    memcpy(saltBuf.data(), salt, saltLen);
    saltBuf.reduce_size(saltLen);
    return CHIP_NO_ERROR;
}

CHIP_ERROR dynamic_commissionable_data_provider::GetSpake2pVerifier(MutableByteSpan &verifierBuf, size_t &verifierLen)
{
    uint32_t setupPasscode = 0;
    uint32_t iterationCount = 0;
    uint8_t salt[Crypto::kSpake2p_Max_PBKDF_Salt_Length] = {0};
    chip::MutableByteSpan saltSpan(salt, Crypto::kSpake2p_Max_PBKDF_Salt_Length);
    ReturnErrorOnFailure(GetSetupPasscode(setupPasscode));
    ReturnErrorOnFailure(GetSpake2pIterationCount(iterationCount));
    ReturnErrorOnFailure(GetSpake2pSalt(saltSpan));
    chip::Crypto::Spake2pVerifier verifier;
    ReturnErrorOnFailure(verifier.Generate(iterationCount, saltSpan, setupPasscode));
    ReturnErrorOnFailure(verifier.Serialize(verifierBuf));
    verifierLen = verifierBuf.size();
    return CHIP_NO_ERROR;
}

CHIP_ERROR dynamic_commissionable_data_provider::GetSetupPasscode(uint32_t &setupPasscode)
{
    if (mSetupPasscode == 0) {
        ReturnErrorOnFailure(GenerateRandomPasscode(mSetupPasscode));
    }
    setupPasscode = mSetupPasscode;
    return CHIP_NO_ERROR;
}

CHIP_ERROR dynamic_commissionable_data_provider::GenerateRandomPasscode(uint32_t &passcode)
{
    ReturnErrorOnFailure(chip::Crypto::DRBG_get_bytes(reinterpret_cast<uint8_t *>(&passcode), sizeof(passcode)));
    // Passcode MUST be 1 to 99999998
    passcode = (passcode % chip::kSetupPINCodeMaximumValue) + 1;
    if (!chip::SetupPayload::IsValidSetupPIN(passcode)) {
        // if the generated passcode is invalid (11111111, 22222222, 33333333, 44444444, 55555555, 66666666,
        // 77777777, 88888888, 12345678, 87654321), increase it by 1 to make it valid.
        passcode = passcode + 1;
    }
    return CHIP_NO_ERROR;
}
