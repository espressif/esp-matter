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

#pragma once

#include <platform/CommissionableDataProvider.h>
#include <platform/internal/GenericDeviceInstanceInfoProvider.h>
#include <platform/ESP32/ESP32FactoryDataProvider.h>
#include <platform/ESP32/ESP32SecureCertDataProvider.h>

namespace chip {
namespace DeviceLayer {

// This implementation is a wrapper around the factory and secure cert data providers
// It facilitates users to choose the precedence of the data that should be used.
// eg: First batch of modules were manufactured with unique data in the factory partition
// and the second batch of modules were manufactured with unique data in the secure cert partition
// This helps users to keep the backward compatibility with the existing code
// by falling back to the different set of data.

class ESP32ManufacturingDataProvider : public CommissionableDataProvider
#if CHIP_DEVICE_CONFIG_ENABLE_DEVICE_INSTANCE_INFO_PROVIDER
    , public Internal::GenericDeviceInstanceInfoProvider<Internal::ESP32Config>
#endif
{
public:
    enum class Precedence : uint8_t {
        // Data is expected to be found in the factory partition first, if not
        // fallback to the secure cert partition
        kFactoryFirst,
        // Data is expected to be found in the secure cert partition first, if not
        // fallback to the factory partition
        kSecureCertFirst
    };

    static const char * PrecedenceToString(Precedence precedence);

    // Default precedence is FactoryFirst
    ESP32ManufacturingDataProvider(Precedence precedence = Precedence::kFactoryFirst);

    // Get the precedence of the data provider
    Precedence GetPrecedence() const;

    // CommissionableDataProvider implementation
    CHIP_ERROR GetSetupDiscriminator(uint16_t  &setupDiscriminator) override;
    CHIP_ERROR GetSpake2pIterationCount(uint32_t  &iterationCount) override;
    CHIP_ERROR GetSpake2pSalt(MutableByteSpan  &saltBuf) override;
    CHIP_ERROR GetSpake2pVerifier(MutableByteSpan  &verifierBuf, size_t  &verifierLen) override;

    // not supported APIs but required to be implemented
    CHIP_ERROR SetSetupDiscriminator(uint16_t setupDiscriminator) override
    {
        return CHIP_ERROR_NOT_IMPLEMENTED;
    }
    CHIP_ERROR GetSetupPasscode(uint32_t  &setupPasscode) override
    {
        return CHIP_ERROR_NOT_IMPLEMENTED;
    }
    CHIP_ERROR SetSetupPasscode(uint32_t setupPasscode) override
    {
        return CHIP_ERROR_NOT_IMPLEMENTED;
    }

#if CHIP_DEVICE_CONFIG_ENABLE_DEVICE_INSTANCE_INFO_PROVIDER
    // DeviceInstanceInfoProvider implementation
    CHIP_ERROR GetRotatingDeviceIdUniqueId(MutableByteSpan  &uniqueIdSpan) override;
#endif // CHIP_DEVICE_CONFIG_ENABLE_DEVICE_INSTANCE_INFO_PROVIDER

private:
    Precedence mPrecedence;
    bool mFactoryDataValid = false;
    bool mSecureCertDataValid = false;

    CommissionableDataProvider * mCommissionableDataProvider = nullptr;
    DeviceInstanceInfoProvider * mDeviceInstanceInfoProvider = nullptr;

    ESP32FactoryDataProvider mFactoryDataProvider;
    ESP32SecureCertDataProvider mSecureCertDataProvider;

    // Helper method to update provider pointers based on precedence
    CHIP_ERROR SetProvidersAsPerPrecedence();

    bool CheckFactoryDataIntegrity();
    bool CheckSecureCertDataIntegrity();
};

} // namespace DeviceLayer
} // namespace chip