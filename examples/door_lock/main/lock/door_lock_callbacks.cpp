/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "bolt_lock_manager.h"
#include "door_lock_storage.h"
#include "door_lock_authorization.h"
#include "app_priv.h"
#ifdef CONFIG_ENABLE_ALIRO_OVER_NFC
#include "aliro_delegate.h"
#endif

#include <app-common/zap-generated/attributes/Accessors.h>
#include <app/server/Server.h>
#include <esp_log.h>
#include <app/util/MatterCallbacks.h>
#include <lib/core/DataModelTypes.h>
#include <lib/support/CodeUtils.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::DoorLock;
using chip::Protocols::InteractionModel::Status;

namespace {
const char * TAG = "door_lock_callbacks";
EndpointId sDoorLockEndpoint = kInvalidEndpointId;

// Keep an SDK proposal only during its synchronous command; never cache a whole database.
class DoorLockCallbacks final : public FabricTable::Delegate, public DataModelCallbacks {
    static bool IsCredentialMutation(const ConcreteCommandPath &path)
    {
        if (path.mClusterId != DoorLock::Id) {
            return false;
        }
        switch (path.mCommandId) {
        case DoorLock::Commands::SetUser::Id:
        case DoorLock::Commands::ClearUser::Id:
        case DoorLock::Commands::SetCredential::Id:
        case DoorLock::Commands::ClearCredential::Id:
            return true;
        default:
            return false;
        }
    }
    CHIP_ERROR PreCommandReceived(const ConcreteCommandPath &path, const Access::SubjectDescriptor &) override
    {
        return !IsCredentialMutation(path) || DoorLockStorage::Instance().Ready()
               ? CHIP_NO_ERROR : CHIP_ERROR_INCORRECT_STATE;
    }
    void PostCommandReceived(const ConcreteCommandPath &path, const Access::SubjectDescriptor &) override
    {
        if (IsCredentialMutation(path)) {
            DoorLockStorage::Instance().DiscardPendingUser();
        }
    }
    void FabricWillBeRemoved(const FabricTable &, FabricIndex) override
    {
        // CHIP clears creator/modifier attribution through the ordinary storage
        // callbacks. Fabric removal does not delete Matter lock credentials.
        if (!DoorLockStorage::Instance().Invalidate("fabric-removal")) {
            ESP_LOGE(TAG, "Fabric invalidation failed closed");
        }
    }
};

DoorLockCallbacks sDoorLockCallbacks;

bool ValidateRemotePIN(EndpointId endpoint, const Optional<ByteSpan>  &pin,
                       DoorLockStorage::Match  &match, bool  &usedPin,
                       OperationErrorEnum  &error)
{
    usedPin = false;
    bool requirePin = false;
    if (DoorLock::Attributes::RequirePINforRemoteOperation::Get(endpoint, &requirePin) != Status::Success) {
        error = OperationErrorEnum::kRestricted;
        return false;
    }

    if (!pin.HasValue()) {
        if (requirePin) {
            error = OperationErrorEnum::kInvalidCredential;
            return false;
        }
        return DoorLockAuthorization::ModeAllows(endpoint, OperationSourceEnum::kRemote, error);
    }
    error = OperationErrorEnum::kInvalidCredential;
    if (DoorLockStorage::Instance().ValidatePIN(pin.Value(), match) &&
            DoorLockAuthorization::Authorize(match, OperationSourceEnum::kRemote, error)) {
        usedPin = true;
        return true;
    }
    return false;
}

bool ReadCredentialSlotLimits(DoorLockServer  &server, EndpointId endpoint,
                              DoorLockStorage::Limits  &limits)
{
    limits.credentialSlotsByType[to_underlying(CredentialTypeEnum::kProgrammingPIN)] = 1;
    if (!server.GetNumberOfPINCredentialsSupported(
                endpoint, limits.credentialSlotsByType[to_underlying(CredentialTypeEnum::kPin)])) {
        return false;
    }

#ifdef CONFIG_ENABLE_ALIRO_OVER_NFC
    AliroDoorLockDelegate  &delegate = AliroDoorLockDelegate::Instance();
    limits.credentialSlotsByType[to_underlying(CredentialTypeEnum::kAliroCredentialIssuerKey)] =
              delegate.GetNumberOfAliroCredentialIssuerKeysSupported();
    uint16_t endpointKeySlots = delegate.GetNumberOfAliroEndpointKeysSupported();
    limits.credentialSlotsByType[to_underlying(CredentialTypeEnum::kAliroEvictableEndpointKey)] = endpointKeySlots;
    limits.credentialSlotsByType[to_underlying(CredentialTypeEnum::kAliroNonEvictableEndpointKey)] = endpointKeySlots;
#endif

    return true;
}

void HandleBoltStateChange(BoltLockManager::State state, BoltLockManager::OperationSource source, bool success)
{
    if (success && state == BoltLockManager::State::kUnlockingInitiated &&
            (source == OperationSourceEnum::kManual || source == OperationSourceEnum::kButton)) {
        OperatingModeEnum mode;
        if (DoorLock::Attributes::OperatingMode::Get(sDoorLockEndpoint, &mode) == Status::Success &&
                mode == OperatingModeEnum::kPrivacy &&
                DoorLock::Attributes::OperatingMode::Set(sDoorLockEndpoint, OperatingModeEnum::kNormal) !=
                Status::Success) {
            ESP_LOGE(TAG, "Manual unlock could not restore Normal operating mode");
        }
    }
    DlLockState lockState = DlLockState::kNotFullyLocked;
    switch (state) {
    case BoltLockManager::State::kLockingInitiated:
        break;
    case BoltLockManager::State::kUnlockingInitiated:
        break;
    case BoltLockManager::State::kLockingCompleted:
        lockState = DlLockState::kLocked;
        break;
    case BoltLockManager::State::kUnlockingCompleted:
        lockState = DlLockState::kUnlocked;
        break;
    }
    const BoltLockManager::Attribution &attribution = BoltLockManager::Instance().GetAttribution();
    Nullable<uint16_t> user;
    Nullable<DataModel::List<const CredentialStruct>> credentials;
    if (attribution.user) {
        user.SetNonNull(attribution.user);
        credentials.SetNonNull(DataModel::List<const CredentialStruct>(&attribution.credential, 1));
    }
    bool published = success
                     ? DoorLockServer::Instance().SetLockState(sDoorLockEndpoint, lockState, source, user,
                                                               credentials, attribution.fabric, attribution.node)
                     : DoorLockServer::Instance().SetLockState(sDoorLockEndpoint, lockState);
    if (!published) {
        ESP_LOGE(TAG, "Failed to publish bolt state %u", to_underlying(lockState));
    }
    esp_err_t err = app_driver_set_lock_state(state);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to update RGB LED for bolt state %u: %s", to_underlying(lockState),
                 esp_err_to_name(err));
    }
}

} // namespace

// Forced into the link so this translation unit supplies the Ember callback overrides.
extern "C" void retain_ember_door_lock_callbacks() {}

void emberAfDoorLockClusterInitCallback(EndpointId endpoint)
{
    sDoorLockEndpoint = endpoint;

    DoorLockServer  &server = DoorLockServer::Instance();
    server.InitServer(endpoint);

    DoorLockStorage::Limits limits;
    VerifyOrDie(server.GetNumberOfUserSupported(endpoint, limits.numberOfUsers));
    VerifyOrDie(server.GetNumberOfCredentialsSupportedPerUser(endpoint, limits.numberOfCredentialsPerUser));
    VerifyOrDie(ReadCredentialSlotLimits(server, endpoint, limits));
    // Matter's KVS is erased by the platform factory reset, including lock
    // records, issuer high-water state and metadata.
    CHIP_ERROR err = DoorLockStorage::Instance().Init(endpoint, limits, Server::GetInstance().GetPersistentStorage());
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Door lock storage initialization failed: %" CHIP_ERROR_FORMAT, err.Format());
    }
    // Keep manual operation and the standard factory-reset command available
    // when storage cannot be authorized.
    err = BoltLockManager::Instance().Init(BoltLockManager::State::kLockingCompleted, HandleBoltStateChange);
    VerifyOrDie(err == CHIP_NO_ERROR);
    Server::GetInstance().GetFabricTable().AddFabricDelegate(&sDoorLockCallbacks);
    DataModelCallbacks::SetInstance(&sDoorLockCallbacks);
    VerifyOrDie(server.SetLockState(endpoint, DlLockState::kLocked, OperationSourceEnum::kUnspecified));
}

bool emberAfPluginDoorLockOnDoorLockCommand(EndpointId endpoint, const Nullable<FabricIndex>  &fabricIndex,
                                            const Nullable<NodeId>  &nodeId, const Optional<ByteSpan>  &pin,
                                            OperationErrorEnum  &error)
{
    DoorLockStorage::Match match;
    bool usedPin = false;
    VerifyOrReturnValue(ValidateRemotePIN(endpoint, pin, match, usedPin, error), false);
    BoltLockManager::Attribution attribution;
    attribution.fabric = fabricIndex;
    attribution.node = nodeId;
    if (usedPin) {
        attribution.user = match.userIndex;
        attribution.credential = {match.type, match.credentialIndex};
    }
    bool accepted = BoltLockManager::Instance().Lock(OperationSourceEnum::kRemote, attribution);
    if (!accepted) {
        error = OperationErrorEnum::kUnspecified;
    }
    return accepted;
}

bool emberAfPluginDoorLockOnDoorUnlockCommand(EndpointId endpoint, const Nullable<FabricIndex>  &fabricIndex,
                                              const Nullable<NodeId>  &nodeId, const Optional<ByteSpan>  &pin,
                                              OperationErrorEnum  &error)
{
    DoorLockStorage::Match match;
    bool usedPin = false;
    if (!ValidateRemotePIN(endpoint, pin, match, usedPin, error)) {
        ESP_LOGE(TAG, "Unlock rejected: source=remote endpoint=%u reason=authorization error=%u", endpoint,
                 to_underlying(error));
        return false;
    }
    BoltLockManager::Attribution attribution;
    attribution.fabric = fabricIndex;
    attribution.node = nodeId;
    if (usedPin) {
        attribution.user = match.userIndex;
        attribution.credential = {match.type, match.credentialIndex};
    }
    if (!BoltLockManager::Instance().Unlock(OperationSourceEnum::kRemote, attribution)) {
        error = OperationErrorEnum::kUnspecified;
        ESP_LOGE(TAG, "Unlock rejected: source=remote endpoint=%u reason=actuator", endpoint);
        return false;
    }
    if (usedPin) {
        ESP_LOGW(TAG, "Unlock event: source=remote endpoint=%u user=%u credential=pin:%u", endpoint,
                 match.userIndex, match.credentialIndex);
    } else {
        ESP_LOGW(TAG, "Unlock event: source=remote endpoint=%u user=none credential=none", endpoint);
    }
    return true;
}

bool emberAfPluginDoorLockGetCredential(EndpointId endpoint, uint16_t credentialIndex,
                                        CredentialTypeEnum credentialType,
                                        EmberAfPluginDoorLockCredentialInfo  &credential)
{
    return DoorLockStorage::Instance().GetCredential(credentialIndex, credentialType, credential);
}

bool emberAfPluginDoorLockSetCredential(EndpointId endpoint, uint16_t credentialIndex, FabricIndex creator,
                                        FabricIndex modifier, DlCredentialStatus credentialStatus,
                                        CredentialTypeEnum credentialType, const ByteSpan  &credentialData)
{
    return DoorLockStorage::Instance().SetCredential(credentialIndex, creator, modifier, credentialStatus,
                                                     credentialType, credentialData);
}

bool emberAfPluginDoorLockGetUser(EndpointId endpoint, uint16_t userIndex,
                                  EmberAfPluginDoorLockUserInfo  &user)
{
    return DoorLockStorage::Instance().GetUser(userIndex, user);
}

bool emberAfPluginDoorLockSetUser(EndpointId endpoint, uint16_t userIndex, FabricIndex creator,
                                  FabricIndex modifier, const CharSpan  &userName, uint32_t uniqueId,
                                  UserStatusEnum userStatus, UserTypeEnum userType,
                                  CredentialRuleEnum credentialRule, const CredentialStruct * credentials,
                                  size_t totalCredentials)
{
    return DoorLockStorage::Instance().SetUser(userIndex, creator, modifier, userName, uniqueId, userStatus, userType,
                                               credentialRule, credentials, totalCredentials);
}

void emberAfPluginDoorLockOnAutoRelock(EndpointId endpoint)
{
    BoltLockManager::Instance().Lock(OperationSourceEnum::kAuto);
}
