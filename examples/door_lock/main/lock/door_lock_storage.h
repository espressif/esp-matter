/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include "door_lock_capabilities.h"

#include <app/clusters/door-lock-server/door-lock-server.h>
#include <lib/core/CHIPPersistentStorageDelegate.h>

#include <stddef.h>
#include <stdint.h>

class DoorLockStorage final {
public:
    static constexpr uint8_t kCredentialTypeCount = 9;

    struct Limits {
        uint16_t numberOfUsers = 0;
        uint8_t numberOfCredentialsPerUser = 0;
        uint16_t credentialSlotsByType[kCredentialTypeCount] = {};
    };

    struct Config {
        chip::EndpointId endpoint;
        Limits limits;
        chip::PersistentStorageDelegate  &defaultStorage;
    };

    static constexpr uint16_t kMaxUsers = DoorLockCapabilities::kUsers;
    static constexpr uint8_t kMaxCredentialsPerUser = DoorLockCapabilities::kCredentialsPerUser;
    static constexpr uint16_t kMaxCredentialSlotsPerType = DoorLockCapabilities::kCredentialSlotsPerType;
    static constexpr uint8_t kMaxCredentialSize = 65;
    static constexpr size_t kCredentialSlotCount = kCredentialTypeCount * kMaxCredentialSlotsPerType;

    static DoorLockStorage  &Instance()
    {
        static DoorLockStorage storage;
        return storage;
    }

    CHIP_ERROR Init(const Config  &config);

    bool GetUser(uint16_t userIndex, EmberAfPluginDoorLockUserInfo  &user);
    bool SetUser(uint16_t userIndex, chip::FabricIndex creator, chip::FabricIndex modifier,
                 const chip::CharSpan  &userName, uint32_t uniqueId, UserStatusEnum userStatus,
                 UserTypeEnum userType, CredentialRuleEnum credentialRule,
                 const CredentialStruct * credentials, size_t totalCredentials);

    bool GetCredential(uint16_t credentialIndex, CredentialTypeEnum credentialType,
                       EmberAfPluginDoorLockCredentialInfo  &credential);
    bool SetCredential(uint16_t credentialIndex, chip::FabricIndex creator,
                       chip::FabricIndex modifier, DlCredentialStatus credentialStatus,
                       CredentialTypeEnum credentialType, const chip::ByteSpan  &credentialData);

    struct PinMatch {
        uint16_t userIndex = 0;
        uint16_t credentialIndex = 0;
    };

    bool ValidatePIN(const chip::ByteSpan  &pin, PinMatch  &match) const;

private:
    DoorLockStorage() = default;

    static constexpr uint32_t kStorageMagic = 0x444C4442; // "DLDB"
    static constexpr uint16_t kStorageVersion = 1;

    struct StorageMetadata {
        uint32_t magic = kStorageMagic;
        uint16_t version = kStorageVersion;
        uint16_t numberOfUsers = 0;
        uint8_t numberOfCredentialsPerUser = 0;
    };

    struct CredentialReferenceRecord {
        uint16_t credentialIndex = 0;
        CredentialTypeEnum credentialType = CredentialTypeEnum::kProgrammingPIN;
    };

    struct UserRecord {
        char userName[DOOR_LOCK_MAX_USER_NAME_SIZE] = {};
        uint16_t userNameLength = 0;
        uint32_t uniqueId = 0;
        UserStatusEnum status = UserStatusEnum::kAvailable;
        UserTypeEnum type = UserTypeEnum::kUnrestrictedUser;
        CredentialRuleEnum credentialRule = CredentialRuleEnum::kSingle;
        chip::FabricIndex createdBy = chip::kUndefinedFabricIndex;
        chip::FabricIndex lastModifiedBy = chip::kUndefinedFabricIndex;
        uint8_t credentialCount = 0;
        CredentialReferenceRecord credentials[kMaxCredentialsPerUser] = {};
    };

    struct CredentialRecord {
        DlCredentialStatus status = DlCredentialStatus::kAvailable;
        CredentialTypeEnum type = CredentialTypeEnum::kProgrammingPIN;
        chip::FabricIndex createdBy = chip::kUndefinedFabricIndex;
        chip::FabricIndex lastModifiedBy = chip::kUndefinedFabricIndex;
        uint8_t dataLength = 0;
        uint8_t data[kMaxCredentialSize] = {};
    };

    CHIP_ERROR LoadMetadata();
    CHIP_ERROR LoadRecords();
    CHIP_ERROR RepairRelationships();

    CHIP_ERROR ReadRecord(const char * key, void * record, uint16_t recordSize, bool  &found) const;
    CHIP_ERROR WriteRecord(const char * key, const void * record, uint16_t recordSize) const;
    CHIP_ERROR DeleteRecord(const char * key) const;

    bool MakeUserKey(uint16_t userIndex, char * key, size_t keySize) const;
    bool MakeCredentialKey(CredentialTypeEnum type, uint16_t credentialIndex,
                           char * key, size_t keySize) const;

    bool IsValidUserRecord(const UserRecord  &user) const;
    bool IsValidCredentialRecord(const CredentialRecord  &credential,
                                 CredentialTypeEnum expectedType) const;

    bool NormalizeCredentialIndex(uint16_t credentialIndex, CredentialTypeEnum type,
                                  uint16_t  &normalizedIndex) const;
    size_t CredentialStorageIndex(uint16_t normalizedIndex, CredentialTypeEnum type) const;
    void RebuildUserCredentialView(size_t userStorageIndex);

    chip::PersistentStorageDelegate * mStorage = nullptr;
    Limits mLimits;
    UserRecord mUsers[kMaxUsers] = {};
    CredentialRecord mCredentials[kCredentialSlotCount] = {};
    CredentialStruct mUserCredentialViews[kMaxUsers][kMaxCredentialsPerUser] = {};
    bool mInitialized = false;
};
