/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "door_lock_storage.h"

#include <esp_log.h>
#include <lib/support/CodeUtils.h>

#include <algorithm>
#include <stdio.h>
#include <string.h>

using namespace chip;
using namespace chip::app::Clusters::DoorLock;

namespace {
const char * TAG = "door_lock_storage";
constexpr char kMetadataKey[] = "dl/meta";

bool IsValidUserStatus(UserStatusEnum status)
{
    return status == UserStatusEnum::kAvailable || status == UserStatusEnum::kOccupiedEnabled ||
           status == UserStatusEnum::kOccupiedDisabled;
}

bool IsValidUserType(UserTypeEnum type)
{
    return to_underlying(type) <= to_underlying(UserTypeEnum::kRemoteOnlyUser);
}

bool IsValidCredentialRule(CredentialRuleEnum rule)
{
    return to_underlying(rule) <= to_underlying(CredentialRuleEnum::kTri);
}

bool IsValidCredentialType(CredentialTypeEnum type)
{
    return to_underlying(type) <= to_underlying(CredentialTypeEnum::kAliroNonEvictableEndpointKey);
}

bool FormatKey(char * key, size_t keySize, const char * format, unsigned first, unsigned second = 0)
{
    int length = snprintf(key, keySize, format, first, second);
    return length > 0 && static_cast<size_t>(length) < keySize &&
           static_cast<size_t>(length) <= PersistentStorageDelegate::kKeyLengthMax;
}
} // namespace

CHIP_ERROR DoorLockStorage::Init(const Config  &config)
{
    const Limits  &limits = config.limits;
    VerifyOrReturnError(limits.numberOfUsers > 0 && limits.numberOfUsers <= kMaxUsers, CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(limits.numberOfCredentialsPerUser > 0 &&
                        limits.numberOfCredentialsPerUser <= kMaxCredentialsPerUser,
                        CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(limits.numberOfWeekdaySchedulesPerUser <= kMaxSchedulesPerUser,
                        CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(limits.numberOfYeardaySchedulesPerUser <= kMaxSchedulesPerUser,
                        CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(limits.numberOfHolidaySchedules <= kMaxHolidaySchedules,
                        CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(limits.credentialSlotsByType[to_underlying(CredentialTypeEnum::kProgrammingPIN)] == 1,
                        CHIP_ERROR_INVALID_ARGUMENT);
    for (uint16_t slotCount : limits.credentialSlotsByType) {
        VerifyOrReturnError(slotCount <= kMaxCredentialSlotsPerType, CHIP_ERROR_INVALID_ARGUMENT);
    }

    mInitialized = false;
    mStorage = &config.defaultStorage;
    mLimits = limits;
    std::fill_n(mUsers, kMaxUsers, UserRecord());
    std::fill_n(mCredentials, kCredentialSlotCount, CredentialRecord());
    for (size_t userIndex = 0; userIndex < kMaxUsers; ++userIndex) {
        std::fill_n(mWeekdaySchedules[userIndex], kMaxSchedulesPerUser,
                    ScheduleRecord<EmberAfPluginDoorLockWeekDaySchedule>());
        std::fill_n(mYeardaySchedules[userIndex], kMaxSchedulesPerUser,
                    ScheduleRecord<EmberAfPluginDoorLockYearDaySchedule>());
    }
    std::fill_n(mHolidaySchedules, kMaxHolidaySchedules,
                ScheduleRecord<EmberAfPluginDoorLockHolidaySchedule>());
    for (size_t userIndex = 0; userIndex < kMaxUsers; ++userIndex) {
        std::fill_n(mUserCredentialViews[userIndex], kMaxCredentialsPerUser, CredentialStruct());
    }

    ReturnErrorOnFailure(LoadMetadata());
    ReturnErrorOnFailure(LoadRecords());
    ReturnErrorOnFailure(RepairRelationships());

    for (size_t i = 0; i < mLimits.numberOfUsers; ++i) {
        RebuildUserCredentialView(i);
    }
    mInitialized = true;
    return CHIP_NO_ERROR;
}

CHIP_ERROR DoorLockStorage::LoadMetadata()
{
    StorageMetadata metadata;
    bool found = false;
    ReturnErrorOnFailure(ReadRecord(kMetadataKey, &metadata, sizeof(metadata), found));

    if (!found) {
        metadata.numberOfUsers = mLimits.numberOfUsers;
        metadata.numberOfCredentialsPerUser = mLimits.numberOfCredentialsPerUser;
        return WriteRecord(kMetadataKey, &metadata, sizeof(metadata));
    }

    VerifyOrReturnError(metadata.magic == kStorageMagic, CHIP_ERROR_VERSION_MISMATCH);
    VerifyOrReturnError(metadata.version == kStorageVersion, CHIP_ERROR_VERSION_MISMATCH);
    VerifyOrReturnError(metadata.numberOfUsers == mLimits.numberOfUsers, CHIP_ERROR_VERSION_MISMATCH);
    VerifyOrReturnError(metadata.numberOfCredentialsPerUser == mLimits.numberOfCredentialsPerUser,
                        CHIP_ERROR_VERSION_MISMATCH);
    return CHIP_NO_ERROR;
}

CHIP_ERROR DoorLockStorage::LoadRecords()
{
    char key[PersistentStorageDelegate::kKeyLengthMax + 1];

    for (uint16_t userIndex = 1; userIndex <= mLimits.numberOfUsers; ++userIndex) {
        VerifyOrReturnError(MakeUserKey(userIndex, key, sizeof(key)), CHIP_ERROR_INVALID_ARGUMENT);
        bool found = false;
        ReturnErrorOnFailure(ReadRecord(key, &mUsers[userIndex - 1], sizeof(UserRecord), found));
        if (found && !IsValidUserRecord(mUsers[userIndex - 1])) {
            ESP_LOGE(TAG, "Invalid door lock user record at index %u", userIndex);
            return CHIP_ERROR_INVALID_ARGUMENT;
        }
    }

    for (uint8_t typeValue = 0; typeValue < kCredentialTypeCount; ++typeValue) {
        CredentialTypeEnum type = static_cast<CredentialTypeEnum>(typeValue);
        uint16_t slotCount = mLimits.credentialSlotsByType[typeValue];
        if (slotCount == 0) {
            continue;
        }
        uint16_t firstIndex = type == CredentialTypeEnum::kProgrammingPIN ? 0 : 1;
        uint16_t lastIndex = type == CredentialTypeEnum::kProgrammingPIN ? 0 : slotCount;
        for (uint16_t credentialIndex = firstIndex; credentialIndex <= lastIndex; ++credentialIndex) {
            uint16_t normalizedIndex = 0;
            VerifyOrReturnError(NormalizeCredentialIndex(credentialIndex, type, normalizedIndex), CHIP_ERROR_INVALID_ARGUMENT);
            size_t storageIndex = CredentialStorageIndex(normalizedIndex, type);
            VerifyOrReturnError(MakeCredentialKey(type, credentialIndex, key, sizeof(key)), CHIP_ERROR_INVALID_ARGUMENT);
            bool found = false;
            ReturnErrorOnFailure(ReadRecord(key, &mCredentials[storageIndex], sizeof(CredentialRecord), found));
            if (found && !IsValidCredentialRecord(mCredentials[storageIndex], type)) {
                ESP_LOGE(TAG, "Invalid door lock credential record type %u index %u", typeValue, credentialIndex);
                return CHIP_ERROR_INVALID_ARGUMENT;
            }
        }
    }

    for (uint16_t userIndex = 1; userIndex <= mLimits.numberOfUsers; ++userIndex) {
        for (uint8_t scheduleIndex = 1; scheduleIndex <= mLimits.numberOfWeekdaySchedulesPerUser; ++scheduleIndex) {
            VerifyOrReturnError(MakeScheduleKey("ws", scheduleIndex, userIndex, key, sizeof(key)), CHIP_ERROR_INVALID_ARGUMENT);
            bool found = false;
            ReturnErrorOnFailure(ReadRecord(key, &mWeekdaySchedules[userIndex - 1][scheduleIndex - 1],
                                            sizeof(mWeekdaySchedules[0][0]), found));
        }
        for (uint8_t scheduleIndex = 1; scheduleIndex <= mLimits.numberOfYeardaySchedulesPerUser; ++scheduleIndex) {
            VerifyOrReturnError(MakeScheduleKey("ys", scheduleIndex, userIndex, key, sizeof(key)), CHIP_ERROR_INVALID_ARGUMENT);
            bool found = false;
            ReturnErrorOnFailure(ReadRecord(key, &mYeardaySchedules[userIndex - 1][scheduleIndex - 1],
                                            sizeof(mYeardaySchedules[0][0]), found));
        }
    }
    for (uint8_t scheduleIndex = 1; scheduleIndex <= mLimits.numberOfHolidaySchedules; ++scheduleIndex) {
        VerifyOrReturnError(MakeScheduleKey("hs", scheduleIndex, 0, key, sizeof(key)), CHIP_ERROR_INVALID_ARGUMENT);
        bool found = false;
        ReturnErrorOnFailure(ReadRecord(key, &mHolidaySchedules[scheduleIndex - 1], sizeof(mHolidaySchedules[0]), found));
    }

    return CHIP_NO_ERROR;
}

CHIP_ERROR DoorLockStorage::RepairRelationships()
{
    bool referenced[kCredentialSlotCount] = {};
    int16_t owner[kCredentialSlotCount];
    std::fill_n(owner, kCredentialSlotCount, static_cast<int16_t>(-1));

    // Detect ambiguous ownership before persisting any deterministic repairs.
    for (uint16_t userIndex = 0; userIndex < mLimits.numberOfUsers; ++userIndex) {
        const UserRecord  &user = mUsers[userIndex];
        if (user.status == UserStatusEnum::kAvailable) {
            continue;
        }
        for (uint8_t i = 0; i < user.credentialCount; ++i) {
            uint16_t normalizedIndex = 0;
            const auto  &reference = user.credentials[i];
            VerifyOrReturnError(NormalizeCredentialIndex(reference.credentialIndex, reference.credentialType, normalizedIndex),
                                CHIP_ERROR_INVALID_ARGUMENT);
            size_t storageIndex = CredentialStorageIndex(normalizedIndex, reference.credentialType);
            if (mCredentials[storageIndex].status != DlCredentialStatus::kOccupied) {
                continue;
            }
            if (owner[storageIndex] >= 0 && owner[storageIndex] != static_cast<int16_t>(userIndex)) {
                ESP_LOGE(TAG, "Credential type %u index %u is owned by multiple users",
                         to_underlying(reference.credentialType), reference.credentialIndex);
                return CHIP_ERROR_INVALID_ARGUMENT;
            }
            owner[storageIndex] = static_cast<int16_t>(userIndex);
            referenced[storageIndex] = true;
        }
    }

    char key[PersistentStorageDelegate::kKeyLengthMax + 1];
    for (uint16_t userIndex = 0; userIndex < mLimits.numberOfUsers; ++userIndex) {
        const UserRecord  &storedUser = mUsers[userIndex];
        if (storedUser.status == UserStatusEnum::kAvailable) {
            continue;
        }

        UserRecord repairedUser = storedUser;
        repairedUser.credentialCount = 0;
        std::fill_n(repairedUser.credentials, kMaxCredentialsPerUser, CredentialReferenceRecord());
        for (uint8_t i = 0; i < storedUser.credentialCount; ++i) {
            uint16_t normalizedIndex = 0;
            const auto  &reference = storedUser.credentials[i];
            VerifyOrReturnError(NormalizeCredentialIndex(reference.credentialIndex, reference.credentialType, normalizedIndex),
                                CHIP_ERROR_INVALID_ARGUMENT);
            size_t storageIndex = CredentialStorageIndex(normalizedIndex, reference.credentialType);
            if (mCredentials[storageIndex].status != DlCredentialStatus::kOccupied) {
                ESP_LOGW(TAG, "Repairing user %u: removing missing credential type %u index %u", userIndex + 1,
                         to_underlying(reference.credentialType), reference.credentialIndex);
                continue;
            }
            repairedUser.credentials[repairedUser.credentialCount++] = reference;
        }

        if (repairedUser.credentialCount != storedUser.credentialCount) {
            VerifyOrReturnError(MakeUserKey(userIndex + 1, key, sizeof(key)), CHIP_ERROR_INVALID_ARGUMENT);
            ReturnErrorOnFailure(WriteRecord(key, &repairedUser, sizeof(repairedUser)));
            mUsers[userIndex] = repairedUser;
        }
    }

    for (uint8_t typeValue = 0; typeValue < kCredentialTypeCount; ++typeValue) {
        CredentialTypeEnum type = static_cast<CredentialTypeEnum>(typeValue);
        uint16_t slotCount = mLimits.credentialSlotsByType[typeValue];
        if (slotCount == 0) {
            continue;
        }
        uint16_t firstIndex = type == CredentialTypeEnum::kProgrammingPIN ? 0 : 1;
        uint16_t lastIndex = type == CredentialTypeEnum::kProgrammingPIN ? 0 : slotCount;
        for (uint16_t credentialIndex = firstIndex; credentialIndex <= lastIndex; ++credentialIndex) {
            uint16_t normalizedIndex = 0;
            VerifyOrReturnError(NormalizeCredentialIndex(credentialIndex, type, normalizedIndex), CHIP_ERROR_INVALID_ARGUMENT);
            size_t storageIndex = CredentialStorageIndex(normalizedIndex, type);
            if (mCredentials[storageIndex].status != DlCredentialStatus::kOccupied || referenced[storageIndex]) {
                continue;
            }

            ESP_LOGW(TAG, "Repairing orphaned credential type %u index %u", typeValue, credentialIndex);
            VerifyOrReturnError(MakeCredentialKey(type, credentialIndex, key, sizeof(key)), CHIP_ERROR_INVALID_ARGUMENT);
            ReturnErrorOnFailure(DeleteRecord(key));
            mCredentials[storageIndex] = CredentialRecord();
        }
    }

    return CHIP_NO_ERROR;
}

bool DoorLockStorage::GetUser(uint16_t userIndex, EmberAfPluginDoorLockUserInfo  &user)
{
    VerifyOrReturnValue(mInitialized && userIndex > 0 && userIndex <= mLimits.numberOfUsers, false);
    size_t storageIndex = userIndex - 1;
    const UserRecord  &storedUser = mUsers[storageIndex];

    user.userStatus = storedUser.status;
    if (storedUser.status == UserStatusEnum::kAvailable) {
        return true;
    }

    user.userName = CharSpan(storedUser.userName, storedUser.userNameLength);
    user.credentials = Span<const CredentialStruct>(mUserCredentialViews[storageIndex], storedUser.credentialCount);
    user.userUniqueId = storedUser.uniqueId;
    user.userType = storedUser.type;
    user.credentialRule = storedUser.credentialRule;
    user.creationSource = DlAssetSource::kMatterIM;
    user.createdBy = storedUser.createdBy;
    user.modificationSource = DlAssetSource::kMatterIM;
    user.lastModifiedBy = storedUser.lastModifiedBy;
    return true;
}

bool DoorLockStorage::SetUser(uint16_t userIndex, FabricIndex creator, FabricIndex modifier,
                              const CharSpan  &userName, uint32_t uniqueId, UserStatusEnum userStatus,
                              UserTypeEnum userType, CredentialRuleEnum credentialRule,
                              const CredentialStruct * credentials, size_t totalCredentials)
{
    VerifyOrReturnValue(mInitialized && userIndex > 0 && userIndex <= mLimits.numberOfUsers, false);
    VerifyOrReturnValue(userName.size() <= DOOR_LOCK_MAX_USER_NAME_SIZE, false);
    VerifyOrReturnValue(totalCredentials <= mLimits.numberOfCredentialsPerUser, false);
    VerifyOrReturnValue(totalCredentials == 0 || credentials != nullptr, false);
    VerifyOrReturnValue(IsValidUserStatus(userStatus) && IsValidUserType(userType) &&
                        IsValidCredentialRule(credentialRule),
                        false);

    char key[PersistentStorageDelegate::kKeyLengthMax + 1];
    VerifyOrReturnValue(MakeUserKey(userIndex, key, sizeof(key)), false);
    size_t storageIndex = userIndex - 1;

    if (userStatus == UserStatusEnum::kAvailable) {
        if (DeleteRecord(key) != CHIP_NO_ERROR) {
            return false;
        }
        mUsers[storageIndex] = UserRecord();
        RebuildUserCredentialView(storageIndex);
        return true;
    }

    UserRecord candidate;
    candidate.userNameLength = static_cast<uint16_t>(userName.size());
    if (!userName.empty()) {
        memcpy(candidate.userName, userName.data(), userName.size());
    }
    candidate.uniqueId = uniqueId;
    candidate.status = userStatus;
    candidate.type = userType;
    candidate.credentialRule = credentialRule;
    candidate.createdBy = creator;
    candidate.lastModifiedBy = modifier;
    candidate.credentialCount = static_cast<uint8_t>(totalCredentials);
    for (size_t i = 0; i < totalCredentials; ++i) {
        uint16_t normalizedIndex = 0;
        VerifyOrReturnValue(NormalizeCredentialIndex(credentials[i].credentialIndex, credentials[i].credentialType,
                                                     normalizedIndex),
                            false);
        candidate.credentials[i].credentialIndex = credentials[i].credentialIndex;
        candidate.credentials[i].credentialType = credentials[i].credentialType;
    }
    VerifyOrReturnValue(IsValidUserRecord(candidate), false);

    if (WriteRecord(key, &candidate, sizeof(candidate)) != CHIP_NO_ERROR) {
        return false;
    }
    mUsers[storageIndex] = candidate;
    RebuildUserCredentialView(storageIndex);
    return true;
}

bool DoorLockStorage::GetCredential(uint16_t credentialIndex, CredentialTypeEnum credentialType,
                                    EmberAfPluginDoorLockCredentialInfo  &credential)
{
    uint16_t normalizedIndex = 0;
    VerifyOrReturnValue(mInitialized && NormalizeCredentialIndex(credentialIndex, credentialType, normalizedIndex), false);
    const CredentialRecord  &storedCredential =
        mCredentials[CredentialStorageIndex(normalizedIndex, credentialType)];

    credential.status = storedCredential.status;
    if (storedCredential.status == DlCredentialStatus::kAvailable) {
        return true;
    }

    credential.credentialType = storedCredential.type;
    credential.credentialData = ByteSpan(storedCredential.data, storedCredential.dataLength);
    credential.creationSource = DlAssetSource::kMatterIM;
    credential.createdBy = storedCredential.createdBy;
    credential.modificationSource = DlAssetSource::kMatterIM;
    credential.lastModifiedBy = storedCredential.lastModifiedBy;
    return true;
}

bool DoorLockStorage::SetCredential(uint16_t credentialIndex, FabricIndex creator, FabricIndex modifier,
                                    DlCredentialStatus credentialStatus, CredentialTypeEnum credentialType,
                                    const ByteSpan  &credentialData)
{
    uint16_t normalizedIndex = 0;
    VerifyOrReturnValue(mInitialized && NormalizeCredentialIndex(credentialIndex, credentialType, normalizedIndex), false);
    VerifyOrReturnValue(credentialStatus == DlCredentialStatus::kAvailable ||
                        credentialStatus == DlCredentialStatus::kOccupied,
                        false);
    VerifyOrReturnValue(credentialData.size() <= kMaxCredentialSize, false);

    char key[PersistentStorageDelegate::kKeyLengthMax + 1];
    VerifyOrReturnValue(MakeCredentialKey(credentialType, credentialIndex, key, sizeof(key)), false);
    size_t storageIndex = CredentialStorageIndex(normalizedIndex, credentialType);

    if (credentialStatus == DlCredentialStatus::kAvailable) {
        if (DeleteRecord(key) != CHIP_NO_ERROR) {
            return false;
        }
        mCredentials[storageIndex] = CredentialRecord();
        return true;
    }

    CredentialRecord candidate;
    candidate.status = credentialStatus;
    candidate.type = credentialType;
    candidate.createdBy = creator;
    candidate.lastModifiedBy = modifier;
    candidate.dataLength = static_cast<uint8_t>(credentialData.size());
    if (!credentialData.empty()) {
        memcpy(candidate.data, credentialData.data(), credentialData.size());
    }
    VerifyOrReturnValue(IsValidCredentialRecord(candidate, credentialType), false);

    if (WriteRecord(key, &candidate, sizeof(candidate)) != CHIP_NO_ERROR) {
        memset(candidate.data, 0, sizeof(candidate.data));
        return false;
    }
    mCredentials[storageIndex] = candidate;
    memset(candidate.data, 0, sizeof(candidate.data));
    return true;
}

DlStatus DoorLockStorage::GetWeekdaySchedule(uint8_t scheduleIndex, uint16_t userIndex,
                                             EmberAfPluginDoorLockWeekDaySchedule  &schedule) const
{
    VerifyOrReturnValue(mInitialized && scheduleIndex > 0 && userIndex > 0 &&
                        scheduleIndex <= mLimits.numberOfWeekdaySchedulesPerUser &&
                        userIndex <= mLimits.numberOfUsers,
                        DlStatus::kFailure);
    const auto  &record = mWeekdaySchedules[userIndex - 1][scheduleIndex - 1];
    VerifyOrReturnValue(record.status != DlScheduleStatus::kAvailable, DlStatus::kNotFound);
    schedule = record.schedule;
    return DlStatus::kSuccess;
}

DlStatus DoorLockStorage::SetWeekdaySchedule(uint8_t scheduleIndex, uint16_t userIndex, DlScheduleStatus status,
                                             DaysMaskMap daysMask, uint8_t startHour, uint8_t startMinute,
                                             uint8_t endHour, uint8_t endMinute)
{
    VerifyOrReturnValue(mInitialized && scheduleIndex > 0 && userIndex > 0 &&
                        scheduleIndex <= mLimits.numberOfWeekdaySchedulesPerUser &&
                        userIndex <= mLimits.numberOfUsers,
                        DlStatus::kFailure);
    char key[PersistentStorageDelegate::kKeyLengthMax + 1];
    VerifyOrReturnValue(MakeScheduleKey("ws", scheduleIndex, userIndex, key, sizeof(key)), DlStatus::kFailure);
    auto  &record = mWeekdaySchedules[userIndex - 1][scheduleIndex - 1];
    if (status == DlScheduleStatus::kAvailable) {
        VerifyOrReturnValue(DeleteRecord(key) == CHIP_NO_ERROR, DlStatus::kFailure);
        record = {};
        return DlStatus::kSuccess;
    }
    record.status = status;
    record.schedule = { daysMask, startHour, startMinute, endHour, endMinute };
    VerifyOrReturnValue(WriteRecord(key, &record, sizeof(record)) == CHIP_NO_ERROR, DlStatus::kFailure);
    return DlStatus::kSuccess;
}

DlStatus DoorLockStorage::GetYeardaySchedule(uint8_t scheduleIndex, uint16_t userIndex,
                                             EmberAfPluginDoorLockYearDaySchedule  &schedule) const
{
    VerifyOrReturnValue(mInitialized && scheduleIndex > 0 && userIndex > 0 &&
                        scheduleIndex <= mLimits.numberOfYeardaySchedulesPerUser &&
                        userIndex <= mLimits.numberOfUsers,
                        DlStatus::kFailure);
    const auto  &record = mYeardaySchedules[userIndex - 1][scheduleIndex - 1];
    VerifyOrReturnValue(record.status != DlScheduleStatus::kAvailable, DlStatus::kNotFound);
    schedule = record.schedule;
    return DlStatus::kSuccess;
}

DlStatus DoorLockStorage::SetYeardaySchedule(uint8_t scheduleIndex, uint16_t userIndex, DlScheduleStatus status,
                                             uint32_t localStartTime, uint32_t localEndTime)
{
    VerifyOrReturnValue(mInitialized && scheduleIndex > 0 && userIndex > 0 &&
                        scheduleIndex <= mLimits.numberOfYeardaySchedulesPerUser &&
                        userIndex <= mLimits.numberOfUsers,
                        DlStatus::kFailure);
    char key[PersistentStorageDelegate::kKeyLengthMax + 1];
    VerifyOrReturnValue(MakeScheduleKey("ys", scheduleIndex, userIndex, key, sizeof(key)), DlStatus::kFailure);
    auto  &record = mYeardaySchedules[userIndex - 1][scheduleIndex - 1];
    if (status == DlScheduleStatus::kAvailable) {
        VerifyOrReturnValue(DeleteRecord(key) == CHIP_NO_ERROR, DlStatus::kFailure);
        record = {};
        return DlStatus::kSuccess;
    }
    record.status = status;
    record.schedule = { localStartTime, localEndTime };
    VerifyOrReturnValue(WriteRecord(key, &record, sizeof(record)) == CHIP_NO_ERROR, DlStatus::kFailure);
    return DlStatus::kSuccess;
}

DlStatus DoorLockStorage::GetHolidaySchedule(uint8_t scheduleIndex,
                                             EmberAfPluginDoorLockHolidaySchedule  &schedule) const
{
    VerifyOrReturnValue(mInitialized && scheduleIndex > 0 && scheduleIndex <= mLimits.numberOfHolidaySchedules,
                        DlStatus::kFailure);
    const auto  &record = mHolidaySchedules[scheduleIndex - 1];
    VerifyOrReturnValue(record.status != DlScheduleStatus::kAvailable, DlStatus::kNotFound);
    schedule = record.schedule;
    return DlStatus::kSuccess;
}

DlStatus DoorLockStorage::SetHolidaySchedule(uint8_t scheduleIndex, DlScheduleStatus status,
                                             uint32_t localStartTime, uint32_t localEndTime,
                                             OperatingModeEnum operatingMode)
{
    VerifyOrReturnValue(mInitialized && scheduleIndex > 0 && scheduleIndex <= mLimits.numberOfHolidaySchedules,
                        DlStatus::kFailure);
    char key[PersistentStorageDelegate::kKeyLengthMax + 1];
    VerifyOrReturnValue(MakeScheduleKey("hs", scheduleIndex, 0, key, sizeof(key)), DlStatus::kFailure);
    auto  &record = mHolidaySchedules[scheduleIndex - 1];
    if (status == DlScheduleStatus::kAvailable) {
        VerifyOrReturnValue(DeleteRecord(key) == CHIP_NO_ERROR, DlStatus::kFailure);
        record = {};
        return DlStatus::kSuccess;
    }
    record.status = status;
    record.schedule = { localStartTime, localEndTime, operatingMode };
    VerifyOrReturnValue(WriteRecord(key, &record, sizeof(record)) == CHIP_NO_ERROR, DlStatus::kFailure);
    return DlStatus::kSuccess;
}

bool DoorLockStorage::ValidatePIN(const ByteSpan  &pin, PinMatch  &match) const
{
    VerifyOrReturnValue(mInitialized, false);
    match = PinMatch();
    const uint16_t pinSlotCount =
        mLimits.credentialSlotsByType[to_underlying(CredentialTypeEnum::kPin)];
    for (uint16_t normalizedIndex = 0; normalizedIndex < pinSlotCount; ++normalizedIndex) {
        const CredentialRecord  &credential =
            mCredentials[CredentialStorageIndex(normalizedIndex, CredentialTypeEnum::kPin)];
        if (credential.status != DlCredentialStatus::kOccupied ||
                !ByteSpan(credential.data, credential.dataLength).data_equal(pin)) {
            continue;
        }

        const uint16_t credentialIndex = normalizedIndex + 1;
        for (uint16_t userIndex = 0; userIndex < mLimits.numberOfUsers; ++userIndex) {
            const UserRecord  &user = mUsers[userIndex];
            if (user.status != UserStatusEnum::kOccupiedEnabled) {
                continue;
            }
            for (uint8_t i = 0; i < user.credentialCount; ++i) {
                if (user.credentials[i].credentialType == CredentialTypeEnum::kPin &&
                        user.credentials[i].credentialIndex == credentialIndex) {
                    match.userIndex = userIndex + 1;
                    match.credentialIndex = credentialIndex;
                    return true;
                }
            }
        }
        return false;
    }
    return false;
}

CHIP_ERROR DoorLockStorage::ReadRecord(const char * key, void * record, uint16_t recordSize, bool  &found) const
{
    uint16_t actualSize = recordSize;
    CHIP_ERROR err = mStorage->SyncGetKeyValue(key, record, actualSize);
    if (err == CHIP_ERROR_PERSISTED_STORAGE_VALUE_NOT_FOUND) {
        found = false;
        return CHIP_NO_ERROR;
    }
    ReturnErrorOnFailure(err);
    VerifyOrReturnError(actualSize == recordSize, CHIP_ERROR_INVALID_ARGUMENT);
    found = true;
    return CHIP_NO_ERROR;
}

CHIP_ERROR DoorLockStorage::WriteRecord(const char * key, const void * record, uint16_t recordSize) const
{
    CHIP_ERROR err = mStorage->SyncSetKeyValue(key, record, recordSize);
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to persist door lock record '%s': %" CHIP_ERROR_FORMAT, key, err.Format());
    }
    return err;
}

CHIP_ERROR DoorLockStorage::DeleteRecord(const char * key) const
{
    CHIP_ERROR err = mStorage->SyncDeleteKeyValue(key);
    if (err == CHIP_ERROR_PERSISTED_STORAGE_VALUE_NOT_FOUND) {
        return CHIP_NO_ERROR;
    }
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to delete door lock record '%s': %" CHIP_ERROR_FORMAT, key, err.Format());
    }
    return err;
}

bool DoorLockStorage::MakeUserKey(uint16_t userIndex, char * key, size_t keySize) const
{
    return FormatKey(key, keySize, "dl/u/%u", userIndex);
}

bool DoorLockStorage::MakeCredentialKey(CredentialTypeEnum type, uint16_t credentialIndex,
                                        char * key, size_t keySize) const
{
    return FormatKey(key, keySize, "dl/c/%u/%u", to_underlying(type), credentialIndex);
}

bool DoorLockStorage::MakeScheduleKey(const char * type, uint8_t scheduleIndex, uint16_t userIndex,
                                      char * key, size_t keySize) const
{
    int length = snprintf(key, keySize, "dl/%s/%u/%u", type, scheduleIndex, userIndex);
    return length > 0 && static_cast<size_t>(length) < keySize &&
           static_cast<size_t>(length) <= PersistentStorageDelegate::kKeyLengthMax;
}

bool DoorLockStorage::IsValidUserRecord(const UserRecord  &user) const
{
    if (!IsValidUserStatus(user.status) || user.status == UserStatusEnum::kAvailable ||
            !IsValidUserType(user.type) || !IsValidCredentialRule(user.credentialRule) ||
            user.userNameLength > DOOR_LOCK_MAX_USER_NAME_SIZE ||
            user.credentialCount > mLimits.numberOfCredentialsPerUser) {
        return false;
    }

    for (uint8_t i = 0; i < user.credentialCount; ++i) {
        uint16_t normalizedIndex = 0;
        if (!NormalizeCredentialIndex(user.credentials[i].credentialIndex, user.credentials[i].credentialType,
                                      normalizedIndex)) {
            return false;
        }
        for (uint8_t j = 0; j < i; ++j) {
            if (user.credentials[i].credentialIndex == user.credentials[j].credentialIndex &&
                    user.credentials[i].credentialType == user.credentials[j].credentialType) {
                return false;
            }
        }
    }
    return true;
}

bool DoorLockStorage::IsValidCredentialRecord(const CredentialRecord  &credential,
                                              CredentialTypeEnum expectedType) const
{
    return credential.status == DlCredentialStatus::kOccupied && credential.type == expectedType &&
           IsValidCredentialType(credential.type) && credential.dataLength <= kMaxCredentialSize;
}

bool DoorLockStorage::NormalizeCredentialIndex(uint16_t credentialIndex, CredentialTypeEnum type,
                                               uint16_t  &normalizedIndex) const
{
    if (!IsValidCredentialType(type)) {
        return false;
    }
    const uint16_t slotCount = mLimits.credentialSlotsByType[to_underlying(type)];
    if (type == CredentialTypeEnum::kProgrammingPIN) {
        normalizedIndex = 0;
        return slotCount == 1 && credentialIndex == 0;
    }
    if (credentialIndex == 0 || credentialIndex > slotCount) {
        return false;
    }
    normalizedIndex = credentialIndex - 1;
    return true;
}

size_t DoorLockStorage::CredentialStorageIndex(uint16_t normalizedIndex, CredentialTypeEnum type) const
{
    return static_cast<size_t>(to_underlying(type)) * kMaxCredentialSlotsPerType + normalizedIndex;
}

void DoorLockStorage::RebuildUserCredentialView(size_t userStorageIndex)
{
    std::fill_n(mUserCredentialViews[userStorageIndex], kMaxCredentialsPerUser, CredentialStruct());
    const UserRecord  &user = mUsers[userStorageIndex];
    for (uint8_t i = 0; i < user.credentialCount; ++i) {
        mUserCredentialViews[userStorageIndex][i].credentialIndex = user.credentials[i].credentialIndex;
        mUserCredentialViews[userStorageIndex][i].credentialType = user.credentials[i].credentialType;
    }
}
