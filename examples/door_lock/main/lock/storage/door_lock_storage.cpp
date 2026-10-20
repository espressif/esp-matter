/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "door_lock_storage.h"
#if CONFIG_ENABLE_ALIRO_OVER_NFC
#include "aliro_delegate.h"
#endif // CONFIG_ENABLE_ALIRO_OVER_NFC
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_log.h>
#include <app/EventLogging.h>
extern "C" {
#include <mbedtls/constant_time.h>
}

using namespace chip;
using namespace chip::app::Clusters::DoorLock;

namespace {

const char *TAG = "door_lock_storage";
constexpr char kMetadataKey[] = "dl/meta";

uint16_t FirstIndex(CredentialTypeEnum type)
{
    return type == CredentialTypeEnum::kProgrammingPIN ? 0 : 1;
}

bool IsEndpoint(CredentialTypeEnum type)
{
    return type == CredentialTypeEnum::kAliroEvictableEndpointKey || type == CredentialTypeEnum::kAliroNonEvictableEndpointKey;
}

void UserKey(uint16_t index, char (&key)[24])
{
    snprintf(key, sizeof(key), "dl/u/%u", index);
}

void CredentialKey(CredentialTypeEnum type, uint16_t index, char (&key)[24])
{
    snprintf(key, sizeof(key), "dl/c/%u/%u", static_cast<unsigned>(type), index);
}

void EnrollmentEvent(EndpointId endpoint, DataOperationTypeEnum operation, uint16_t user, uint16_t index)
{
    Events::LockUserChange::Type event;
    event.lockDataType = LockDataTypeEnum::kAliroEvictableEndpointKey;
    event.dataOperationType = operation;
    event.operationSource = OperationSourceEnum::kAliro;
    event.userIndex.SetNonNull(user);
    event.dataIndex.SetNonNull(index);
    EventNumber number;
    CHIP_ERROR err = app::LogEvent(event, endpoint, number);
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Enrollment event failed: %" CHIP_ERROR_FORMAT, err.Format());
    }
}

struct Metadata {
    uint32_t magic = 0x444C4958;
    uint32_t version = 2;
    uint16_t users = 0;
    uint8_t perUser = 0;
    uint16_t slots[DoorLockStorage::kCredentialTypeCount] = {};
};

} // namespace

bool DoorLockStorage::Invalidate(const char *reason)
{
    ++mGeneration;
#if CONFIG_ENABLE_ALIRO_OVER_NFC
    if (AliroDoorLockDelegate::Instance().ClearFast() != ESP_OK) {
        mReady = false;
        ESP_LOGE(TAG, "Fast-cache invalidation failed: %s", reason);
        return false;
    }
#endif // CONFIG_ENABLE_ALIRO_OVER_NFC
    return mReady;
}

void DoorLockStorage::Unavailable(const char *reason)
{
    mReady = false;
    Invalidate(reason);
    ESP_LOGE(TAG, "Credential authorization unavailable: %s", reason);
}

CHIP_ERROR DoorLockStorage::Init(EndpointId endpoint, const Limits &limits, PersistentStorageDelegate &storage)
{
    mReady = false;
    DiscardPendingUser();
    mStorage = &storage;
    mEndpoint = endpoint;
    mLimits = limits;
    if (!CheckLimits()) {
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
    ++mGeneration;
    return Load() ? CHIP_NO_ERROR : CHIP_ERROR_INCORRECT_STATE;
}

bool DoorLockStorage::Load()
{
    Metadata expected, stored;
    expected.users = mLimits.numberOfUsers;
    expected.perUser = mLimits.numberOfCredentialsPerUser;
    memcpy(expected.slots, mLimits.credentialSlotsByType, sizeof(expected.slots));
    uint16_t size = sizeof(stored);
    CHIP_ERROR err = mStorage->SyncGetKeyValue(kMetadataKey, &stored, size);
    mReady = true;
    if (err == CHIP_ERROR_PERSISTED_STORAGE_VALUE_NOT_FOUND) {
        for (uint16_t i = 1; mReady && i <= mLimits.numberOfUsers; ++i) {
            UserRecord user;
            if (!ReadUser(i, user) || user.status != UserStatusEnum::kAvailable) {
                Unavailable("metadata-missing");
            }
        }
        for (uint8_t t = 0; mReady && t < kCredentialTypeCount; ++t) {
            auto type = static_cast<CredentialTypeEnum>(t);
            for (uint16_t n = 0; mReady && n < mLimits.credentialSlotsByType[t]; ++n) {
                CredentialRecord credential;
                if (!ReadCredential(type, n + FirstIndex(type), credential) ||
                        credential.status != DlCredentialStatus::kAvailable) {
                    Unavailable("metadata-missing");
                }
            }
        }
        err = mReady ? mStorage->SyncSetKeyValue(kMetadataKey, &expected, sizeof(expected)) : CHIP_ERROR_INCORRECT_STATE;
    } else if (err == CHIP_ERROR_BUFFER_TOO_SMALL ||
               (err == CHIP_NO_ERROR && (size != sizeof(stored) || stored.version != expected.version))) {
        ESP_LOGE(TAG, "Storage version/format mismatch; migration is not supported on this demo");
        abort();
    } else if (err == CHIP_NO_ERROR && (stored.magic != expected.magic || stored.users != expected.users ||
                                        stored.perUser != expected.perUser || memcmp(stored.slots, expected.slots, sizeof(stored.slots)) != 0)) {
        err = CHIP_ERROR_INVALID_ARGUMENT;
    }
    mReady = mReady && err == CHIP_NO_ERROR;
    if (mReady) {
        ValidateDatabase();
    }
    if (!mReady) {
        Unavailable("load-failed");
    }
    return mReady;
}

bool DoorLockStorage::ValidateDatabase()
{
    DiscardPendingUser();
    for (uint16_t i = 1; i <= mLimits.numberOfUsers; ++i) {
        UserRecord user;
        if (!ReadUser(i, user)) {
            return false;
        }
    }

    // Count associations in scratch RAM instead of rereading all credentials for every user.
    // Bounds come from CheckLimits, not the configured user count.
    uint16_t owners[kCredentialTypeCount * kMaxCredentialSlots];
    size_t occupied = 0;
    uint16_t endpoints = 0;
    for (uint8_t t = 0; t < kCredentialTypeCount; ++t) {
        auto type = static_cast<CredentialTypeEnum>(t);
        for (uint16_t n = 0; n < mLimits.credentialSlotsByType[t]; ++n) {
            CredentialRecord credential;
            if (!ReadCredential(type, n + FirstIndex(type), credential)) {
                return false;
            }
            if (credential.status == DlCredentialStatus::kAvailable) {
                continue;
            }
            if (IsEndpoint(type) && ++endpoints > DoorLockCapabilities::kAliroEndpointKeySlots) {
                Unavailable("endpoint-capacity");
                return false;
            }
            if (credential.ownerUserIndex == 0) {
                continue; // ProgrammingPIN without owner is not a user's association.
            }
            owners[occupied++] = credential.ownerUserIndex;
            size_t count = std::count(owners, owners + occupied, credential.ownerUserIndex);
            if (count > mLimits.numberOfCredentialsPerUser) {
                Unavailable("credential-capacity");
                return false;
            }
            if (count == 1) {
                UserRecord owner;
                if (!ReadUser(credential.ownerUserIndex, owner)) {
                    return false;
                }
                if (owner.status == UserStatusEnum::kAvailable) {
                    Unavailable("credential-owner");
                    return false;
                }
            }
        }
    }
    return mReady;
}

bool DoorLockStorage::CheckLimits() const
{
    if (mLimits.numberOfUsers == 0 || mLimits.numberOfUsers > kMaxUsers || mLimits.numberOfUsers == UINT16_MAX ||
            mLimits.numberOfCredentialsPerUser == 0 || mLimits.numberOfCredentialsPerUser > kMaxCredentialsPerUser) {
        return false;
    }
    for (uint8_t t = 0; t < kCredentialTypeCount; ++t) {
        uint16_t count = mLimits.credentialSlotsByType[t];
        auto type = static_cast<CredentialTypeEnum>(t);
        if (count > kMaxCredentialSlots || (type == CredentialTypeEnum::kProgrammingPIN && count > 1) ||
                (count != 0 && type != CredentialTypeEnum::kProgrammingPIN && type != CredentialTypeEnum::kPin &&
                 type != CredentialTypeEnum::kAliroCredentialIssuerKey && !IsEndpoint(type)) ||
                (type == CredentialTypeEnum::kAliroCredentialIssuerKey && count > DoorLockCapabilities::kAliroCredentialIssuerKeySlots) ||
                (IsEndpoint(type) && count > DoorLockCapabilities::kAliroEndpointKeySlots)) {
            return false;
        }
    }
    return true;
}

bool DoorLockStorage::IndexValid(CredentialTypeEnum type, uint16_t index) const
{
    unsigned t = static_cast<unsigned>(type);
    return t < kCredentialTypeCount && index >= FirstIndex(type) &&
           static_cast<unsigned>(index - FirstIndex(type)) < mLimits.credentialSlotsByType[t];
}

bool DoorLockStorage::CheckUser(const UserRecord &user) const
{
    if (user.nameLength > sizeof(user.name)) {
        return false;
    }
    if (user.status == UserStatusEnum::kAvailable) {
        return user.nameLength == 0;
    }
    return (user.status == UserStatusEnum::kOccupiedEnabled || user.status == UserStatusEnum::kOccupiedDisabled) &&
           static_cast<unsigned>(user.type) <= static_cast<unsigned>(UserTypeEnum::kRemoteOnlyUser) &&
           static_cast<unsigned>(user.rule) <= static_cast<unsigned>(CredentialRuleEnum::kTri);
}

bool DoorLockStorage::CheckCredential(CredentialTypeEnum type, const CredentialRecord &credential) const
{
    if (credential.status == DlCredentialStatus::kAvailable) {
        return credential.size == 0 && credential.ownerUserIndex == 0 && credential.iteration == 0;
    }
    if (credential.status != DlCredentialStatus::kOccupied || credential.size > sizeof(credential.data) ||
            credential.ownerUserIndex > mLimits.numberOfUsers ||
            (credential.ownerUserIndex == 0 && type != CredentialTypeEnum::kProgrammingPIN) ||
            (type != CredentialTypeEnum::kAliroCredentialIssuerKey && credential.iteration != 0)) {
        return false;
    }
    if (type == CredentialTypeEnum::kProgrammingPIN || type == CredentialTypeEnum::kPin) {
        if (credential.size < 4 || credential.size > 16) {
            return false;
        }
        for (uint8_t i = 0; i < credential.size; ++i) {
            if (credential.data[i] < '0' || credential.data[i] > '9') {
                return false;
            }
        }
        return true;
    }
    return credential.size == 65 && credential.data[0] == 0x04 &&
           (type == CredentialTypeEnum::kAliroCredentialIssuerKey || IsEndpoint(type));
}

bool DoorLockStorage::Read(const char *key, void *record, size_t expectedSize)
{
    if (!mReady) {
        return false;
    }
    uint16_t size = expectedSize;
    CHIP_ERROR err = mStorage->SyncGetKeyValue(key, record, size);
    if (err == CHIP_ERROR_PERSISTED_STORAGE_VALUE_NOT_FOUND) {
        return true;
    }
    if (err != CHIP_NO_ERROR || size != expectedSize) {
        Unavailable("read-failed-or-malformed");
        return false;
    }
    return true;
}

bool DoorLockStorage::SaveRecord(const char *key, const void *record, size_t size)
{
    if (!mReady || !Invalidate(key)) {
        return false;
    }
    CHIP_ERROR err = record ? mStorage->SyncSetKeyValue(key, record, size) : mStorage->SyncDeleteKeyValue(key);
    if (err != CHIP_NO_ERROR && (record || err != CHIP_ERROR_PERSISTED_STORAGE_VALUE_NOT_FOUND)) {
        Unavailable("record-write-failed");
        return false;
    }
    return true;
}

bool DoorLockStorage::ReadUser(uint16_t index, UserRecord &out)
{
    out = {};
    if (!mReady || index == 0 || index > mLimits.numberOfUsers) {
        return false;
    }
    if (index == mPendingUserIndex) {
        out = mPendingUser;
        return true;
    }
    char key[24];
    UserKey(index, key);
    if (!Read(key, &out, sizeof(out))) {
        return false;
    }
    if (!CheckUser(out)) {
        Unavailable("user-malformed");
        return false;
    }
    return true;
}

bool DoorLockStorage::ReadCredential(CredentialTypeEnum type, uint16_t index, CredentialRecord &out)
{
    out = {};
    if (!IndexValid(type, index)) {
        return false;
    }
    char key[24];
    CredentialKey(type, index, key);
    if (!Read(key, &out, sizeof(out))) {
        return false;
    }
    if (!CheckCredential(type, out)) {
        Unavailable("credential-malformed");
        return false;
    }
    return true;
}

bool DoorLockStorage::UserCredentials(uint16_t owner, CredentialStruct(&out)[kMaxCredentialsPerUser], uint8_t &count)
{
    count = 0;
    for (uint8_t t = 0; t < kCredentialTypeCount; ++t) {
        auto type = static_cast<CredentialTypeEnum>(t);
        for (uint16_t n = 0; n < mLimits.credentialSlotsByType[t]; ++n) {
            uint16_t index = n + FirstIndex(type);
            CredentialRecord credential;
            if (!ReadCredential(type, index, credential)) {
                return false;
            }
            if (credential.status == DlCredentialStatus::kOccupied && credential.ownerUserIndex == owner) {
                if (count == mLimits.numberOfCredentialsPerUser) {
                    Unavailable("user-capacity");
                    return false;
                }
                out[count++] = {type, index};
            }
        }
    }
    return mReady;
}

bool DoorLockStorage::ReadEndpointSlots(EndpointSlots &out)
{
    out = {};
    for (auto type : {
                CredentialTypeEnum::kAliroEvictableEndpointKey, CredentialTypeEnum::kAliroNonEvictableEndpointKey
            }) {
        for (uint16_t i = 1; i <= mLimits.credentialSlotsByType[static_cast<unsigned>(type)]; ++i) {
            CredentialRecord credential;
            if (!ReadCredential(type, i, credential)) {
                return false;
            }
            out.occupied += credential.status == DlCredentialStatus::kOccupied;
            if (type == CredentialTypeEnum::kAliroEvictableEndpointKey && credential.status == DlCredentialStatus::kAvailable && out.freeEvictable == 0) {
                out.freeEvictable = i;
            }
        }
    }
    return mReady;
}

bool DoorLockStorage::WriteUser(uint16_t index, const UserRecord &user)
{
    char key[24];
    UserKey(index, key);
    return user.status == UserStatusEnum::kAvailable ? SaveRecord(key, nullptr, 0) : SaveRecord(key, &user, sizeof(user));
}

bool DoorLockStorage::GetUser(uint16_t index, EmberAfPluginDoorLockUserInfo &out)
{
    UserView &view = mUserViews[mNextUserView++ % 2];
    if (!ReadUser(index, view.user) || !UserCredentials(index, view.credentials, view.count)) {
        return false;
    }
    if (index == mPendingUserIndex) {
        view.credentials[view.count++] = mPendingCredential;
    }
    out.userStatus = view.user.status;
    out.userName = CharSpan(view.user.name, view.user.nameLength);
    out.userUniqueId = view.user.uniqueId;
    out.userType = view.user.type;
    out.credentialRule = view.user.rule;
    out.createdBy = view.user.createdBy;
    out.lastModifiedBy = view.user.modifiedBy;
    out.credentials = Span<const CredentialStruct>(view.credentials, view.count);
    out.creationSource = DlAssetSource::kMatterIM;
    out.modificationSource = DlAssetSource::kMatterIM;
    return true;
}

bool DoorLockStorage::SetUser(uint16_t index, FabricIndex creator, FabricIndex modifier, const CharSpan &name,
                              uint32_t uniqueId, UserStatusEnum status, UserTypeEnum type, CredentialRuleEnum rule,
                              const CredentialStruct *credentials, size_t count)
{
    if (!mReady || index == 0 || index > mLimits.numberOfUsers || name.size() > kMaxUserNameSize ||
            count > mLimits.numberOfCredentialsPerUser || (count != 0 && credentials == nullptr) ||
            (status == UserStatusEnum::kAvailable && count != 0) ||
            (mPendingUserIndex != 0 && mPendingUserIndex != index)) {
        return false;
    }
    UserRecord user;
    if (status != UserStatusEnum::kAvailable) {
        user.status = status;
        user.type = type;
        user.rule = rule;
        user.uniqueId = uniqueId;
        user.createdBy = creator;
        user.modifiedBy = modifier;
        user.nameLength = name.size();
        if (user.nameLength != 0) {
            memcpy(user.name, name.data(), user.nameLength);
        }
    }
    if (!CheckUser(user)) {
        return false;
    }
    CredentialStruct owned[kMaxCredentialsPerUser];
    uint8_t remaining;
    if (!UserCredentials(index, owned, remaining)) {
        return false;
    }
    bool pending = false;
    CredentialStruct proposed;
    for (size_t i = 0; i < count; ++i) {
        // Consume each existing association once; the SDK may propose one new, empty slot.
        auto end = owned + remaining;
        auto existing = std::find_if(owned, end, [&](const CredentialStruct & ref) {
            return ref.credentialType == credentials[i].credentialType &&
                   ref.credentialIndex == credentials[i].credentialIndex;
        });
        if (existing != end) {
            *existing = owned[--remaining];
            continue;
        }
        CredentialRecord credential;
        if (pending || !ReadCredential(credentials[i].credentialType, credentials[i].credentialIndex, credential) ||
                credential.status != DlCredentialStatus::kAvailable) {
            return false;
        }
        proposed = credentials[i];
        pending = true;
    }
    if (remaining != 0) {
        return false; // Omitting an existing association must not silently detach it.
    }
    if (pending) {
        mPendingUser = user;
        mPendingCredential = proposed;
        mPendingUserIndex = index;
        return true;
    }
    DiscardPendingUser();
    return WriteUser(index, user);
}

bool DoorLockStorage::GetCredential(uint16_t index, CredentialTypeEnum type, EmberAfPluginDoorLockCredentialInfo &out)
{
    if (!ReadCredential(type, index, mCredentialView)) {
        return false;
    }
    out.status = mCredentialView.status;
    out.credentialType = type;
    out.credentialData = ByteSpan(mCredentialView.data, mCredentialView.size);
    out.createdBy = mCredentialView.createdBy;
    out.lastModifiedBy = mCredentialView.modifiedBy;
    out.creationSource = DlAssetSource::kMatterIM;
    out.modificationSource = DlAssetSource::kMatterIM;
    return true;
}

bool DoorLockStorage::SetCredential(uint16_t index, FabricIndex creator, FabricIndex modifier, DlCredentialStatus status,
                                    CredentialTypeEnum type, const ByteSpan &data)
{
    CredentialRecord credential;
    if ((status != DlCredentialStatus::kAvailable && status != DlCredentialStatus::kOccupied) ||
            data.size() > sizeof(credential.data) || (status == DlCredentialStatus::kOccupied && data.empty()) ||
            !ReadCredential(type, index, credential) ||
            (mPendingUserIndex != 0 && (mPendingCredential.credentialType != type || mPendingCredential.credentialIndex != index))) {
        return false;
    }
    char key[24];
    CredentialKey(type, index, key);
    if (status == DlCredentialStatus::kAvailable) {
        if (credential.status == DlCredentialStatus::kOccupied && credential.ownerUserIndex != 0) {
            // SDK ClearUser deletes credentials first; disable the owner before interruptible removal.
            UserRecord owner;
            if (!ReadUser(credential.ownerUserIndex, owner)) {
                return false;
            }
            if (owner.status == UserStatusEnum::kAvailable) {
                Unavailable("credential-owner");
                return false;
            }
            if (owner.status != UserStatusEnum::kOccupiedDisabled) {
                owner.status = UserStatusEnum::kOccupiedDisabled;
                if (!WriteUser(credential.ownerUserIndex, owner)) {
                    return false;
                }
            }
        }
        bool saved = SaveRecord(key, nullptr, 0);
        if (saved) {
            DiscardPendingUser();
        }
        return saved;
    }

    bool newSlot = credential.status == DlCredentialStatus::kAvailable;
    if (mPendingUserIndex != 0) {
        if (!newSlot) {
            return false;
        }
        // SetUser already validated the complete association list and per-user capacity.
        credential.ownerUserIndex = mPendingUserIndex;
    }

    // Attribution-only updates preserve an issuer's learned anti-rollback floor.
    if (credential.size != data.size() || mbedtls_ct_memcmp(credential.data, data.data(), data.size()) != 0) {
        credential.iteration = 0;
    }
    credential.status = status;
    credential.createdBy = creator;
    credential.modifiedBy = modifier;
    credential.size = data.size();
    memcpy(credential.data, data.data(), data.size());
    if (!CheckCredential(type, credential)) {
        return false;
    }
    if (credential.ownerUserIndex != 0) {
        UserRecord owner;
        if (!ReadUser(credential.ownerUserIndex, owner) || owner.status == UserStatusEnum::kAvailable) {
            return false;
        }
    }

    if (newSlot && IsEndpoint(type)) {
        EndpointSlots slots;
        if (!ReadEndpointSlots(slots) || slots.occupied >= DoorLockCapabilities::kAliroEndpointKeySlots) {
            return false;
        }
    }

    if (mPendingUserIndex != 0 && !WriteUser(mPendingUserIndex, mPendingUser)) {
        return false;
    }
    bool saved = SaveRecord(key, &credential, sizeof(credential));
    if (saved) {
        DiscardPendingUser();
    }
    return saved;
}

bool DoorLockStorage::FindCredentialByData(const ByteSpan &data, std::initializer_list<CredentialTypeEnum> types,
                                           Match &out, bool &found)
{
    out = {};
    found = false;
    for (auto type : types) {
        if (static_cast<unsigned>(type) >= kCredentialTypeCount) {
            return false;
        }
        for (uint16_t n = 0; n < mLimits.credentialSlotsByType[static_cast<unsigned>(type)]; ++n) {
            uint16_t index = n + FirstIndex(type);
            CredentialRecord credential;
            if (!ReadCredential(type, index, credential)) {
                return false;
            }
            if (credential.status == DlCredentialStatus::kOccupied && credential.size == data.size() &&
                    mbedtls_ct_memcmp(credential.data, data.data(), data.size()) == 0) {
                if (found) {
                    return false;
                }
                found = true;
                out = {credential.ownerUserIndex, index, type};
            }
        }
    }
    return mReady;
}

bool DoorLockStorage::ValidatePIN(const ByteSpan &pin, Match &out)
{
    bool found;
    return FindCredentialByData(pin, {CredentialTypeEnum::kPin}, out, found) && found;
}

bool DoorLockStorage::FindEndpoint(const uint8_t key[65], Match &out, bool &found)
{
    return FindCredentialByData(ByteSpan(key, 65), {
        CredentialTypeEnum::kAliroEvictableEndpointKey, CredentialTypeEnum::kAliroNonEvictableEndpointKey
    }, out, found);
}

bool DoorLockStorage::LearnIteration(uint16_t issuerIndex, uint64_t iteration)
{
    CredentialRecord issuer;
    if (!ReadCredential(CredentialTypeEnum::kAliroCredentialIssuerKey, issuerIndex, issuer) ||
            issuer.status != DlCredentialStatus::kOccupied) {
        return false;
    }
    if (iteration <= issuer.iteration) {
        return true;
    }
    issuer.iteration = iteration;
    char key[24];
    CredentialKey(CredentialTypeEnum::kAliroCredentialIssuerKey, issuerIndex, key);
    return SaveRecord(key, &issuer, sizeof(issuer));
}

bool DoorLockStorage::Enroll(uint16_t ownerIndex, const uint8_t key[65], Match &out)
{
    UserRecord user;
    CredentialStruct owned[kMaxCredentialsPerUser];
    uint8_t ownedCount;
    if (mPendingUserIndex != 0 || !ReadUser(ownerIndex, user) || user.status != UserStatusEnum::kOccupiedEnabled ||
            user.type != UserTypeEnum::kUnrestrictedUser || user.rule != CredentialRuleEnum::kSingle ||
            !UserCredentials(ownerIndex, owned, ownedCount)) {
        return false;
    }
    Match existing;
    bool found;
    if (!FindEndpoint(key, existing, found)) {
        return false;
    }
    if (found) {
        out = existing;
        return out.userIndex == ownerIndex;
    }
    EndpointSlots slots;
    if (!ReadEndpointSlots(slots)) {
        return false;
    }
    uint16_t freeSlot = slots.freeEvictable, victim = 0;
    if (slots.occupied >= DoorLockCapabilities::kAliroEndpointKeySlots ||
            ownedCount == mLimits.numberOfCredentialsPerUser || freeSlot == 0) {
        for (uint8_t i = 0; i < ownedCount; ++i) {
            if (owned[i].credentialType == CredentialTypeEnum::kAliroEvictableEndpointKey) {
                victim = owned[i].credentialIndex;
                break;
            }
        }
        if (victim == 0) {
            return true; // Document authorization does not depend on retaining an endpoint key.
        }
        freeSlot = victim;
    }
    CredentialRecord credential;
    credential.status = DlCredentialStatus::kOccupied;
    credential.ownerUserIndex = ownerIndex;
    credential.createdBy = user.createdBy;
    credential.modifiedBy = user.modifiedBy;
    credential.size = 65;
    memcpy(credential.data, key, 65);
    char storageKey[24];
    CredentialKey(CredentialTypeEnum::kAliroEvictableEndpointKey, freeSlot, storageKey);
    if (!CheckCredential(CredentialTypeEnum::kAliroEvictableEndpointKey, credential) ||
            !SaveRecord(storageKey, &credential, sizeof(credential))) {
        return false;
    }
    if (victim != 0) {
        EnrollmentEvent(mEndpoint, DataOperationTypeEnum::kClear, ownerIndex, victim);
    }
    EnrollmentEvent(mEndpoint, DataOperationTypeEnum::kAdd, ownerIndex, freeSlot);
    out = {ownerIndex, freeSlot, CredentialTypeEnum::kAliroEvictableEndpointKey};
    return true;
}
