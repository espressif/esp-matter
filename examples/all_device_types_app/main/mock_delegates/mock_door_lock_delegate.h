/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <app/clusters/door-lock-server/door-lock-delegate.h>

/*
 * Mock DoorLock Delegate Implementation
 * This file provides a mock implementation of the DoorLock::Delegate interface
 * that returns success for all methods.
 * For more details, take a look at the delegate interface in the Matter SDK.
 * 1. Delegate Interface: https://github.com/project-chip/connectedhomeip/blob/d144bbb/src/app/clusters/door-lock-server/door-lock-delegate.h
 * 2. Delegate Implementation: https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/lock-app/lock-common/include/LockEndpoint.h and
 *    https://github.com/project-chip/connectedhomeip/blob/d144bbb/examples/lock-app/lock-common/src/LockEndpoint.cpp
 */

namespace chip {
namespace app {
namespace Clusters {
namespace DoorLock {

class MockDoorLockDelegate : public Delegate {
public:
    MockDoorLockDelegate() = default;

    CHIP_ERROR GetAliroReaderVerificationKey(MutableByteSpan  &verificationKey) override;
    CHIP_ERROR GetAliroReaderGroupIdentifier(MutableByteSpan  &groupIdentifier) override;
    CHIP_ERROR GetAliroReaderGroupSubIdentifier(MutableByteSpan  &groupSubIdentifier) override;
    CHIP_ERROR GetAliroExpeditedTransactionSupportedProtocolVersionAtIndex(size_t index, MutableByteSpan  &protocolVersion) override;
    CHIP_ERROR GetAliroGroupResolvingKey(MutableByteSpan  &groupResolvingKey) override;
    CHIP_ERROR GetAliroSupportedBLEUWBProtocolVersionAtIndex(size_t index, MutableByteSpan  &protocolVersion) override;
    uint8_t GetAliroBLEAdvertisingVersion() override;
    uint16_t GetNumberOfAliroCredentialIssuerKeysSupported() override;
    uint16_t GetNumberOfAliroEndpointKeysSupported() override;
    CHIP_ERROR SetAliroReaderConfig(const ByteSpan  &signingKey, const ByteSpan  &verificationKey,
                                    const ByteSpan  &groupIdentifier, const Optional<ByteSpan>  &groupResolvingKey) override;
    CHIP_ERROR ClearAliroReaderConfig() override;

private:
    const char *LOG_TAG = "door_lock";
};

} // namespace DoorLock
} // namespace Clusters
} // namespace app
} // namespace chip