/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#pragma once

#include "door_lock_capabilities.h"
#include <initializer_list>
#include <stdint.h>
#include <app/clusters/door-lock-server/door-lock-server.h>
#include <lib/core/CHIPPersistentStorageDelegate.h>
#include <lib/support/Span.h>

using chip::app::Clusters::DoorLock::CredentialRuleEnum;
using chip::app::Clusters::DoorLock::CredentialTypeEnum;
using chip::app::Clusters::DoorLock::UserStatusEnum;
using chip::app::Clusters::DoorLock::UserTypeEnum;

/** CHIP-thread-owned, per-record storage. Credential records are the sole ownership authority. */
class DoorLockStorage {
public:
    static constexpr uint16_t kMaxUsers = DoorLockCapabilities::kUsers;
    static constexpr uint8_t kMaxCredentialsPerUser = DoorLockCapabilities::kCredentialsPerUser;
    static constexpr uint16_t kMaxCredentialSlots = DoorLockCapabilities::kCredentialSlotsPerType;
    static constexpr uint8_t kCredentialTypeCount = static_cast<uint8_t>(CredentialTypeEnum::kAliroNonEvictableEndpointKey) + 1;
    static constexpr uint8_t kMaxCredentialSize = 65;
    static constexpr uint8_t kMaxUserNameSize = DOOR_LOCK_MAX_USER_NAME_SIZE;

    struct Limits {
        uint16_t numberOfUsers = 0;
        uint8_t numberOfCredentialsPerUser = 0;
        uint16_t credentialSlotsByType[kCredentialTypeCount] = {};
    };

    struct UserRecord {
        char name[kMaxUserNameSize] = {};
        uint8_t nameLength = 0;
        uint32_t uniqueId = 0;
        UserStatusEnum status = UserStatusEnum::kAvailable;
        UserTypeEnum type = UserTypeEnum::kUnrestrictedUser;
        CredentialRuleEnum rule = CredentialRuleEnum::kSingle;
        chip::FabricIndex createdBy = chip::kUndefinedFabricIndex;
        chip::FabricIndex modifiedBy = chip::kUndefinedFabricIndex;
    };

    struct CredentialRecord {
        DlCredentialStatus status = DlCredentialStatus::kAvailable;
        chip::FabricIndex createdBy = chip::kUndefinedFabricIndex;
        chip::FabricIndex modifiedBy = chip::kUndefinedFabricIndex;
        uint8_t size = 0;
        uint8_t data[kMaxCredentialSize] = {};
        uint16_t ownerUserIndex = 0; // Only a programming PIN may be ownerless.
        uint64_t iteration = 0;     // Unsigned issuer high-water mark; zero is the initial floor.
    };

    struct Match {
        uint16_t userIndex = 0;
        uint16_t credentialIndex = 0;
        CredentialTypeEnum type = CredentialTypeEnum::kPin;
    };

    static DoorLockStorage &Instance()
    {
        static DoorLockStorage instance;
        return instance;
    }
    /** Load the configured record ranges; incompatible versions abort without erasing storage. */
    CHIP_ERROR Init(chip::EndpointId endpoint, const Limits &limits, chip::PersistentStorageDelegate &storage);
    bool Ready() const
    {
        return mReady;
    }

    chip::EndpointId Endpoint() const
    {
        return mEndpoint;
    }

    uint64_t Generation() const
    {
        return mGeneration;
    }

    /** Clear fast keys and cancel in-flight proofs before durable policy changes. */
    bool Invalidate(const char *reason);
    void Unavailable(const char *reason);
    /** Boot-time consistency scan; working memory does not scale with the number of users or slots. */
    bool ValidateDatabase();
    /** Discard the SDK's uncommitted SetUser -> SetCredential proposal after command dispatch. */
    void DiscardPendingUser()
    {
        mPendingUserIndex = 0;
    }

    bool ReadUser(uint16_t index, UserRecord &out);
    bool ReadCredential(CredentialTypeEnum type, uint16_t index, CredentialRecord &out);
    /** SDK views rotate twice; a third call reuses backing. Do not retain spans asynchronously. */
    bool GetUser(uint16_t index, EmberAfPluginDoorLockUserInfo &out);
    bool SetUser(uint16_t index, chip::FabricIndex creator, chip::FabricIndex modifier, const chip::CharSpan &name,
                 uint32_t uniqueId, UserStatusEnum status, UserTypeEnum type, CredentialRuleEnum rule,
                 const CredentialStruct *credentials, size_t count);
    /** The next GetCredential call replaces this view's backing. */
    bool GetCredential(uint16_t index, CredentialTypeEnum type, EmberAfPluginDoorLockCredentialInfo &out);
    bool SetCredential(uint16_t index, chip::FabricIndex creator, chip::FabricIndex modifier, DlCredentialStatus status,
                       CredentialTypeEnum type, const chip::ByteSpan &data);
    // Lookup proves a unique stored key match; Authorize checks its current owner and policy.
    bool ValidatePIN(const chip::ByteSpan &pin, Match &out);
    /** A miss sets found=false; errors and ambiguous identities return false. */
    bool FindCredentialByData(const chip::ByteSpan &data, std::initializer_list<CredentialTypeEnum> types,
                              Match &out, bool &found);
    bool FindEndpoint(const uint8_t key[65], Match &out, bool &found);
    bool LearnIteration(uint16_t issuerIndex, uint64_t iteration);
    bool Enroll(uint16_t owner, const uint8_t key[65], Match &out);

private:
    DoorLockStorage() = default;
    bool CheckLimits() const;
    bool CheckUser(const UserRecord &user) const;
    bool CheckCredential(CredentialTypeEnum type, const CredentialRecord &credential) const;
    bool IndexValid(CredentialTypeEnum type, uint16_t index) const;
    bool Load();
    bool Read(const char *key, void *record, size_t size);
    bool SaveRecord(const char *key, const void *record, size_t size);
    struct EndpointSlots {
        size_t occupied = 0;
        uint16_t freeEvictable = 0;
    };
    bool ReadEndpointSlots(EndpointSlots &out);
    bool WriteUser(uint16_t index, const UserRecord &user);
    bool UserCredentials(uint16_t owner, CredentialStruct(&out)[kMaxCredentialsPerUser], uint8_t &count);

    chip::EndpointId mEndpoint = 0;
    chip::PersistentStorageDelegate *mStorage = nullptr;
    Limits mLimits;
    uint64_t mGeneration = 0;
    bool mReady = false;
    uint16_t mPendingUserIndex = 0;
    UserRecord mPendingUser;
    CredentialStruct mPendingCredential;
    struct UserView {
        UserRecord user;
        CredentialStruct credentials[kMaxCredentialsPerUser];
        uint8_t count = 0;
    } mUserViews[2];
    uint8_t mNextUserView = 0;
    CredentialRecord mCredentialView;
};
