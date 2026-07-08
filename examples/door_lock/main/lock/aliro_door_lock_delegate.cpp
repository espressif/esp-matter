/*
  This example code is in the Public Domain (or CC0 licensed, at your option.)

  Unless required by applicable law or agreed to in writing, this
  software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
  CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_aliro.h>
#include <esp_aliro_types.h>
#include <esp_aliro_utils.h>
#include <esp_check.h>
#include <m5nfc.h>
#include <nvs.h>

#include <app/clusters/door-lock-server/door-lock-server.h>
#include <lib/support/CodeUtils.h>
#include <platform/ESP32/ESP32Config.h>
#include <platform/ESP32/ESP32Utils.h>
#include <platform/ESP32/ScopedNvsHandle.h>
#include <platform/PlatformManager.h>

#include "aliro_door_lock_delegate.h"
#include "door_lock_manager.h"

using namespace chip;
using namespace chip::DeviceLayer::Internal;

#define NFC_DETECT_TASK_STACK_SIZE 6144
#define NFC_DETECT_TASK_PRIORITY 5

constexpr uint8_t kAliroProtocolVersion[] = {0x01, 0x00};
constexpr size_t kAliroKeyPEMBufMax = 256;
constexpr size_t kAliroVerificationKeyLen = 65;
constexpr size_t kAliroSigningKeyLen = 32;
extern uint16_t door_lock_endpoint_id;

const ESP32Config::Key kAliroGroupId = { ESP32Config::kConfigNamespace_ChipConfig, "aliro-grp-id"};
const ESP32Config::Key kAliroVerificationKey = { ESP32Config::kConfigNamespace_ChipConfig, "aliro-pub-key" };
const ESP32Config::Key kAliroSigningKey = { ESP32Config::kConfigNamespace_ChipConfig, "aliro-priv-key" };

CHIP_ERROR AliroDoorLockDelegate::GetAliroReaderVerificationKey(MutableByteSpan  &verificationKey)
{
    if (!m_aliro_reader) {
        verificationKey.reduce_size(0);
        return CHIP_NO_ERROR;
    }
    size_t verificationKeyLen = verificationKey.size();
    esp_err_t err = esp_aliro_reader_get_public_key_raw_data(m_aliro_reader, verificationKey.data(), &verificationKeyLen);
    verificationKey.reduce_size(verificationKeyLen);
    return ESP32Utils::MapError(err);
}

CHIP_ERROR AliroDoorLockDelegate::GetAliroReaderGroupIdentifier(MutableByteSpan  &groupIdentifier)
{
    if (!m_aliro_reader) {
        groupIdentifier.reduce_size(0);
        return CHIP_NO_ERROR;
    }
    size_t groupIdentifierLen = groupIdentifier.size();
    esp_err_t err = esp_aliro_reader_get_group_identifier(m_aliro_reader, groupIdentifier.data(), &groupIdentifierLen);
    groupIdentifier.reduce_size(groupIdentifierLen);
    return ESP32Utils::MapError(err);
}

CHIP_ERROR AliroDoorLockDelegate::GetAliroReaderGroupSubIdentifier(MutableByteSpan  &groupSubIdentifier)
{
    if (!m_aliro_reader) {
        groupSubIdentifier.reduce_size(0);
        return CHIP_NO_ERROR;
    }
    size_t groupSubIdentifierLen = groupSubIdentifier.size();
    esp_err_t err = esp_aliro_reader_get_group_sub_identifier(m_aliro_reader, groupSubIdentifier.data(), &groupSubIdentifierLen);
    groupSubIdentifier.reduce_size(groupSubIdentifierLen);
    return ESP32Utils::MapError(err);
}

CHIP_ERROR AliroDoorLockDelegate::GetAliroExpeditedTransactionSupportedProtocolVersionAtIndex(size_t index,
                                                                                              MutableByteSpan  &protocolVersion)
{
    if (index != 0 || protocolVersion.size() < sizeof(kAliroProtocolVersion)) {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }
    memcpy(protocolVersion.data(), kAliroProtocolVersion, sizeof(kAliroProtocolVersion));
    protocolVersion.reduce_size(sizeof(kAliroProtocolVersion));
    return CHIP_NO_ERROR;
}

CHIP_ERROR AliroDoorLockDelegate::GetAliroGroupResolvingKey(MutableByteSpan  &groupResolvingKey)
{
    return CHIP_ERROR_NOT_IMPLEMENTED;
}

CHIP_ERROR AliroDoorLockDelegate::GetAliroSupportedBLEUWBProtocolVersionAtIndex(size_t index, MutableByteSpan  &protocolVersion)
{
    return CHIP_ERROR_NOT_IMPLEMENTED;
}

uint8_t AliroDoorLockDelegate::GetAliroBLEAdvertisingVersion()
{
    return 0;
}

uint16_t AliroDoorLockDelegate::GetNumberOfAliroCredentialIssuerKeysSupported()
{
    return 8;
}

uint16_t AliroDoorLockDelegate::GetNumberOfAliroEndpointKeysSupported()
{
    return 8;
}

CHIP_ERROR AliroDoorLockDelegate::SetAliroReaderConfig(const ByteSpan  &signingKey, const ByteSpan  &verificationKey,
                                                       const ByteSpan  &groupIdentifier, const chip::Optional<ByteSpan>  &groupResolvingKey)
{
    if (m_aliro_reader) {
        return CHIP_ERROR_INCORRECT_STATE;
    }
    if (groupIdentifier.size() != 16 || signingKey.size() != 32 || verificationKey.size() != 65) {
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
    if (groupResolvingKey.HasValue()) {
        ESP_LOGW("Aliro", "groupResolvingKey will be ignored as the Door Lock doesn't support Aliro Over BLE+UWB");
    }
    char pubkey_pem_buf[kAliroKeyPEMBufMax];
    char privkey_pem_buf[kAliroKeyPEMBufMax];
    size_t pubkey_pem_len = sizeof(pubkey_pem_buf) - 1;
    size_t privkey_pem_len = sizeof(privkey_pem_buf) - 1;
    ReturnErrorOnFailure(
        ESP32Utils::MapError(esp_aliro_get_privkey_pem_from_raw_data(signingKey.data(), signingKey.size(),
                                                                     privkey_pem_buf, &privkey_pem_len)));
    privkey_pem_buf[privkey_pem_len] = 0;
    ReturnErrorOnFailure(
        ESP32Utils::MapError(esp_aliro_get_pubkey_pem_from_raw_data(verificationKey.data(), verificationKey.size(),
                                                                    pubkey_pem_buf, &pubkey_pem_len)));
    pubkey_pem_buf[pubkey_pem_len] = 0;
    esp_aliro_reader_config_t reader_config = {
        .group_identifier = {0},
        .reader_pubkey = pubkey_pem_buf,
        .reader_privkey = privkey_pem_buf,
    };
    memcpy(reader_config.group_identifier, groupIdentifier.data(), groupIdentifier.size());

    ReturnErrorOnFailure(ESP32Utils::MapError(esp_aliro_reader_create(&m_aliro_reader, &reader_config)));
    ReturnErrorOnFailure(ESP32Utils::MapError(esp_aliro_reader_enable(m_aliro_reader)));
    ReturnErrorOnFailure(ESP32Config::WriteConfigValueBin(kAliroGroupId, groupIdentifier.data(), groupIdentifier.size()));
    ReturnErrorOnFailure(ESP32Config::WriteConfigValueBin(kAliroVerificationKey, verificationKey.data(), verificationKey.size()));
    ReturnErrorOnFailure(ESP32Config::WriteConfigValueBin(kAliroSigningKey, signingKey.data(), signingKey.size()));
    return CHIP_NO_ERROR;
}

CHIP_ERROR AliroDoorLockDelegate::ClearAliroReaderConfig()
{
    if (m_aliro_reader) {
        ReturnErrorOnFailure(ESP32Config::ClearConfigValue(kAliroGroupId));
        ReturnErrorOnFailure(ESP32Config::ClearConfigValue(kAliroVerificationKey));
        ReturnErrorOnFailure(ESP32Config::ClearConfigValue(kAliroSigningKey));
        ReturnErrorOnFailure(ESP32Utils::MapError(esp_aliro_reader_disable(m_aliro_reader)));
        ReturnErrorOnFailure(ESP32Utils::MapError(esp_aliro_reader_delete(&m_aliro_reader)));
        m_aliro_reader = 0;
    }
    return CHIP_NO_ERROR;
}

esp_err_t AliroDoorLockDelegate::NfcMessageExchange(const uint8_t *command, size_t command_len, uint8_t *response,
                                                    size_t *response_len)
{
    return m5nfc_message_exchange(command, command_len, response, response_len);
}

void AliroDoorLockDelegate::NfcDetectTask(void *delegate)
{
    AliroDoorLockDelegate *doorLockDelegate = (AliroDoorLockDelegate *)delegate;
    while (true) {
        m5nfc_update();
        if (!m5nfc_activate()) {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }
        if (!doorLockDelegate->m_aliro_reader) {
            m5nfc_deactivate();
            continue;
        }
        const int64_t session_start_us = esp_timer_get_time();
        esp_aliro_session_config_t session_cfg = {
            .aid_type = ESP_ALIRO_NFC_AID_EXPEDITED_PHASE,
            .auth_policy = ESP_ALIRO_AUTH_POLICY_USER_DEVICE_SETTING_SECURE_ACTION,
        };
        esp_aliro_session_handle_t session = 0;
        esp_err_t err = esp_aliro_session_create(doorLockDelegate->m_aliro_reader, &session, &session_cfg);
        if (err == ESP_OK) {
            err = esp_aliro_session_run_expedited(session, NfcMessageExchange);
        }
        if (err == ESP_OK) {
            err = esp_aliro_session_run_exchange(session, NfcMessageExchange, ESP_ALIRO_CRYPTO_ENGINE_EXPEDITED);
        }
        if (session) {
            (void)esp_aliro_session_delete(&session);
        }
        const int64_t session_time_ms = (esp_timer_get_time() - session_start_us) / 1000;
        if (err == ESP_OK) {
            ESP_LOGI("Aliro", "Aliro NFC transaction completed successfully in %lld ms", (long long)session_time_ms);
            // UnLock the door lock
            (void) DeviceLayer::PlatformMgr().ScheduleWork([](intptr_t) {
                BoltLockMgr().Unlock(door_lock_endpoint_id, OperationSourceEnum::kAliro);
            });
        } else {
            ESP_LOGW("Aliro", "Aliro NFC transaction failed after %lld ms: %s", (long long)session_time_ms,
                     esp_err_to_name(err));
        }
        m5nfc_deactivate();
    }
}

esp_err_t AliroDoorLockDelegate::Init()
{
    if (m_nfc_detect_task_handle) {
        return ESP_ERR_INVALID_STATE;
    }

    ESP_RETURN_ON_ERROR(m5nfc_init(), "Aliro", "NFC reader initialization failed");

    xTaskCreate(NfcDetectTask, "nfc_detect", NFC_DETECT_TASK_STACK_SIZE, this, NFC_DETECT_TASK_PRIORITY, &m_nfc_detect_task_handle);
    esp_aliro_config_t aliro_config = {
        .storage_partition_name = nullptr,
        .fast_transaction_storage_size = 10,
    };
    ESP_RETURN_ON_ERROR(esp_aliro_init(&aliro_config), "Aliro", "Failed to init aliro");

    esp_aliro_reader_config_t reader_config = {
        .group_identifier = {0},
        .reader_pubkey = nullptr,
        .reader_privkey = nullptr,
    };
    size_t group_id_len = sizeof(reader_config.group_identifier);
    CHIP_ERROR err = ESP32Config::ReadConfigValueBin(kAliroGroupId, reader_config.group_identifier, group_id_len, group_id_len);
    if (err == CHIP_DEVICE_ERROR_CONFIG_NOT_FOUND) {
        ESP_LOGI("Aliro", "Aliro Reader is not configured");
        return ESP_OK;
    }
    ESP_RETURN_ON_FALSE(err == CHIP_NO_ERROR, ESP_FAIL, "Aliro", "Failed to get reader group id");
    ESP_RETURN_ON_FALSE(group_id_len == sizeof(reader_config.group_identifier), ESP_ERR_INVALID_SIZE, "Aliro",
                        "Failed to get reader group id");
    uint8_t verification_key_buf[kAliroVerificationKeyLen] = {0};
    size_t verification_key_len = kAliroVerificationKeyLen;
    uint8_t signing_key_buf[kAliroSigningKeyLen];
    size_t signing_key_len = kAliroSigningKeyLen;
    ReturnValueOnFailure(
        ESP32Config::ReadConfigValueBin(kAliroVerificationKey, verification_key_buf, verification_key_len, verification_key_len), ESP_FAIL);
    ReturnValueOnFailure(
        ESP32Config::ReadConfigValueBin(kAliroSigningKey, signing_key_buf, signing_key_len, signing_key_len), ESP_FAIL);

    char pubkey_pem_buf[kAliroKeyPEMBufMax] = {0};
    char privkey_pem_buf[kAliroKeyPEMBufMax] = {0};
    size_t pubkey_pem_len = sizeof(pubkey_pem_buf);
    size_t privkey_pem_len = sizeof(privkey_pem_buf);

    ESP_RETURN_ON_ERROR(esp_aliro_get_privkey_pem_from_raw_data(signing_key_buf, signing_key_len, privkey_pem_buf, &privkey_pem_len),
                        "Aliro", "Failed to get priv key pem");
    privkey_pem_buf[privkey_pem_len] = 0;
    ESP_RETURN_ON_ERROR(esp_aliro_get_pubkey_pem_from_raw_data(verification_key_buf, verification_key_len,
                                                               pubkey_pem_buf, &pubkey_pem_len), "Aliro", "Failed to get pub key pem");
    pubkey_pem_buf[pubkey_pem_len] = 0;
    reader_config.reader_pubkey = pubkey_pem_buf;
    reader_config.reader_privkey = privkey_pem_buf;

    ESP_RETURN_ON_ERROR(esp_aliro_reader_create(&m_aliro_reader, &reader_config), "Aliro", "Failed to create reader");
    ESP_RETURN_ON_ERROR(esp_aliro_reader_enable(m_aliro_reader), "Aliro", "Failed to enable reader");
    return ESP_OK;
}
