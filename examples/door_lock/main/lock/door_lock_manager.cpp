/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "door_lock_manager.h"

#include <platform/ESP32/ESP32Config.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <cstring>
#include <esp_log.h>

static const char *TAG = "doorlock_manager";

BoltLockManager BoltLockManager::sLock;

TimerHandle_t sLockTimer;

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::DeviceLayer::Internal;
using namespace ESP32DoorLock::LockInitParams;
using namespace chip::Protocols::InteractionModel;

CHIP_ERROR BoltLockManager::Init(DataModel::Nullable<DoorLock::DlLockState> state,
                                 LockParam lockParam)
{
    LockParams = lockParam;

    if (LockParams.numberOfUsers > kMaxUsers)
    {
        ESP_LOGI(TAG,"Max number of users is greater than %d, the maximum amount of users currently supported on this platform", kMaxUsers);
        return CHIP_ERROR_NO_MEMORY;
    }

    if (LockParams.numberOfCredentialsPerUser > kMaxCredentialsPerUser)
    {
        ESP_LOGI(TAG, "Max number of credentials per user is greater than %d, the maximum amount of users currently supported on "
                      "this platform", kMaxCredentialsPerUser);
        return CHIP_ERROR_NO_MEMORY;
    }

    if (LockParams.numberOfWeekdaySchedulesPerUser > kMaxWeekdaySchedulesPerUser)
    {
        ESP_LOGI(TAG, " Max number of schedules is greater than %d, the maximum amount of schedules currently supported on this platform",
            kMaxWeekdaySchedulesPerUser);
        return CHIP_ERROR_NO_MEMORY;
    }

    if (LockParams.numberOfYeardaySchedulesPerUser > kMaxYeardaySchedulesPerUser)
    {
        ESP_LOGI(TAG, "Max number of schedules is greater than %d, the maximum amount of schedules currently supported on this platform",
            kMaxYeardaySchedulesPerUser);
        return CHIP_ERROR_NO_MEMORY;
    }

    if (LockParams.numberOfHolidaySchedules > kMaxHolidaySchedules)
    {
        ESP_LOGI(TAG, "Max number of schedules is greater than %d, the maximum amount of schedules currently supported on this platform",
            kMaxHolidaySchedules);
        return CHIP_ERROR_NO_MEMORY;
    }

    return CHIP_NO_ERROR;
}

bool BoltLockManager::IsValidUserIndex(uint16_t userIndex)
{
    return (userIndex < kMaxUsers);
}

bool BoltLockManager::IsValidCredentialIndex(uint16_t credentialIndex, CredentialTypeEnum type)
{
    if (CredentialTypeEnum::kProgrammingPIN == type)
    {
        return (0 == credentialIndex); // 0 is required index for Programming PIN
    }
    return (credentialIndex < kMaxCredentialsPerUser);
}

bool BoltLockManager::IsValidWeekdayScheduleIndex(uint8_t scheduleIndex)
{
    return (scheduleIndex < kMaxWeekdaySchedulesPerUser);
}

bool BoltLockManager::IsValidYeardayScheduleIndex(uint8_t scheduleIndex)
{
    return (scheduleIndex < kMaxYeardaySchedulesPerUser);
}

bool BoltLockManager::IsValidHolidayScheduleIndex(uint8_t scheduleIndex)
{
    return (scheduleIndex < kMaxHolidaySchedules);
}

bool BoltLockManager::ReadConfigValues()
{
    size_t outLen;
    ESP32Config::ReadConfigValueBin(ESP32Config::kConfigKey_LockUser, reinterpret_cast<uint8_t *>(&mLockUsers),
                                    sizeof(EmberAfPluginDoorLockUserInfo) * ArraySize(mLockUsers), outLen);

    ESP32Config::ReadConfigValueBin(ESP32Config::kConfigKey_Credential, reinterpret_cast<uint8_t *>(&mLockCredentials),
                                    sizeof(EmberAfPluginDoorLockCredentialInfo) * ArraySize(mLockCredentials), outLen);

    ESP32Config::ReadConfigValueBin(ESP32Config::kConfigKey_LockUserName, reinterpret_cast<uint8_t *>(mUserNames),
                                    sizeof(mUserNames), outLen);

    ESP32Config::ReadConfigValueBin(ESP32Config::kConfigKey_CredentialData, reinterpret_cast<uint8_t *>(mCredentialData),
                                    sizeof(mCredentialData), outLen);

    ESP32Config::ReadConfigValueBin(ESP32Config::kConfigKey_UserCredentials, reinterpret_cast<uint8_t *>(mCredentials),
                                    sizeof(CredentialStruct) * LockParams.numberOfUsers * LockParams.numberOfCredentialsPerUser,
                                    outLen);

    ESP32Config::ReadConfigValueBin(ESP32Config::kConfigKey_WeekDaySchedules, reinterpret_cast<uint8_t *>(mWeekdaySchedule),
                                    sizeof(EmberAfPluginDoorLockWeekDaySchedule) * LockParams.numberOfWeekdaySchedulesPerUser *
                                        LockParams.numberOfUsers,
                                    outLen);

    ESP32Config::ReadConfigValueBin(ESP32Config::kConfigKey_YearDaySchedules, reinterpret_cast<uint8_t *>(mYeardaySchedule),
                                    sizeof(EmberAfPluginDoorLockYearDaySchedule) * LockParams.numberOfYeardaySchedulesPerUser *
                                        LockParams.numberOfUsers,
                                    outLen);

    ESP32Config::ReadConfigValueBin(ESP32Config::kConfigKey_HolidaySchedules, reinterpret_cast<uint8_t *>(&(mHolidaySchedule)),
                                    sizeof(EmberAfPluginDoorLockHolidaySchedule) * LockParams.numberOfHolidaySchedules, outLen);

    return true;
}

bool BoltLockManager::Lock(EndpointId endpointId, const Optional<ByteSpan> & pin, OperationErrorEnum & err)
{
    return setLockState(endpointId, DlLockState::kLocked, pin, err);
}

bool BoltLockManager::Unlock(EndpointId endpointId, const Optional<ByteSpan> & pin, OperationErrorEnum & err)
{
    return setLockState(endpointId, DlLockState::kUnlocked, pin, err);
}

bool BoltLockManager::GetUser(EndpointId endpointId, uint16_t userIndex, EmberAfPluginDoorLockUserInfo & user)
{
    VerifyOrReturnValue(userIndex > 0, false); // indices are one-indexed

    userIndex--;

    VerifyOrReturnValue(IsValidUserIndex(userIndex), false);

    ESP_LOGI(TAG, "Door Lock App: BoltLockManager::GetUser [endpoint=%d,userIndex=%hu]", endpointId, userIndex);

    const auto & userInDb = mLockUsers[userIndex];

    user.userStatus = userInDb.userStatus;
    if (UserStatusEnum::kAvailable == user.userStatus)
    {
        ESP_LOGI(TAG, "Found unoccupied user [endpoint=%d]", endpointId);
        return true;
    }

    user.userName       = CharSpan(userInDb.userName.data(), userInDb.userName.size());
    user.credentials    = Span<const CredentialStruct>(mCredentials[userIndex], userInDb.credentials.size());
    user.userUniqueId   = userInDb.userUniqueId;
    user.userType       = userInDb.userType;
    user.credentialRule = userInDb.credentialRule;
    // So far there's no way to actually create the credential outside Matter, so here we always set the creation/modification
    // source to Matter
    user.creationSource     = DlAssetSource::kMatterIM;
    user.createdBy          = userInDb.createdBy;
    user.modificationSource = DlAssetSource::kMatterIM;
    user.lastModifiedBy     = userInDb.lastModifiedBy;

    ESP_LOGI(TAG, "Found occupied user [endpoint=%d,name=\"%.*s\",credentialsCount=%u,uniqueId=%" PRIu32
             ",type=%u,credentialRule=%u,createdBy=%d,lastModifiedBy=%d]",
             endpointId, static_cast<int>(user.userName.size()), user.userName.data(), user.credentials.size(), user.userUniqueId,
             to_underlying(user.userType), to_underlying(user.credentialRule), user.createdBy, user.lastModifiedBy);

    return true;
}

bool BoltLockManager::SetUser(EndpointId endpointId, uint16_t userIndex, FabricIndex creator,
                              FabricIndex modifier, const CharSpan & userName, uint32_t uniqueId,
                              UserStatusEnum userStatus, UserTypeEnum usertype, CredentialRuleEnum credentialRule,
                              const CredentialStruct * credentials, size_t totalCredentials)
{
    ESP_LOGI(TAG, "Door Lock App: BoltLockManager::SetUser "
             "[endpoint=%d,userIndex=%d,creator=%d,modifier=%d,userName=%s,uniqueId=%" PRIu32 ""
             "userStatus=%u,userType=%u,credentialRule=%u,credentials=%p,totalCredentials=%u]",
             endpointId, userIndex, creator, modifier, userName.data(), uniqueId, to_underlying(userStatus),
             to_underlying(usertype), to_underlying(credentialRule), credentials, totalCredentials);

    VerifyOrReturnValue(userIndex > 0, false); // indices are one-indexed

    userIndex--;

    VerifyOrReturnValue(IsValidUserIndex(userIndex), false);

    auto & userInStorage = mLockUsers[userIndex];

    if (userName.size() > DOOR_LOCK_MAX_USER_NAME_SIZE)
    {
        ESP_LOGE(TAG, "Cannot set user - user name is too long [endpoint=%d,index=%d]", endpointId, userIndex);
        return false;
    }

    if (totalCredentials > LockParams.numberOfCredentialsPerUser)
    {
        ESP_LOGE(TAG, "Cannot set user - total number of credentials is too big [endpoint=%d,index=%d,totalCredentials=%u]",
                 endpointId, userIndex, totalCredentials);
        return false;
    }

    Platform::CopyString(mUserNames[userIndex], userName);
    userInStorage.userName       = CharSpan(mUserNames[userIndex], userName.size());
    userInStorage.userUniqueId   = uniqueId;
    userInStorage.userStatus     = userStatus;
    userInStorage.userType       = usertype;
    userInStorage.credentialRule = credentialRule;
    userInStorage.lastModifiedBy = modifier;
    userInStorage.createdBy      = creator;

    for (size_t i = 0; i < totalCredentials; ++i)
    {
        mCredentials[userIndex][i] = credentials[i];
    }
    userInStorage.credentials = Span<const CredentialStruct>(mCredentials[userIndex], totalCredentials);

    // Save user information in NVM flash
    ESP32Config::WriteConfigValueBin(ESP32Config::kConfigKey_LockUser, reinterpret_cast<const uint8_t *>(&mLockUsers),
                                     sizeof(EmberAfPluginDoorLockUserInfo) * LockParams.numberOfUsers);

    ESP32Config::WriteConfigValueBin(ESP32Config::kConfigKey_UserCredentials, reinterpret_cast<const uint8_t *>(mCredentials),
                                     sizeof(CredentialStruct) * LockParams.numberOfUsers * LockParams.numberOfCredentialsPerUser);

    ESP32Config::WriteConfigValueBin(ESP32Config::kConfigKey_LockUserName, reinterpret_cast<const uint8_t *>(mUserNames),
                                     sizeof(mUserNames));

    ESP_LOGI(TAG, "Successfully set the user [mEndpointId=%d,index=%d]", endpointId, userIndex);

    return true;
}

bool BoltLockManager::GetCredential(EndpointId endpointId, uint16_t credentialIndex, CredentialTypeEnum credentialType,
                                    EmberAfPluginDoorLockCredentialInfo & credential)
{

    if (CredentialTypeEnum::kProgrammingPIN == credentialType)
    {
        VerifyOrReturnValue(IsValidCredentialIndex(credentialIndex, credentialType),
                            false); // programming pin index is only index allowed to contain 0
    }
    else
    {
        VerifyOrReturnValue(IsValidCredentialIndex(--credentialIndex, credentialType), false); // otherwise, indices are one-indexed
    }

    ESP_LOGI(TAG, "Lock App: BoltLockManager::GetCredential [credentialType=%u], credentialIndex=%d", to_underlying(credentialType),
             credentialIndex);

    const auto & credentialInStorage = mLockCredentials[credentialIndex];

    credential.status = credentialInStorage.status;
    ESP_LOGI(TAG, "CredentialStatus: %d, CredentialIndex: %d ", (int) credential.status, credentialIndex);

    if (DlCredentialStatus::kAvailable == credential.status)
    {
        ESP_LOGI(TAG, "Found unoccupied credential ");
        return true;
    }
    credential.credentialType = credentialInStorage.credentialType;
    credential.credentialData = credentialInStorage.credentialData;
    credential.createdBy      = credentialInStorage.createdBy;
    credential.lastModifiedBy = credentialInStorage.lastModifiedBy;
    // So far there's no way to actually create the credential outside Matter, so here we always set the creation/modification
    // source to Matter
    credential.creationSource     = DlAssetSource::kMatterIM;
    credential.modificationSource = DlAssetSource::kMatterIM;

    ESP_LOGI(TAG, "Found occupied credential [type=%u,dataSize=%u]", to_underlying(credential.credentialType),
             credential.credentialData.size());

    return true;
}

bool BoltLockManager::SetCredential(EndpointId endpointId, uint16_t credentialIndex, FabricIndex creator,
                                    FabricIndex modifier, DlCredentialStatus credentialStatus,
                                    CredentialTypeEnum credentialType, const ByteSpan & credentialData)
{

    if (CredentialTypeEnum::kProgrammingPIN == credentialType)
    {
        VerifyOrReturnValue(IsValidCredentialIndex(credentialIndex, credentialType),
                            false); // programming pin index is only index allowed to contain 0
    }
    else
    {
        VerifyOrReturnValue(IsValidCredentialIndex(--credentialIndex, credentialType), false); // otherwise, indices are one-indexed
    }

    ESP_LOGI(TAG, "Door Lock App: BoltLockManager::SetCredential "
             "[credentialStatus=%u,credentialType=%u,credentialDataSize=%u,creator=%d,modifier=%d]",
             to_underlying(credentialStatus), to_underlying(credentialType), credentialData.size(), creator, modifier);

    auto & credentialInStorage = mLockCredentials[credentialIndex];

    credentialInStorage.status         = credentialStatus;
    credentialInStorage.credentialType = credentialType;
    credentialInStorage.createdBy      = creator;
    credentialInStorage.lastModifiedBy = modifier;

    memcpy(mCredentialData[credentialIndex], credentialData.data(), credentialData.size());
    credentialInStorage.credentialData = ByteSpan{ mCredentialData[credentialIndex], credentialData.size() };

    // Save credential information in NVM flash
    ESP32Config::WriteConfigValueBin(ESP32Config::kConfigKey_Credential, reinterpret_cast<const uint8_t *>(&mLockCredentials),
                                     sizeof(EmberAfPluginDoorLockCredentialInfo) * LockParams.numberOfCredentialsPerUser);

    ESP32Config::WriteConfigValueBin(ESP32Config::kConfigKey_CredentialData, reinterpret_cast<const uint8_t *>(&mCredentialData),
                                     sizeof(mCredentialData));

    ESP_LOGI(TAG, "Successfully set the credential [credentialType=%u]", to_underlying(credentialType));

    return true;
}

DlStatus BoltLockManager::GetWeekdaySchedule(EndpointId endpointId, uint8_t weekdayIndex, uint16_t userIndex,
                                             EmberAfPluginDoorLockWeekDaySchedule & schedule)
{

    VerifyOrReturnValue(weekdayIndex > 0, DlStatus::kFailure); // indices are one-indexed
    VerifyOrReturnValue(userIndex > 0, DlStatus::kFailure);    // indices are one-indexed

    weekdayIndex--;
    userIndex--;

    VerifyOrReturnValue(IsValidWeekdayScheduleIndex(weekdayIndex), DlStatus::kFailure);
    VerifyOrReturnValue(IsValidUserIndex(userIndex), DlStatus::kFailure);

    const auto & scheduleInStorage = mWeekdaySchedule[userIndex][weekdayIndex];
    if (DlScheduleStatus::kAvailable == scheduleInStorage.status)
    {
        return DlStatus::kNotFound;
    }

    schedule = scheduleInStorage.schedule;

    return DlStatus::kSuccess;
}

DlStatus BoltLockManager::SetWeekdaySchedule(EndpointId endpointId, uint8_t weekdayIndex, uint16_t userIndex,
                                             DlScheduleStatus status, DaysMaskMap daysMask, uint8_t startHour, uint8_t startMinute,
                                             uint8_t endHour, uint8_t endMinute)
{

    VerifyOrReturnValue(weekdayIndex > 0, DlStatus::kFailure); // indices are one-indexed
    VerifyOrReturnValue(userIndex > 0, DlStatus::kFailure);    // indices are one-indexed

    weekdayIndex--;
    userIndex--;

    VerifyOrReturnValue(IsValidWeekdayScheduleIndex(weekdayIndex), DlStatus::kFailure);
    VerifyOrReturnValue(IsValidUserIndex(userIndex), DlStatus::kFailure);

    auto & scheduleInStorage = mWeekdaySchedule[userIndex][weekdayIndex];

    scheduleInStorage.schedule.daysMask    = daysMask;
    scheduleInStorage.schedule.startHour   = startHour;
    scheduleInStorage.schedule.startMinute = startMinute;
    scheduleInStorage.schedule.endHour     = endHour;
    scheduleInStorage.schedule.endMinute   = endMinute;
    scheduleInStorage.status               = status;

    // Save schedule information in NVM flash
    ESP32Config::WriteConfigValueBin(ESP32Config::kConfigKey_WeekDaySchedules, reinterpret_cast<const uint8_t *>(mWeekdaySchedule),
                                     sizeof(EmberAfPluginDoorLockWeekDaySchedule) * LockParams.numberOfWeekdaySchedulesPerUser *
                                         LockParams.numberOfUsers);

    return DlStatus::kSuccess;
}

DlStatus BoltLockManager::GetYeardaySchedule(EndpointId endpointId, uint8_t yearDayIndex, uint16_t userIndex,
                                             EmberAfPluginDoorLockYearDaySchedule & schedule)
{
    VerifyOrReturnValue(yearDayIndex > 0, DlStatus::kFailure); // indices are one-indexed
    VerifyOrReturnValue(userIndex > 0, DlStatus::kFailure);    // indices are one-indexed

    yearDayIndex--;
    userIndex--;

    VerifyOrReturnValue(IsValidYeardayScheduleIndex(yearDayIndex), DlStatus::kFailure);
    VerifyOrReturnValue(IsValidUserIndex(userIndex), DlStatus::kFailure);

    const auto & scheduleInStorage = mYeardaySchedule[userIndex][yearDayIndex];
    if (DlScheduleStatus::kAvailable == scheduleInStorage.status)
    {
        return DlStatus::kNotFound;
    }

    schedule = scheduleInStorage.schedule;

    return DlStatus::kSuccess;
}

DlStatus BoltLockManager::SetYeardaySchedule(EndpointId endpointId, uint8_t yearDayIndex, uint16_t userIndex,
                                             DlScheduleStatus status, uint32_t localStartTime, uint32_t localEndTime)
{
    VerifyOrReturnValue(yearDayIndex > 0, DlStatus::kFailure); // indices are one-indexed
    VerifyOrReturnValue(userIndex > 0, DlStatus::kFailure);    // indices are one-indexed

    yearDayIndex--;
    userIndex--;

    VerifyOrReturnValue(IsValidYeardayScheduleIndex(yearDayIndex), DlStatus::kFailure);
    VerifyOrReturnValue(IsValidUserIndex(userIndex), DlStatus::kFailure);

    auto & scheduleInStorage = mYeardaySchedule[userIndex][yearDayIndex];

    scheduleInStorage.schedule.localStartTime = localStartTime;
    scheduleInStorage.schedule.localEndTime   = localEndTime;
    scheduleInStorage.status                  = status;

    // Save schedule information in NVM flash
    ESP32Config::WriteConfigValueBin(ESP32Config::kConfigKey_YearDaySchedules, reinterpret_cast<const uint8_t *>(mYeardaySchedule),
                                     sizeof(EmberAfPluginDoorLockYearDaySchedule) * LockParams.numberOfYeardaySchedulesPerUser *
                                         LockParams.numberOfUsers);

    return DlStatus::kSuccess;
}

DlStatus BoltLockManager::GetHolidaySchedule(EndpointId endpointId, uint8_t holidayIndex,
                                             EmberAfPluginDoorLockHolidaySchedule & schedule)
{
    VerifyOrReturnValue(holidayIndex > 0, DlStatus::kFailure); // indices are one-indexed

    holidayIndex--;

    VerifyOrReturnValue(IsValidHolidayScheduleIndex(holidayIndex), DlStatus::kFailure);

    const auto & scheduleInStorage = mHolidaySchedule[holidayIndex];
    if (DlScheduleStatus::kAvailable == scheduleInStorage.status)
    {
        return DlStatus::kNotFound;
    }

    schedule = scheduleInStorage.schedule;

    return DlStatus::kSuccess;
}

DlStatus BoltLockManager::SetHolidaySchedule(EndpointId endpointId, uint8_t holidayIndex, DlScheduleStatus status,
                                             uint32_t localStartTime, uint32_t localEndTime, OperatingModeEnum operatingMode)
{
    VerifyOrReturnValue(holidayIndex > 0, DlStatus::kFailure); // indices are one-indexed

    holidayIndex--;

    VerifyOrReturnValue(IsValidHolidayScheduleIndex(holidayIndex), DlStatus::kFailure);

    auto & scheduleInStorage = mHolidaySchedule[holidayIndex];

    scheduleInStorage.schedule.localStartTime = localStartTime;
    scheduleInStorage.schedule.localEndTime   = localEndTime;
    scheduleInStorage.schedule.operatingMode  = operatingMode;
    scheduleInStorage.status                  = status;

    // Save schedule information in NVM flash
    ESP32Config::WriteConfigValueBin(ESP32Config::kConfigKey_HolidaySchedules,
                                     reinterpret_cast<const uint8_t *>(&(mHolidaySchedule)),
                                     sizeof(EmberAfPluginDoorLockHolidaySchedule) * LockParams.numberOfHolidaySchedules);

    return DlStatus::kSuccess;
}

const char * BoltLockManager::lockStateToString(DlLockState lockState) const
{
    switch (lockState)
    {
    case DlLockState::kNotFullyLocked:
        return "Not Fully Locked";
    case DlLockState::kLocked:
        return "Locked";
    case DlLockState::kUnlocked:
        return "Unlocked";
    case DlLockState::kUnlatched:
        return "Unlatched";
    case DlLockState::kUnknownEnumValue:
        break;
    }

    return "Unknown";
}

bool BoltLockManager::setLockState(EndpointId endpointId, DlLockState lockState, const Optional<ByteSpan> & pin,
                                   OperationErrorEnum & err)
{

    // Assume pin is required until told otherwise
    bool requirePin = true;
    if ( Status::Success != DoorLock::Attributes::RequirePINforRemoteOperation::Get(endpointId, &requirePin)) {
        requirePin = false;
    }

    // If a pin code is not given
    if (!pin.HasValue())
    {
        ESP_LOGI(TAG, "Door Lock App: PIN code is not specified [endpointId=%d]", endpointId);

        // If a pin code is not required
        if (!requirePin)
        {
            ESP_LOGI(TAG, "Door Lock App: setting door lock state to \"%s\" [endpointId=%d]", lockStateToString(lockState),
                     endpointId);

            DoorLockServer::Instance().SetLockState(endpointId, lockState);

            return true;
        }

        ESP_LOGI(TAG, "Door Lock App: PIN code is not specified, but it is required [endpointId=%d]", endpointId);

        return false;
    }

    // Check the PIN code
    for (uint8_t i = 0; i < kMaxCredentials; i++)
    {
        if (mLockCredentials[i].credentialType != CredentialTypeEnum::kPin ||
            mLockCredentials[i].status == DlCredentialStatus::kAvailable)
        {
            continue;
        }

        if (mLockCredentials[i].credentialData.data_equal(pin.Value()))
        {
            ESP_LOGI(TAG, "Lock App: specified PIN code was found in the database, setting lock state to \"%s\" [endpointId=%d]",
                     lockStateToString(lockState), endpointId);

            DoorLockServer::Instance().SetLockState(endpointId, lockState);

            return true;
        }
    }

    ESP_LOGI(TAG, "Door Lock App: specified PIN code was not found in the database, ignoring command to set lock state to \"%s\" "
             "[endpointId=%d]",
             lockStateToString(lockState), endpointId);

    err = OperationErrorEnum::kInvalidCredential;
    return false;
}

CHIP_ERROR BoltLockManager::InitLockState()
{
    // Initial lock state
    DataModel::Nullable<DoorLock::DlLockState> state;
    EndpointId endpointId{ 1 };
    DoorLock::Attributes::LockState::Get(endpointId, state);

    uint8_t numberOfCredentialsPerUser = 0;
    if (!DoorLockServer::Instance().GetNumberOfCredentialsSupportedPerUser(endpointId, numberOfCredentialsPerUser))
    {
        ESP_LOGE(TAG, "Unable to get number of credentials supported per user when initializing lock endpoint, defaulting to 5 [endpointId=%d]", endpointId);
        numberOfCredentialsPerUser = 5;
    }

    uint16_t numberOfUsers = 0;
    if (!DoorLockServer::Instance().GetNumberOfUserSupported(endpointId, numberOfUsers))
    {
        ESP_LOGE(TAG, "Unable to get number of supported users when initializing lock endpoint, defaulting to 10 [endpointId=%d]", endpointId);
        numberOfUsers = 10;
    }

    CHIP_ERROR err = BoltLockMgr().Init(state, ParamBuilder()
                                               .SetNumberOfUsers(numberOfUsers)
                                               .SetNumberOfCredentialsPerUser(numberOfCredentialsPerUser)
                                               .GetLockParam());
    ReadConfigValues();
    if (err != CHIP_NO_ERROR)
    {
        ESP_LOGE(TAG, "BoltLockMgr().Init() failed");
        return err;
    }

    return err;
}
