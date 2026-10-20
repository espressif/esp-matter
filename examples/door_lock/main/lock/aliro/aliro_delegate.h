/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include "door_lock_capabilities.h"

#include <stddef.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_aliro.h>
#include <app/clusters/door-lock-server/door-lock-server.h>

using MutableByteSpan = chip::MutableByteSpan;
using ByteSpan = chip::ByteSpan;

class AliroDoorLockDelegate : public chip::app::Clusters::DoorLock::Delegate {
public:
    /** Copy the configured reader verification key into the caller span; empty when unconfigured. */
    CHIP_ERROR GetAliroReaderVerificationKey(MutableByteSpan &verificationKey) override;

    /** Copy the configured group identifier; empty when unconfigured. */
    CHIP_ERROR GetAliroReaderGroupIdentifier(MutableByteSpan &groupIdentifier) override;

    /** Copy the SDK group sub-identifier; empty when unconfigured. */
    CHIP_ERROR GetAliroReaderGroupSubIdentifier(MutableByteSpan &groupSubIdentifier) override;

    /** Enumerate the supported expedited protocol version; signal list exhaustion otherwise. */
    CHIP_ERROR GetAliroExpeditedTransactionSupportedProtocolVersionAtIndex(size_t index,
                                                                           MutableByteSpan &protocolVersion) override;

    /** Group-resolving keys are unsupported by this NFC-only profile. */
    CHIP_ERROR GetAliroGroupResolvingKey(MutableByteSpan &) override
    {
        return CHIP_ERROR_NOT_IMPLEMENTED;
    }

    /** BLE/UWB is unsupported; the version list is empty. */
    CHIP_ERROR GetAliroSupportedBLEUWBProtocolVersionAtIndex(size_t, MutableByteSpan &) override
    {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }

    /** Report no BLE advertising support. */
    uint8_t GetAliroBLEAdvertisingVersion() override
    {
        return 0;
    }

    /** Return the advertised provisioned issuer-key capacity. */
    uint16_t GetNumberOfAliroCredentialIssuerKeysSupported() override
    {
        return DoorLockCapabilities::kAliroCredentialIssuerKeySlots;
    }

    /** Return the shared evictable/non-evictable endpoint-key capacity. */
    uint16_t GetNumberOfAliroEndpointKeysSupported() override
    {
        return DoorLockCapabilities::kAliroEndpointKeySlots;
    }

    /** Persist one complete key/group blob; reject while NFC owns the reader. */
    CHIP_ERROR SetAliroReaderConfig(const ByteSpan &signingKey, const ByteSpan &verificationKey,
                                    const ByteSpan &groupIdentifier,
                                    const chip::Optional<ByteSpan> &groupResolvingKey) override;

    /** Disable the reader and delete its configuration; reject active NFC ownership. */
    CHIP_ERROR ClearAliroReaderConfig() override;

    /** Initialize SDK/NFC, load reader configuration and start the worker. */
    esp_err_t Init();

    /** Erase volatile fast keys without deleting an active session. */
    esp_err_t ClearFast();

    /** Return the single owner of SDK reader/session lifetimes. */
    static AliroDoorLockDelegate &Instance()
    {
        static AliroDoorLockDelegate instance;
        return instance;
    }

private:
    AliroDoorLockDelegate() = default;
    AliroDoorLockDelegate(const AliroDoorLockDelegate &) = delete;
    AliroDoorLockDelegate &operator=(const AliroDoorLockDelegate &) = delete;

    static void ReaderTask(void *context);
    bool mBusy = false;
    bool mFailed = false;
    esp_aliro_reader_handle_t m_aliro_reader = 0;
    TaskHandle_t m_nfc_detect_task_handle = nullptr;
};
