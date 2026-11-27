/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "aliro_delegate.h"
#include "aliro_document.h"
#include "bolt_lock_manager.h"
#include "door_lock_authorization.h"
#include "door_lock_storage.h"
#include "time_sync.h"

#include <cstdint>
#include <memory>
#include <new>
#include <string.h>

#include <esp_aliro_utils.h>
#include <esp_log.h>
#include <esp_matter.h>
#include <esp_system.h>
#include <m5nfc.h>
#include <platform/CHIPDeviceLayer.h>

using namespace chip;
using namespace chip::app::Clusters::DoorLock;

namespace {

const char *TAG = "aliro_reader";
constexpr size_t kResponseCapacity = 8192;

esp_err_t Authenticate(esp_aliro_reader_handle_t reader, esp_aliro_session_handle_t &session, AliroProof &proof,
                       esp_aliro_crypto_engine_type_t &engine, esp_aliro_reader_status_t &status)
{
    {
        esp_matter::lock::ScopedChipStackLock lock(portMAX_DELAY);
        DoorLockStorage &store = DoorLockStorage::Instance();
        if (!store.Ready()) {
            return ESP_FAIL;
        }
        proof.generation = store.Generation();
    }
    esp_aliro_session_config_t config = {};
    config.aid_type = ESP_ALIRO_NFC_AID_EXPEDITED_PHASE;
    config.auth_policy = ESP_ALIRO_AUTH_POLICY_USER_DEVICE_SETTING_SECURE_ACTION;
    esp_err_t err = esp_aliro_session_create(reader, &session, &config);
    if (err != ESP_OK) {
        return err;
    }

    esp_aliro_expedited_result_t authentication = {};
    err = esp_aliro_session_run_expedited(session, m5nfc_message_exchange, &authentication);
    if (err != ESP_OK) {
        return err;
    }
    memcpy(proof.endpointKey, authentication.access_credential_public_key, 65);
    bool found = false;
    {
        esp_matter::lock::ScopedChipStackLock lock(portMAX_DELAY);
        DoorLockStorage &store = DoorLockStorage::Instance();
        if (store.Generation() != proof.generation || !store.FindEndpoint(proof.endpointKey, proof.credential, found)) {
            return ESP_ERR_INVALID_STATE;
        }
    }
    ESP_LOGI(TAG, "Authenticated: path=%s authorization=pending", found ? "endpoint" : "issuer");

    if (found) {
        return ESP_OK;
    }
    err = esp_aliro_session_continue_standard(session, m5nfc_message_exchange, &authentication);
    if (err != ESP_OK) {
        return err;
    }
    time_t now;
    if (!TimeSync::Now(now, proof.clockGeneration)) {
        return ESP_ERR_INVALID_STATE;
    }
    uint8_t request[128];
    size_t requestSize = sizeof(request);
    err = esp_aliro_utils_access_document_request_encode(AliroDocument::kElement, sizeof(AliroDocument::kElement),
                                                         false, request, &requestSize);
    if (err != ESP_OK) {
        return err;
    }
    std::unique_ptr<uint8_t[]> response(new (std::nothrow) uint8_t[kResponseCapacity]);
    if (!response) {
        return ESP_ERR_NO_MEM;
    }
    size_t responseSize = kResponseCapacity;
    err = esp_aliro_session_run_envelope(session, m5nfc_message_exchange, request, requestSize,
                                         response.get(), &responseSize);
    if (err != ESP_OK) {
        return err;
    }
    engine = ESP_ALIRO_CRYPTO_ENGINE_STEP_UP;
    err = AliroDocument::Verify(response.get(), responseSize, authentication, proof);
    if (err == ESP_ERR_INVALID_RESPONSE) {
        status = ESP_ALIRO_READER_STATUS_INVALID_ACCESS_RIGHTS;
    }
    return err;
}

esp_aliro_reader_status_t AuthorizeAndUnlock(esp_aliro_session_handle_t session, const AliroProof &proof,
                                             DoorLockStorage::Match &match, OperationErrorEnum &error)
{
    DoorLockStorage &store = DoorLockStorage::Instance();
    if (!BoltLockManager::Instance().IsLocked()) {
        ESP_LOGW(TAG, "Unlock not attempted: lock is not fully locked");
        return ESP_ALIRO_READER_STATUS_UNKNOWN;
    }
    if (!store.Ready() || store.Generation() != proof.generation) {
        ESP_LOGW(TAG, "Unlock not attempted: stale authentication result");
        return ESP_ALIRO_READER_STATUS_UNKNOWN;
    }
    // Identity was resolved under this generation; only this operation may mutate
    // storage until actuator acceptance, while the CHIP lock remains held.
    match = proof.credential;
    if (proof.issuerPath) {
        if (!AliroDocument::CurrentTimeValid(proof)) {
            ESP_LOGW(TAG, "Unlock not attempted: document time no longer valid");
            return ESP_ALIRO_READER_STATUS_UNKNOWN;
        }
        if (proof.hasIteration && !store.LearnIteration(match.credentialIndex, proof.iteration)) {
            ESP_LOGE(TAG, "Failed to persist the document access iteration");
            return ESP_ALIRO_READER_STATUS_HARDWARE_ISSUE;
        }
        if (!proof.capabilityAllowed) {
            ESP_LOGW(TAG, "Denied: unsupported/denied Matter AccessData");
            return ESP_ALIRO_READER_STATUS_INVALID_ACCESS_RIGHTS;
        }
    }
    if (!DoorLockAuthorization::Authorize(match, OperationSourceEnum::kAliro, error)) {
        return ESP_ALIRO_READER_STATUS_INVALID_ACCESS_RIGHTS;
    }

    // Authorization linearizes at synchronous actuator acceptance under the
    // CHIP lock held by the NFC task.
    esp_err_t committed = esp_aliro_session_commit_fast_transaction(session);
    if (committed != ESP_OK) {
        ESP_LOGE(TAG, "Fast-state commit failed: %d", committed);
    }
    // Policy is serialized by CHIP. Recheck time after the SDK commit, before
    // actuator acceptance. A failed commit may also have partially changed fast state.
    if (committed != ESP_OK || (proof.issuerPath && !AliroDocument::CurrentTimeValid(proof))) {
        if (AliroDoorLockDelegate::Instance().ClearFast() != ESP_OK) {
            store.Unavailable("denied-fast-clear");
        }
        return committed != ESP_OK ? ESP_ALIRO_READER_STATUS_HARDWARE_ISSUE : ESP_ALIRO_READER_STATUS_UNKNOWN;
    }

    BoltLockManager::Attribution attribution;
    attribution.user = match.userIndex;
    attribution.credential = CredentialStruct{match.type, match.credentialIndex};
    bool ok = BoltLockManager::Instance().Unlock(OperationSourceEnum::kAliro, attribution);
    ESP_LOGI(TAG, "Actuator %s: path=%s user=%u credential=%u:%u", ok ? "accepted" : "rejected",
             proof.issuerPath ? "issuer" : "endpoint", match.userIndex, static_cast<unsigned>(match.type),
             match.credentialIndex);
    if (!ok) {
        if (AliroDoorLockDelegate::Instance().ClearFast() != ESP_OK) {
            store.Unavailable("rejected-fast-clear");
        }
        return ESP_ALIRO_READER_STATUS_HARDWARE_ISSUE;
    }
    // Learning is optional and must not grant a reusable credential for a denied
    // tap. Keep this operation attributed to the issuer that authorized it.
    if (proof.issuerPath) {
        DoorLockStorage::Match learned;
        if (!store.Enroll(match.userIndex, proof.endpointKey, learned)) {
            ESP_LOGW(TAG, "Endpoint learning failed after actuator acceptance");
        }
    }
    return ESP_ALIRO_READER_STATUS_SECURED;
}

} // namespace

void AliroDoorLockDelegate::ReaderTask(void *context)
{
    AliroDoorLockDelegate &delegate = *static_cast<AliroDoorLockDelegate *>(context);
    auto &platform = DeviceLayer::PlatformMgr();
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(100));
        if (!platform.TryLockChipStack()) {
            continue;
        }
        const bool canPoll = BoltLockManager::Instance().IsLocked() && DoorLockStorage::Instance().Ready();
        platform.UnlockChipStack();
        // Removal probes can also wake a peer: suppress both probes and activation.
        if (!canPoll) {
            continue;
        }
        m5nfc_update();
        if (!m5nfc_activate()) {
            continue;
        }
        // A busy stack must not turn this tap into deferred work.
        if (!platform.TryLockChipStack()) {
            m5nfc_deactivate();
            continue;
        }
        esp_aliro_reader_handle_t reader = 0;
        if (BoltLockManager::Instance().IsLocked() && DoorLockStorage::Instance().Ready() &&
                !delegate.mBusy && !delegate.mFailed && delegate.m_aliro_reader) {
            delegate.mBusy = true;
            reader = delegate.m_aliro_reader;
        }
        platform.UnlockChipStack();
        if (!reader) {
            m5nfc_deactivate();
            continue;
        }

        esp_aliro_session_handle_t session = 0;
        esp_aliro_crypto_engine_type_t engine = ESP_ALIRO_CRYPTO_ENGINE_EXPEDITED;
        esp_aliro_reader_status_t status = ESP_ALIRO_READER_STATUS_UNKNOWN;
        AliroProof proof;
        DoorLockStorage::Match match;
        OperationErrorEnum error = OperationErrorEnum::kInvalidCredential;
        esp_err_t err = Authenticate(reader, session, proof, engine, status);
        if (err == ESP_OK) {
            error = OperationErrorEnum::kRestricted;
            if (platform.TryLockChipStack()) {
                status = AuthorizeAndUnlock(session, proof, match, error);
                platform.UnlockChipStack();
            } else {
                ESP_LOGW(TAG, "Unlock not attempted: CHIP stack busy");
            }
        } else {
            ESP_LOGW(TAG, "Authentication/document processing failed: %d", err);
        }
        const bool accepted = status == ESP_ALIRO_READER_STATUS_SECURED;
        if (session) {
            // A queued SDK authentication rejection overrides UNKNOWN and uses the expedited engine.
            const esp_err_t finish = esp_aliro_session_finish(session, m5nfc_message_exchange, engine, status);
            if (finish == ESP_OK) {
                ESP_LOGI(TAG, "Final EXCHANGE completed (requested Reader Status 0x%04x)",
                         static_cast<unsigned>(status));
            } else if (err == ESP_OK || finish != ESP_ERR_INVALID_STATE) {
                ESP_LOGW(TAG, "Final EXCHANGE failed: %d%s", finish,
                         accepted ? " after actuator acceptance (not rolled back)" : "");
            }
        }

        ESP_LOGI(TAG, "NFC minimum stack free=%u bytes heap free=%u minimum=%u bytes",
                 static_cast<uint32_t>(uxTaskGetStackHighWaterMark(nullptr)),
                 static_cast<uint32_t>(esp_get_free_heap_size()),
                 static_cast<uint32_t>(esp_get_minimum_free_heap_size()));
        esp_err_t deleted = session ? esp_aliro_session_delete(&session) : ESP_OK;
        {
            // Waiting here only reports failure/releases ownership; it cannot retry an unlock.
            esp_matter::lock::ScopedChipStackLock lock(portMAX_DELAY);
            if (!accepted) {
                DoorLockAuthorization::ReportUnlockFailure(match, OperationSourceEnum::kAliro, error);
            }
            if (deleted == ESP_OK) {
                delegate.mBusy = false;
            } else {
                ESP_LOGE(TAG, "Session deletion failed; reader quarantined until reboot");
                delegate.mFailed = true;
                DoorLockStorage::Instance().Unavailable("session-cleanup");
            }
        }
        m5nfc_deactivate();
    }
}
