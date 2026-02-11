/*
 *
 *    Copyright (c) 2025 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include <crypto/CHIPCryptoPAL.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/ConfigurationManager.h>
#include <platform/ESP32/ESP32Config.h>
#include <platform/internal/GenericDeviceInstanceInfoProvider.h>

#include <ESP32ManufacturingDataProvider.h>

namespace chip {
namespace DeviceLayer {

const char * ESP32ManufacturingDataProvider::PrecedenceToString(Precedence precedence)
{
    switch (precedence) {
    case Precedence::kFactoryFirst:
        return "FactoryFirst";
    case Precedence::kSecureCertFirst:
        return "SecureCertFirst";
    default:
        return "Unknown";
    }
}

ESP32ManufacturingDataProvider::ESP32ManufacturingDataProvider(Precedence precedence)
    : CommissionableDataProvider()
#if CHIP_DEVICE_CONFIG_ENABLE_DEVICE_INSTANCE_INFO_PROVIDER
    , Internal::GenericDeviceInstanceInfoProvider<Internal::ESP32Config>(ConfigurationManagerImpl::GetDefaultInstance())
#endif
    , mPrecedence(precedence)
{
    // check data integrity of both factory and secure cert data, we should have at least one valid data provider
    mFactoryDataValid = CheckFactoryDataIntegrity();
    mSecureCertDataValid = CheckSecureCertDataIntegrity();

    // should we really die here? can the non-Matter functionality still work?
    VerifyOrReturn(mFactoryDataValid || mSecureCertDataValid,
                   ChipLogError(DeviceLayer, "No valid data found in factory or secure cert partition"));

    SetProvidersAsPerPrecedence();

    ChipLogDetail(DeviceLayer, "ESP32ManufacturingDataProvider initialized with precedence:%s",
                  PrecedenceToString(precedence));
}

ESP32ManufacturingDataProvider::Precedence ESP32ManufacturingDataProvider::GetPrecedence() const
{
    return mPrecedence;
}

// check if the factory has valid data, if not, return false
bool ESP32ManufacturingDataProvider::CheckFactoryDataIntegrity()
{
    uint16_t setupDiscriminator;
    uint32_t iterationCount;

    uint8_t saltBuf[chip::Crypto::kSpake2p_Max_PBKDF_Salt_Length];
    uint8_t verifierBuf[chip::Crypto::kSpake2p_VerifierSerialized_Length];
    uint8_t uniqueIdSpan[ConfigurationManager::kRotatingDeviceIDUniqueIDLength];

    MutableByteSpan salt(saltBuf);
    MutableByteSpan verifier(verifierBuf);
    size_t verifierLen;
    MutableByteSpan uniqueId(uniqueIdSpan);

    VerifyOrReturnValue(mPrecedence == Precedence::kFactoryFirst, false);
    VerifyOrReturnValue(mFactoryDataProvider.GetSetupDiscriminator(setupDiscriminator) == CHIP_NO_ERROR, false);
    VerifyOrReturnValue(mFactoryDataProvider.GetSpake2pIterationCount(iterationCount) == CHIP_NO_ERROR, false);
    VerifyOrReturnValue(mFactoryDataProvider.GetSpake2pSalt(salt) == CHIP_NO_ERROR, false);
    VerifyOrReturnValue(mFactoryDataProvider.GetSpake2pVerifier(verifier, verifierLen) == CHIP_NO_ERROR, false);

#if CHIP_DEVICE_CONFIG_ENABLE_DEVICE_INSTANCE_INFO_PROVIDER && CHIP_ENABLE_ROTATING_DEVICE_ID
    VerifyOrReturnValue(mFactoryDataProvider.GetRotatingDeviceIdUniqueId(uniqueId) == CHIP_NO_ERROR, false);
#endif // CHIP_DEVICE_CONFIG_ENABLE_DEVICE_INSTANCE_INFO_PROVIDER && CHIP_ENABLE_ROTATING_DEVICE_ID

    return true;
}

// check if the secure cert has valid data, if not, return false
bool ESP32ManufacturingDataProvider::CheckSecureCertDataIntegrity()
{
    uint16_t setupDiscriminator;
    uint32_t iterationCount;

    uint8_t saltBuf[chip::Crypto::kSpake2p_Max_PBKDF_Salt_Length];
    uint8_t verifierBuf[chip::Crypto::kSpake2p_VerifierSerialized_Length];
    uint8_t uniqueIdSpan[ConfigurationManager::kRotatingDeviceIDUniqueIDLength];

    MutableByteSpan salt(saltBuf);
    MutableByteSpan verifier(verifierBuf);
    size_t verifierLen;
    MutableByteSpan uniqueId(uniqueIdSpan);

    VerifyOrReturnValue(mSecureCertDataProvider.GetSetupDiscriminator(setupDiscriminator) == CHIP_NO_ERROR, false);
    VerifyOrReturnValue(mSecureCertDataProvider.GetSpake2pIterationCount(iterationCount) == CHIP_NO_ERROR, false);
    VerifyOrReturnValue(mSecureCertDataProvider.GetSpake2pSalt(salt) == CHIP_NO_ERROR, false);
    VerifyOrReturnValue(mSecureCertDataProvider.GetSpake2pVerifier(verifier, verifierLen) == CHIP_NO_ERROR, false);

#if CHIP_DEVICE_CONFIG_ENABLE_DEVICE_INSTANCE_INFO_PROVIDER && CHIP_ENABLE_ROTATING_DEVICE_ID
    VerifyOrReturnValue(mSecureCertDataProvider.GetRotatingDeviceIdUniqueId(uniqueId) == CHIP_NO_ERROR, false);
#endif // CHIP_DEVICE_CONFIG_ENABLE_DEVICE_INSTANCE_INFO_PROVIDER && CHIP_ENABLE_ROTATING_DEVICE_ID

    return true;
}

CHIP_ERROR ESP32ManufacturingDataProvider::SetProvidersAsPerPrecedence()
{
    bool useFactoryData = false;

    // we performed CheckFactoryDataIntegrity() and CheckSecureCertDataIntegrity() in constructor,
    // so we can use the result here
    switch (mPrecedence) {
    case Precedence::kFactoryFirst: {
        useFactoryData = mFactoryDataValid;
        break;
    }
    case Precedence::kSecureCertFirst: {
        useFactoryData = !mSecureCertDataValid;
        break;
    }
    default: {
        ChipLogError(DeviceLayer, "Invalid precedence");
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
    }

    ChipLogProgress(DeviceLayer, "Using commissionable and device instance info providers from %s",
                    useFactoryData ? "factory" : "secure cert");

    mCommissionableDataProvider = useFactoryData ? &mFactoryDataProvider : &mSecureCertDataProvider;
    mDeviceInstanceInfoProvider = useFactoryData ? &mFactoryDataProvider : &mSecureCertDataProvider;

    return CHIP_NO_ERROR;
}

CHIP_ERROR ESP32ManufacturingDataProvider::GetSetupDiscriminator(uint16_t  &setupDiscriminator)
{
    return mCommissionableDataProvider->GetSetupDiscriminator(setupDiscriminator);
}

CHIP_ERROR ESP32ManufacturingDataProvider::GetSpake2pIterationCount(uint32_t  &iterationCount)
{
    return mCommissionableDataProvider->GetSpake2pIterationCount(iterationCount);
}

CHIP_ERROR ESP32ManufacturingDataProvider::GetSpake2pSalt(MutableByteSpan  &saltBuf)
{
    return mCommissionableDataProvider->GetSpake2pSalt(saltBuf);
}

CHIP_ERROR ESP32ManufacturingDataProvider::GetSpake2pVerifier(MutableByteSpan  &verifierBuf, size_t  &verifierLen)
{
    return mCommissionableDataProvider->GetSpake2pVerifier(verifierBuf, verifierLen);
}

CHIP_ERROR ESP32ManufacturingDataProvider::GetRotatingDeviceIdUniqueId(MutableByteSpan  &uniqueIdSpan)
{
#if CHIP_DEVICE_CONFIG_ENABLE_DEVICE_INSTANCE_INFO_PROVIDER && CHIP_ENABLE_ROTATING_DEVICE_ID
    return mDeviceInstanceInfoProvider->GetRotatingDeviceIdUniqueId(uniqueIdSpan);
#else
    return CHIP_ERROR_NOT_IMPLEMENTED;
#endif // CHIP_DEVICE_CONFIG_ENABLE_DEVICE_INSTANCE_INFO_PROVIDER && CHIP_ENABLE_ROTATING_DEVICE_ID
}

} // namespace DeviceLayer
} // namespace chip
