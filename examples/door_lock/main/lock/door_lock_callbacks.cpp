/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "bolt_lock_manager.h"
#include "door_lock_storage.h"
#include "app_priv.h"
#ifdef CONFIG_ENABLE_ALIRO_OVER_NFC
#include "aliro_door_lock_delegate.h"
#endif

#include <app-common/zap-generated/attributes/Accessors.h>
#include <app/server/Server.h>
#include <esp_log.h>
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

bool ValidateRemotePIN(EndpointId endpoint, const Optional<ByteSpan>  &pin,
                       DoorLockStorage::PinMatch  &match, bool  &usedPin,
                       OperationErrorEnum  &error)
{
    usedPin = false;
    bool requirePin = false;
    if (DoorLock::Attributes::RequirePINforRemoteOperation::Get(endpoint, &requirePin) != Status::Success) {
        ESP_LOGW(TAG, "Unable to read RequirePINforRemoteOperation; allowing commands without a PIN");
        requirePin = false;
    }

    if (!pin.HasValue()) {
        if (requirePin) {
            error = OperationErrorEnum::kInvalidCredential;
            return false;
        }
        return true;
    }
    if (DoorLockStorage::Instance().ValidatePIN(pin.Value(), match)) {
        usedPin = true;
        return true;
    }

    error = OperationErrorEnum::kInvalidCredential;
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

bool ReadScheduleLimits(DoorLockServer  &server, EndpointId endpoint, DoorLockStorage::Limits  &limits)
{
    return server.GetNumberOfWeekDaySchedulesPerUserSupported(endpoint, limits.numberOfWeekdaySchedulesPerUser) &&
           server.GetNumberOfYearDaySchedulesPerUserSupported(endpoint, limits.numberOfYeardaySchedulesPerUser) &&
           server.GetNumberOfHolidaySchedulesSupported(endpoint, limits.numberOfHolidaySchedules);
}

void HandleBoltStateChange(BoltLockManager::State state, BoltLockManager::OperationSource source)
{
    DlLockState lockState = DlLockState::kNotFullyLocked;
    switch (state) {
    case BoltLockManager::State::kLockingInitiated:
        break;
    case BoltLockManager::State::kUnlockingInitiated:
        if (source == OperationSourceEnum::kButton) {
            ESP_LOGW(TAG, "Unlock event: source=button user=none credential=none");
        } else if (source == OperationSourceEnum::kAliro) {
            ESP_LOGW(TAG, "Unlock event: source=aliro user=unavailable credential=unavailable");
        }
        break;
    case BoltLockManager::State::kLockingCompleted:
        lockState = DlLockState::kLocked;
        break;
    case BoltLockManager::State::kUnlockingCompleted:
        lockState = DlLockState::kUnlocked;
        break;
    }

    if (!DoorLockServer::Instance().SetLockState(sDoorLockEndpoint, lockState, source)) {
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
    VerifyOrDie(ReadScheduleLimits(server, endpoint, limits));
    DoorLockStorage::Config storageConfig{ endpoint, limits, Server::GetInstance().GetPersistentStorage() };
    CHIP_ERROR err = DoorLockStorage::Instance().Init(storageConfig);
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Door lock storage initialization failed: %" CHIP_ERROR_FORMAT, err.Format());
    }
    VerifyOrDie(err == CHIP_NO_ERROR);

    err = BoltLockManager::Instance().Init(BoltLockManager::State::kLockingCompleted, HandleBoltStateChange);
    VerifyOrDie(err == CHIP_NO_ERROR);
    VerifyOrDie(server.SetLockState(endpoint, DlLockState::kLocked, OperationSourceEnum::kUnspecified));
}

bool emberAfPluginDoorLockOnDoorLockCommand(EndpointId endpoint, const Nullable<FabricIndex>  &fabricIndex,
                                            const Nullable<NodeId>  &nodeId, const Optional<ByteSpan>  &pin,
                                            OperationErrorEnum  &error)
{
    DoorLockStorage::PinMatch match;
    bool usedPin = false;
    VerifyOrReturnValue(ValidateRemotePIN(endpoint, pin, match, usedPin, error), false);
    return BoltLockManager::Instance().Lock(OperationSourceEnum::kRemote);
}

bool emberAfPluginDoorLockOnDoorUnlockCommand(EndpointId endpoint, const Nullable<FabricIndex>  &fabricIndex,
                                              const Nullable<NodeId>  &nodeId, const Optional<ByteSpan>  &pin,
                                              OperationErrorEnum  &error)
{
    DoorLockStorage::PinMatch match;
    bool usedPin = false;
    if (!ValidateRemotePIN(endpoint, pin, match, usedPin, error)) {
        ESP_LOGE(TAG, "Unlock rejected: source=remote endpoint=%u reason=invalid-credential", endpoint);
        return false;
    }
    if (!BoltLockManager::Instance().Unlock(OperationSourceEnum::kRemote)) {
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

DlStatus emberAfPluginDoorLockGetSchedule(EndpointId endpoint, uint8_t scheduleIndex, uint16_t userIndex,
                                          EmberAfPluginDoorLockWeekDaySchedule  &schedule)
{
    return DoorLockStorage::Instance().GetWeekdaySchedule(scheduleIndex, userIndex, schedule);
}

DlStatus emberAfPluginDoorLockGetSchedule(EndpointId endpoint, uint8_t scheduleIndex, uint16_t userIndex,
                                          EmberAfPluginDoorLockYearDaySchedule  &schedule)
{
    return DoorLockStorage::Instance().GetYeardaySchedule(scheduleIndex, userIndex, schedule);
}

DlStatus emberAfPluginDoorLockGetSchedule(EndpointId endpoint, uint8_t scheduleIndex,
                                          EmberAfPluginDoorLockHolidaySchedule  &schedule)
{
    return DoorLockStorage::Instance().GetHolidaySchedule(scheduleIndex, schedule);
}

DlStatus emberAfPluginDoorLockSetSchedule(EndpointId endpoint, uint8_t scheduleIndex, uint16_t userIndex,
                                          DlScheduleStatus status, DaysMaskMap daysMask, uint8_t startHour,
                                          uint8_t startMinute, uint8_t endHour, uint8_t endMinute)
{
    return DoorLockStorage::Instance().SetWeekdaySchedule(scheduleIndex, userIndex, status, daysMask, startHour,
                                                          startMinute, endHour, endMinute);
}

DlStatus emberAfPluginDoorLockSetSchedule(EndpointId endpoint, uint8_t scheduleIndex, uint16_t userIndex,
                                          DlScheduleStatus status, uint32_t localStartTime, uint32_t localEndTime)
{
    return DoorLockStorage::Instance().SetYeardaySchedule(scheduleIndex, userIndex, status, localStartTime,
                                                          localEndTime);
}

DlStatus emberAfPluginDoorLockSetSchedule(EndpointId endpoint, uint8_t scheduleIndex, DlScheduleStatus status,
                                          uint32_t localStartTime, uint32_t localEndTime,
                                          OperatingModeEnum operatingMode)
{
    return DoorLockStorage::Instance().SetHolidaySchedule(scheduleIndex, status, localStartTime, localEndTime,
                                                          operatingMode);
}

void emberAfPluginDoorLockOnAutoRelock(EndpointId endpoint)
{
    BoltLockManager::Instance().Lock(OperationSourceEnum::kAuto);
}
