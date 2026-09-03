/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "rfid_door_lock_handler.h"

#include "bolt_lock_manager.h"
#include "door_lock_storage.h"

#include <rfid_nfc_reader.h>

#include <esp_log.h>
#include <lib/support/BytesToHex.h>
#include <platform/PlatformManager.h>

#include <cstring>

using namespace chip;
using namespace chip::app::Clusters::DoorLock;

namespace {
const char * TAG = "rfid_door_lock";

// Heap-allocated so it can safely cross from the NFC polling task, which
// reports the tag, to the Matter event loop, which is the only context
// allowed to touch door-lock/BoltLockManager state.
struct DetectedTag {
    uint8_t uid[10];
    size_t uidLength;
};

// Renders a UID as an upper-case hex string with no separators (e.g.
// "04A1B2C3") into a caller-owned buffer, matching the format
// `matter esp dl rfid-unlock` expects and the raw bytes a controller's
// SetCredential command should carry as CredentialData -- so an installer
// can copy this straight out of the log when provisioning a new tag. Takes
// an explicit output buffer (rather than a shared static one) since callers
// run on different tasks (the NFC polling task and the Matter event loop).
void UidToHex(const uint8_t * uid, size_t uidLength, char * out, size_t outSize)
{
    if (Encoding::BytesToUppercaseHexString(uid, uidLength, out, outSize) != CHIP_NO_ERROR) {
        out[0] = 0;
    }
}

void UnlockIfCredentialMatches(intptr_t arg)
{
    auto * tag = reinterpret_cast<DetectedTag *>(arg);
    ByteSpan uidSpan(tag->uid, tag->uidLength);

    DoorLockStorage::CredentialMatch match;
    if (!DoorLockStorage::Instance().ValidateRfid(uidSpan, match)) {
        // Not provisioned: log the UID as hex so it can be pasted into a
        // Matter controller's SetCredential command (CredentialType=RFID)
        // to enroll this tag.
        char hex[2 * sizeof(DetectedTag::uid) + 1];
        UidToHex(tag->uid, tag->uidLength, hex, sizeof(hex));
        ESP_LOGW(TAG, "Unlock rejected: source=rfid reason=invalid-credential uid=%s", hex);
        delete tag;
        return;
    }

    BoltLockManager::CredentialMatch credential{ true, match.userIndex, match.credentialIndex,
                                                 CredentialTypeEnum::kRfid };
    if (!BoltLockManager::Instance().Unlock(OperationSourceEnum::kRfid, credential)) {
        ESP_LOGE(TAG, "Unlock rejected: source=rfid user=%u credential=rfid:%u reason=actuator", match.userIndex,
                 match.credentialIndex);
    } else {
        ESP_LOGW(TAG, "Unlock event: source=rfid user=%u credential=rfid:%u", match.userIndex, match.credentialIndex);
    }
    delete tag;
}

// Runs on the NFC polling task's context. Copies the tag identifier and
// hops onto the Matter event loop before touching any door-lock state.
void HandleRfidTagDetected(const uint8_t * uid, size_t uidLength)
{
    if (uidLength == 0 || uidLength > sizeof(DetectedTag::uid)) {
        ESP_LOGW(TAG, "Ignoring tag with unsupported UID length %u", static_cast<unsigned>(uidLength));
        return;
    }
    char hex[2 * sizeof(DetectedTag::uid) + 1];
    UidToHex(uid, uidLength, hex, sizeof(hex));
    ESP_LOGI(TAG, "Tag detected uid=%s", hex);
    auto * tag = new DetectedTag;
    tag->uidLength = uidLength;
    memcpy(tag->uid, uid, uidLength);
    CHIP_ERROR err = DeviceLayer::PlatformMgr().ScheduleWork(UnlockIfCredentialMatches, reinterpret_cast<intptr_t>(tag));
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to schedule RFID credential check: %" CHIP_ERROR_FORMAT, err.Format());
        delete tag;
    }
}

} // namespace

esp_err_t rfid_door_lock_init()
{
    return RfidNfcReader::Instance().Init(HandleRfidTagDetected);
}
