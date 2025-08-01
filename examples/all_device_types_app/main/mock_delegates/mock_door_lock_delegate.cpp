/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"

#include "mock_door_lock_delegate.h"

namespace chip {
namespace app {
namespace Clusters {
namespace DoorLock {

CHIP_ERROR MockDoorLockDelegate::GetAliroReaderVerificationKey(MutableByteSpan  &verificationKey)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    verificationKey.reduce_size(0);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockDoorLockDelegate::GetAliroReaderGroupIdentifier(MutableByteSpan  &groupIdentifier)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    groupIdentifier.reduce_size(0);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockDoorLockDelegate::GetAliroReaderGroupSubIdentifier(MutableByteSpan  &groupSubIdentifier)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_NOT_FOUND;
}

CHIP_ERROR MockDoorLockDelegate::GetAliroExpeditedTransactionSupportedProtocolVersionAtIndex(size_t index, MutableByteSpan  &protocolVersion)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

CHIP_ERROR MockDoorLockDelegate::GetAliroGroupResolvingKey(MutableByteSpan  &groupResolvingKey)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    groupResolvingKey.reduce_size(0);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockDoorLockDelegate::GetAliroSupportedBLEUWBProtocolVersionAtIndex(size_t index, MutableByteSpan  &protocolVersion)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

uint8_t MockDoorLockDelegate::GetAliroBLEAdvertisingVersion()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

uint16_t MockDoorLockDelegate::GetNumberOfAliroCredentialIssuerKeysSupported()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

uint16_t MockDoorLockDelegate::GetNumberOfAliroEndpointKeysSupported()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

CHIP_ERROR MockDoorLockDelegate::SetAliroReaderConfig(const ByteSpan  &signingKey, const ByteSpan  &verificationKey,
                                                      const ByteSpan  &groupIdentifier, const Optional<ByteSpan>  &groupResolvingKey)
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockDoorLockDelegate::ClearAliroReaderConfig()
{
    // Implement your own logic here.
    ESP_LOGE(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

} // namespace DoorLock
} // namespace Clusters
} // namespace app
} // namespace chip