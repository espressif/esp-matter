/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "aliro_delegate.h"
#include "door_lock_storage.h"

#include <string.h>

#include <esp_aliro_utils.h>
#include <esp_log.h>
#include <m5nfc.h>
extern "C" {
#include <mbedtls/constant_time.h>
}
#include <mbedtls/platform_util.h>
#include <platform/ESP32/ESP32Config.h>
#include <platform/ESP32/ESP32Utils.h>

using namespace chip;

namespace {

const DeviceLayer::Internal::ESP32Config::Key kReaderConfig = {DeviceLayer::Internal::ESP32Config::kConfigNamespace_ChipConfig, "aliro-reader"};

struct ReaderConfig {
    uint8_t group[16];
    uint8_t verification[65];
    uint8_t signing[32];
};
constexpr uint8_t kVersion[] = {1, 0};

// Reader owns the cache; SDK serializes callbacks and ClearFast routes resets through it.
constexpr size_t kFastCacheCapacity = 3;
struct FastCacheSlot {
    bool used = false;
    esp_aliro_fast_transaction_entry_t entry = {};
};
FastCacheSlot sFastEntries[kFastCacheCapacity];
size_t sFastNext = 0;

esp_err_t VisitFastEntries(void *, esp_aliro_fast_transaction_entry_visitor_t visitor, void *context)
{
    if (!visitor) {
        return ESP_ERR_INVALID_ARG;
    }
    for (FastCacheSlot &slot : sFastEntries) {
        if (slot.used && !visitor(&slot.entry, context)) {
            break;
        }
    }
    return ESP_OK;
}

esp_err_t PutFastEntry(void *, const esp_aliro_fast_transaction_entry_t *entry)
{
    if (!entry) {
        return ESP_ERR_INVALID_ARG;
    }
    size_t target = kFastCacheCapacity;
    for (size_t i = 0; i < kFastCacheCapacity; ++i) {
        if (sFastEntries[i].used &&
                mbedtls_ct_memcmp(sFastEntries[i].entry.access_credential_public_key, entry->access_credential_public_key, 65) == 0) {
            target = i;
            break;
        }
    }
    if (target == kFastCacheCapacity) {
        target = sFastNext++ % kFastCacheCapacity;
    }
    mbedtls_platform_zeroize(&sFastEntries[target], sizeof(FastCacheSlot));
    sFastEntries[target].entry = *entry;
    sFastEntries[target].used = true;
    return ESP_OK;
}

esp_err_t ClearFastEntries(void *)
{
    mbedtls_platform_zeroize(sFastEntries, sizeof(sFastEntries));
    sFastNext = 0;
    return ESP_OK;
}

using ReaderGetter = esp_err_t (*)(esp_aliro_reader_handle_t, uint8_t *, size_t *);

CHIP_ERROR ReadReaderBytes(esp_aliro_reader_handle_t reader, MutableByteSpan &out, ReaderGetter getter)
{
    if (!reader) {
        out.reduce_size(0);
        return CHIP_NO_ERROR;
    }
    size_t size = out.size();
    esp_err_t err = getter(reader, out.data(), &size);
    if (err == ESP_OK) {
        out.reduce_size(size);
    }
    return DeviceLayer::Internal::ESP32Utils::MapError(err);
}

esp_err_t CreateReader(const ByteSpan &signing, const ByteSpan &verification, const ByteSpan &group,
                       esp_aliro_reader_handle_t &reader)
{
    char publicPem[256] = {}, privatePem[256] = {};
    size_t publicSize = sizeof(publicPem), privateSize = sizeof(privatePem);
    esp_err_t err =
        esp_aliro_utils_get_privkey_pem_from_raw_data(signing.data(), signing.size(), privatePem, &privateSize);
    if (err == ESP_OK) {
        err = esp_aliro_utils_get_pubkey_pem_from_raw_data(verification.data(), verification.size(), publicPem,
                                                           &publicSize);
    }
    esp_aliro_reader_config_t config = {};
    memcpy(config.group_identifier, group.data(), 16);
    config.reader_pubkey = publicPem;
    config.reader_privkey = privatePem;
    if (err == ESP_OK) {
        err = esp_aliro_reader_create(&reader, &config);
    }
    mbedtls_platform_zeroize(privatePem, sizeof(privatePem));
    if (err == ESP_OK) {
        esp_aliro_fast_transaction_storage_delegate_t storage = {};
        storage.for_each_entry = VisitFastEntries;
        storage.store_entry = storage.touch_entry = PutFastEntry;
        storage.clear = ClearFastEntries;
        err = esp_aliro_reader_set_fast_transaction_storage_delegate(reader, &storage);
    }
    if (err == ESP_OK) {
        // Full credential keys let new endpoints authenticate before document verification.
        err = esp_aliro_reader_enable(reader);
    }
    if (err != ESP_OK && reader) {
        esp_aliro_reader_delete(&reader);
        reader = 0;
    }
    return err;
}

} // namespace

CHIP_ERROR AliroDoorLockDelegate::GetAliroReaderVerificationKey(MutableByteSpan &key)
{
    return ReadReaderBytes(m_aliro_reader, key, esp_aliro_reader_get_public_key_raw_data);
}

CHIP_ERROR AliroDoorLockDelegate::GetAliroReaderGroupIdentifier(MutableByteSpan &group)
{
    return ReadReaderBytes(m_aliro_reader, group, esp_aliro_reader_get_group_identifier);
}

CHIP_ERROR AliroDoorLockDelegate::GetAliroReaderGroupSubIdentifier(MutableByteSpan &group)
{
    return ReadReaderBytes(m_aliro_reader, group, esp_aliro_reader_get_group_sub_identifier);
}

CHIP_ERROR
AliroDoorLockDelegate::GetAliroExpeditedTransactionSupportedProtocolVersionAtIndex(size_t index,
                                                                                   MutableByteSpan &version)
{
    if (index || version.size() < sizeof(kVersion)) {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }
    memcpy(version.data(), kVersion, sizeof(kVersion));
    version.reduce_size(sizeof(kVersion));
    return CHIP_NO_ERROR;
}

CHIP_ERROR AliroDoorLockDelegate::SetAliroReaderConfig(const ByteSpan &signing, const ByteSpan &verification,
                                                       const ByteSpan &group, const Optional<ByteSpan> &resolving)
{
    if (mBusy) {
        return CHIP_ERROR_BUSY;
    }
    if (mFailed) {
        return CHIP_ERROR_INCORRECT_STATE;
    }
    if (signing.size() != 32 || verification.size() != 65 || group.size() != 16 || resolving.HasValue()) {
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
    if (!DoorLockStorage::Instance().Invalidate("reader-configuration")) {
        return CHIP_ERROR_INCORRECT_STATE;
    }
    if (m_aliro_reader) {
        CHIP_ERROR deleted = DeviceLayer::Internal::ESP32Utils::MapError(esp_aliro_reader_delete(&m_aliro_reader));
        if (deleted != CHIP_NO_ERROR) {
            mFailed = true;
            DoorLockStorage::Instance().Unavailable("reader-delete");
            return deleted;
        }
    }

    esp_aliro_reader_handle_t replacement = 0;
    ReturnErrorOnFailure(DeviceLayer::Internal::ESP32Utils::MapError(CreateReader(signing, verification, group, replacement)));
    ReaderConfig config;
    memcpy(config.group, group.data(), sizeof(config.group));
    memcpy(config.verification, verification.data(), sizeof(config.verification));
    memcpy(config.signing, signing.data(), sizeof(config.signing));
    CHIP_ERROR err = DeviceLayer::Internal::ESP32Config::WriteConfigValueBin(kReaderConfig, reinterpret_cast<const uint8_t *>(&config),
                                                                             sizeof(config));
    mbedtls_platform_zeroize(config.signing, sizeof(config.signing));
    if (err != CHIP_NO_ERROR) {
        esp_aliro_reader_delete(&replacement);
        mFailed = true;
        DoorLockStorage::Instance().Unavailable("reader-configuration-write");
        return err;
    }
    m_aliro_reader = replacement;
    return CHIP_NO_ERROR;
}

CHIP_ERROR AliroDoorLockDelegate::ClearAliroReaderConfig()
{
    if (mBusy) {
        return CHIP_ERROR_BUSY;
    }
    if (!DoorLockStorage::Instance().Invalidate("reader-clear")) {
        return CHIP_ERROR_INCORRECT_STATE;
    }
    CHIP_ERROR err = CHIP_NO_ERROR;
    if (m_aliro_reader) {
        err = DeviceLayer::Internal::ESP32Utils::MapError(esp_aliro_reader_delete(&m_aliro_reader));
    }
    if (err != CHIP_NO_ERROR) {
        mFailed = true;
        DoorLockStorage::Instance().Unavailable("reader-delete");
        return err;
    }
    m_aliro_reader = 0;
    err = DeviceLayer::Internal::ESP32Config::ClearConfigValue(kReaderConfig);
    if (err == CHIP_DEVICE_ERROR_CONFIG_NOT_FOUND) {
        err = CHIP_NO_ERROR;
    }
    if (err != CHIP_NO_ERROR) {
        mFailed = true;
        DoorLockStorage::Instance().Unavailable("reader-clear");
    } else {
        mFailed = false;
    }
    return err;
}

esp_err_t AliroDoorLockDelegate::ClearFast()
{
    return m_aliro_reader ? esp_aliro_reader_clear_fast_transaction_storage(m_aliro_reader) : ClearFastEntries(nullptr);
}

esp_err_t AliroDoorLockDelegate::Init()
{
    if (m_nfc_detect_task_handle) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t initialized = m5nfc_init();
    if (initialized != ESP_OK) {
        return initialized;
    }
    initialized = esp_aliro_init(nullptr);
    if (initialized != ESP_OK) {
        return initialized;
    }

    ReaderConfig config = {};
    size_t size = 0;
    CHIP_ERROR stored = DeviceLayer::Internal::ESP32Config::ReadConfigValueBin(kReaderConfig, reinterpret_cast<uint8_t *>(&config),
                                                                               sizeof(config), size);
    esp_err_t err = ESP_OK;
    if (stored == CHIP_NO_ERROR && size == sizeof(config)) {
        err = CreateReader(ByteSpan(config.signing), ByteSpan(config.verification), ByteSpan(config.group),
                           m_aliro_reader);
    } else if (stored != CHIP_DEVICE_ERROR_CONFIG_NOT_FOUND) {
        err = ESP_ERR_INVALID_STATE;
    }
    mbedtls_platform_zeroize(config.signing, sizeof(config.signing));
    if (err != ESP_OK) {
        mFailed = true;
        DoorLockStorage::Instance().Unavailable("reader-initialization");
    }
    if (xTaskCreate(ReaderTask, "aliro_nfc", 6144, this, 5, &m_nfc_detect_task_handle) != pdPASS) {
        return ESP_ERR_NO_MEM;
    }
    return err;
}
