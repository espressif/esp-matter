/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "door_lock_authorization.h"

#include <esp_log.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app/EventLogging.h>

using namespace chip::app::Clusters::DoorLock;
using chip::Protocols::InteractionModel::Status;

void DoorLockAuthorization::ReportUnlockFailure(const DoorLockStorage::Match &match, OperationSourceEnum source,
                                                OperationErrorEnum error)
{
    chip::app::DataModel::Nullable<uint16_t> user;
    chip::app::DataModel::Nullable<chip::app::DataModel::List<const CredentialStruct>> credentials;
    CredentialStruct credential{match.type, match.credentialIndex};
    if (match.userIndex) {
        user.SetNonNull(match.userIndex);
        credentials.SetNonNull(chip::app::DataModel::List<const CredentialStruct>(&credential, 1));
    }
    Events::LockOperationError::Type event{LockOperationTypeEnum::kUnlock, source, error, user, {}, {},
                                           chip::MakeOptional(credentials)};
    chip::EventNumber number;
    CHIP_ERROR err = chip::app::LogEvent(event, DoorLockStorage::Instance().Endpoint(), number);
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE("lock_auth", "Operation error event failed: %s", err.AsString());
    }
}

bool DoorLockAuthorization::ModeAllows(chip::EndpointId endpoint, OperationSourceEnum source, OperationErrorEnum &error,
                                       bool programmingUser)
{
    error = OperationErrorEnum::kRestricted;
    OperatingModeEnum mode;
    bool enabled = false;
    if (Attributes::ActuatorEnabled::Get(endpoint, &enabled) != Status::Success || !enabled ||
            Attributes::OperatingMode::Get(endpoint, &mode) != Status::Success) {
        return false;
    }
    bool remote = source == OperationSourceEnum::kRemote;
    switch (mode) {
    case OperatingModeEnum::kNormal:
        return true;
    case OperatingModeEnum::kVacation:
        return remote || (source == OperationSourceEnum::kKeypad && programmingUser);
    case OperatingModeEnum::kPrivacy:
        return false;
    case OperatingModeEnum::kNoRemoteLockUnlock:
        return !remote;
    default:
        return false; // Passage and unknown modes are not advertised.
    }
}

bool DoorLockAuthorization::Authorize(const DoorLockStorage::Match &match, OperationSourceEnum source,
                                      OperationErrorEnum &error)
{
    DoorLockStorage &store = DoorLockStorage::Instance();
    error = OperationErrorEnum::kInvalidCredential;
    DoorLockStorage::CredentialRecord credential;
    DoorLockStorage::UserRecord user;
    if (!store.ReadCredential(match.type, match.credentialIndex, credential) ||
            credential.status != DlCredentialStatus::kOccupied || credential.ownerUserIndex != match.userIndex ||
            !store.ReadUser(match.userIndex, user)) {
        return false;
    }

    if (user.status != UserStatusEnum::kOccupiedEnabled) {
        error = OperationErrorEnum::kDisabledUserDenied;
        return false;
    }

    error = OperationErrorEnum::kRestricted;
    if (user.rule != CredentialRuleEnum::kSingle ||
            !ModeAllows(store.Endpoint(), source, error, user.type == UserTypeEnum::kProgrammingUser)) {
        return false;
    }

    switch (user.type) {
    case UserTypeEnum::kUnrestrictedUser:
    case UserTypeEnum::kProgrammingUser:
        return true;
    case UserTypeEnum::kRemoteOnlyUser:
        return source == OperationSourceEnum::kRemote;
    default:
        // Schedules and other unsupported user workflows never become unrestricted.
        return false;
    }
}
