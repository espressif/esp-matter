/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "door_lock_console.h"

#include "bolt_lock_manager.h"
#include "door_lock_capabilities.h"
#include "door_lock_authorization.h"
#include "door_lock_storage.h"
#if CONFIG_ENABLE_TIME_SYNC
#include "time_sync.h"
#endif // CONFIG_ENABLE_TIME_SYNC

#include <app/clusters/door-lock-server/door-lock-server.h>
#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_console.h>

#include <cstring>

using namespace chip;
using namespace chip::app::Clusters::DoorLock;

namespace {
const char * TAG = "door_lock_console";

const char * credential_type_name(CredentialTypeEnum type)
{
    switch (type) {
    case CredentialTypeEnum::kProgrammingPIN:
        return "programming-pin";
    case CredentialTypeEnum::kPin:
        return "pin";
    case CredentialTypeEnum::kAliroCredentialIssuerKey:
        return "aliro-issuer";
    case CredentialTypeEnum::kAliroEvictableEndpointKey:
        return "aliro-evictable";
    case CredentialTypeEnum::kAliroNonEvictableEndpointKey:
        return "aliro-non-evictable";
    default:
        return "not supported";
    }
}

uint16_t count_occupied_credentials(CredentialTypeEnum type, uint16_t slotCount)
{
    uint16_t occupied = 0;
    for (uint16_t credentialIndex = 1; credentialIndex <= slotCount; ++credentialIndex) {
        EmberAfPluginDoorLockCredentialInfo credential;
        if (DoorLockStorage::Instance().GetCredential(credentialIndex, type, credential) &&
                credential.status == DlCredentialStatus::kOccupied) {
            ++occupied;
        }
    }
    return occupied;
}

void print_door_lock_status()
{
    uint16_t occupiedUsers = 0;
    for (uint16_t userIndex = 1; userIndex <= DoorLockCapabilities::kUsers; ++userIndex) {
        EmberAfPluginDoorLockUserInfo user;
        if (DoorLockStorage::Instance().GetUser(userIndex, user) && user.userStatus != UserStatusEnum::kAvailable) {
            ++occupiedUsers;
        }
    }

    const uint16_t occupiedPins =
        count_occupied_credentials(CredentialTypeEnum::kPin, DoorLockCapabilities::kPinCredentialSlots);
    printf("Users: %u occupied, %u available\n", occupiedUsers, DoorLockCapabilities::kUsers - occupiedUsers);
    printf("PIN slots: %u occupied, %u available\n", occupiedPins,
           DoorLockCapabilities::kPinCredentialSlots - occupiedPins);
#ifdef CONFIG_ENABLE_ALIRO_OVER_NFC
    const uint16_t occupiedIssuers = count_occupied_credentials(
                                         CredentialTypeEnum::kAliroCredentialIssuerKey, DoorLockCapabilities::kAliroCredentialIssuerKeySlots);
    const uint16_t occupiedEvictable = count_occupied_credentials(
                                           CredentialTypeEnum::kAliroEvictableEndpointKey, DoorLockCapabilities::kAliroEndpointKeySlots);
    const uint16_t occupiedNonEvictable = count_occupied_credentials(
                                              CredentialTypeEnum::kAliroNonEvictableEndpointKey, DoorLockCapabilities::kAliroEndpointKeySlots);
    const uint16_t occupiedEndpoints = occupiedEvictable + occupiedNonEvictable;
    printf("Aliro issuer slots: %u occupied, %u available\n", occupiedIssuers,
           DoorLockCapabilities::kAliroCredentialIssuerKeySlots - occupiedIssuers);
    printf("Aliro endpoint slots: %u occupied, %u available\n", occupiedEndpoints,
           DoorLockCapabilities::kAliroEndpointKeySlots - occupiedEndpoints);
#endif // CONFIG_ENABLE_ALIRO_OVER_NFC
}

void print_door_lock_users()
{
    for (uint16_t userIndex = 1; userIndex <= DoorLockCapabilities::kUsers; ++userIndex) {
        EmberAfPluginDoorLockUserInfo user;
        if (!DoorLockStorage::Instance().GetUser(userIndex, user)) {
            printf("User %u: read failed\n", userIndex);
            continue;
        }
        if (user.userStatus == UserStatusEnum::kAvailable) {
            printf("User %u: available\n", userIndex);
            continue;
        }

        printf("User %u: %s, credentials=[", userIndex,
               user.userStatus == UserStatusEnum::kOccupiedEnabled ? "enabled" : "disabled");
        for (size_t i = 0; i < user.credentials.size(); ++i) {
            if (i != 0) {
                printf(", ");
            }
            printf("%s:%u", credential_type_name(user.credentials[i].credentialType),
                   user.credentials[i].credentialIndex);
        }
        printf("]\n");
    }
}

void print_door_lock_console_help()
{
    printf("Door lock commands:\n"
           "\thelp: Print help\n"
           "\tstatus: Print aggregate user and credential capacity\n"
           "\tusers: Print users and credential references without credential data\n"
#if CONFIG_ENABLE_TIME_SYNC
           "\ttime: Print UTC and synchronization readiness\n"
#endif // CONFIG_ENABLE_TIME_SYNC
           "\tpin-unlock <PIN>: Authorize a PIN and unlock locally\n");
}

esp_err_t door_lock_console_handler(int argc, char ** argv)
{
    esp_matter::lock::ScopedChipStackLock lock(portMAX_DELAY);

    if (argc == 1 && std::strcmp(argv[0], "help") == 0) {
        print_door_lock_console_help();
        return ESP_OK;
    }
    if (argc == 1 && std::strcmp(argv[0], "status") == 0) {
        print_door_lock_status();
        return ESP_OK;
    }
    if (argc == 1 && std::strcmp(argv[0], "users") == 0) {
        print_door_lock_users();
        return ESP_OK;
    }
#if CONFIG_ENABLE_TIME_SYNC
    if (argc == 1 && std::strcmp(argv[0], "time") == 0) {
        time_t utc;
        uint32_t generation;
        const bool ready = TimeSync::Now(utc, generation);
        printf("Time ready: %s\nUTC (Unix seconds): %lld\nClock generation: %lu\n",
               ready ? "yes" : "no", static_cast<long long>(utc), static_cast<unsigned long>(generation));
        return ESP_OK;
    }
#endif // CONFIG_ENABLE_TIME_SYNC
    if (argc == 2 && std::strcmp(argv[0], "pin-unlock") == 0) {
        const ByteSpan pin(reinterpret_cast<const uint8_t *>(argv[1]), std::strlen(argv[1]));
        DoorLockStorage::Match match;
        OperationErrorEnum error = OperationErrorEnum::kRestricted;
        if (!DoorLockStorage::Instance().ValidatePIN(pin, match) ||
                !DoorLockAuthorization::Authorize(match, OperationSourceEnum::kKeypad, error)) {
            ESP_LOGE(TAG, "Unlock rejected: source=keypad reason=authorization error=%u", static_cast<unsigned>(error));
            DoorLockAuthorization::ReportUnlockFailure(match, OperationSourceEnum::kKeypad, error);
            return ESP_ERR_INVALID_ARG;
        }

        BoltLockManager::Attribution attribution;
        attribution.user = match.userIndex;
        attribution.credential = {match.type, match.credentialIndex};
        if (!BoltLockManager::Instance().Unlock(OperationSourceEnum::kKeypad, attribution)) {
            ESP_LOGE(TAG, "Unlock rejected: source=keypad user=%u credential=pin:%u reason=actuator", match.userIndex,
                     match.credentialIndex);
            DoorLockAuthorization::ReportUnlockFailure(match, OperationSourceEnum::kKeypad,
                                                       OperationErrorEnum::kUnspecified);
            return ESP_FAIL;
        }
        ESP_LOGW(TAG, "Unlock event: source=keypad user=%u credential=pin:%u", match.userIndex,
                 match.credentialIndex);
        return ESP_OK;
    }

    print_door_lock_console_help();
    return ESP_ERR_INVALID_ARG;
}
} // namespace

esp_err_t door_lock_console_register_commands()
{
    static const esp_matter::console::command_t command = {
        .name = "dl",
        .description = "Inspect door-lock storage or test local PIN unlock. Usage: matter esp dl help",
        .handler = door_lock_console_handler,
    };
    return esp_matter::console::add_commands(&command, 1);
}
