/*
  This example code is in the Public Domain (or CC0 licensed, at your option.)

  Unless required by applicable law or agreed to in writing, this
  software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
  CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <app/clusters/door-lock-server/door-lock-server.h>
#include <esp_aliro.h>

using MutableByteSpan = chip::MutableByteSpan;
using ByteSpan = chip::ByteSpan;

class AliroDoorLockDelegate : public chip::app::Clusters::DoorLock::Delegate {
public:
    ~AliroDoorLockDelegate() = default;

    CHIP_ERROR GetAliroReaderVerificationKey(MutableByteSpan  &verificationKey) override;
    CHIP_ERROR GetAliroReaderGroupIdentifier(MutableByteSpan  &groupIdentifier) override;
    CHIP_ERROR GetAliroReaderGroupSubIdentifier(MutableByteSpan  &groupSubIdentifier) override;

    CHIP_ERROR GetAliroExpeditedTransactionSupportedProtocolVersionAtIndex(size_t index,
                                                                           MutableByteSpan  &protocolVersion) override;

    CHIP_ERROR GetAliroGroupResolvingKey(MutableByteSpan  &groupResolvingKey) override;

    CHIP_ERROR GetAliroSupportedBLEUWBProtocolVersionAtIndex(size_t index, MutableByteSpan  &protocolVersion) override;

    uint8_t GetAliroBLEAdvertisingVersion() override;

    uint16_t GetNumberOfAliroCredentialIssuerKeysSupported() override;

    uint16_t GetNumberOfAliroEndpointKeysSupported() override;

    CHIP_ERROR SetAliroReaderConfig(const ByteSpan  &signingKey, const ByteSpan  &verificationKey,
                                    const ByteSpan  &groupIdentifier, const chip::Optional<ByteSpan>  &groupResolvingKey) override;

    CHIP_ERROR ClearAliroReaderConfig() override;

    esp_err_t Init();

    static esp_err_t NfcMessageExchange(const uint8_t *command, size_t command_len, uint8_t *response,
                                        size_t *response_len);
    static void NfcDetectTask(void *delegate);

    static AliroDoorLockDelegate  &Instance()
    {
        static AliroDoorLockDelegate instance;
        return instance;
    }
private:
    AliroDoorLockDelegate() = default;
    esp_aliro_reader_handle_t m_aliro_reader = 0;
    TaskHandle_t m_nfc_detect_task_handle = nullptr;
};
