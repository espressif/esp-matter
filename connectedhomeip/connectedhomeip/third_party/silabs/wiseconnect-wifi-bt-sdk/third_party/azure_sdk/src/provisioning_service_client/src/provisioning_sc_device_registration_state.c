// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>

#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/gballoc.h"

#include "prov_service_client/provisioning_sc_device_registration_state.h"
#include "prov_service_client/provisioning_sc_json_const.h"
#include "prov_service_client/provisioning_sc_shared_helpers.h"
#include "prov_service_client/provisioning_sc_models_serializer.h"
#include "parson.h"

typedef struct DEVICE_REGISTRATION_STATE_TAG
{
    char* registration_id;
    char* created_date_time_utc;
    char* device_id;
    REGISTRATION_STATUS status;
    char* updated_date_time_utc;
    int error_code;
    char* error_message;
    char* etag;
} DEVICE_REGISTRATION_STATE;

MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(REGISTRATION_STATUS, REGISTRATION_STATUS_VALUES)

static REGISTRATION_STATUS registrationStatus_fromJson(const char* str_rep)
{
    REGISTRATION_STATUS new_status = REGISTRATION_STATUS_ERROR;

    if (str_rep != NULL)
    {
        if (strcmp(str_rep, REGISTRATION_STATUS_JSON_VALUE_UNASSIGNED) == 0)
        {
            new_status = REGISTRATION_STATUS_UNASSIGNED;
        }
        else if (strcmp(str_rep, REGISTRATION_STATUS_JSON_VALUE_ASSIGNING) == 0)
        {
            new_status = REGISTRATION_STATUS_ASSIGNING;
        }
        else if (strcmp(str_rep, REGISTRATION_STATUS_JSON_VALUE_ASSIGNED) == 0)
        {
            new_status = REGISTRATION_STATUS_ASSIGNED;
        }
        else if (strcmp(str_rep, REGISTRATION_STATUS_JSON_VALUE_FAILED) == 0)
        {
            new_status = REGISTRATION_STATUS_FAILED;
        }
        else if (strcmp(str_rep, REGISTRATION_STATUS_JSON_VALUE_DISABLED) == 0)
        {
            new_status = REGISTRATION_STATUS_DISABLED;
        }
        else
        {
            LogError("Could not convert '%s' from JSON", str_rep);
        }
    }

    return new_status;
}

void deviceRegistrationState_destroy(DEVICE_REGISTRATION_STATE_HANDLE device_reg_state)
{
    if (device_reg_state != NULL)
    {
        free(device_reg_state->registration_id);
        free(device_reg_state->created_date_time_utc);
        free(device_reg_state->device_id);
        free(device_reg_state->updated_date_time_utc);
        free(device_reg_state->error_message);
        free(device_reg_state->etag);
        free(device_reg_state);
    }
}

DEVICE_REGISTRATION_STATE_HANDLE deviceRegistrationState_fromJson(JSON_Object* root_object)
{
    DEVICE_REGISTRATION_STATE_HANDLE new_device_reg_state = NULL;

    if (root_object == NULL)
    {
        LogError("No device registration state in JSON");
    }
    else if ((new_device_reg_state = malloc(sizeof(DEVICE_REGISTRATION_STATE))) == NULL)
    {
        LogError("Allocation of Device Registration State failed");
    }
    else
    {
        memset(new_device_reg_state, 0, sizeof(DEVICE_REGISTRATION_STATE));

        if (copy_json_string_field(&(new_device_reg_state->registration_id), root_object, DEVICE_REGISTRATION_STATE_JSON_KEY_REG_ID) != 0)
        {
            LogError("Failed to set '%s' in Device Registration State", DEVICE_REGISTRATION_STATE_JSON_KEY_REG_ID);
            deviceRegistrationState_destroy(new_device_reg_state);
            new_device_reg_state = NULL;
        }
        else if (copy_json_string_field(&(new_device_reg_state->created_date_time_utc), root_object, DEVICE_REGISTRATION_STATE_JSON_KEY_CREATED_TIME) != 0)
        {
            LogError("Failed to set '%s' in Device Registration State", DEVICE_REGISTRATION_STATE_JSON_KEY_CREATED_TIME);
            deviceRegistrationState_destroy(new_device_reg_state);
            new_device_reg_state = NULL;
        }
        else if (copy_json_string_field(&(new_device_reg_state->device_id), root_object, DEVICE_REGISTRATION_STATE_JSON_KEY_DEVICE_ID) != 0)
        {
            LogError("Failed to set '%s' in Device Registration State", DEVICE_REGISTRATION_STATE_JSON_KEY_DEVICE_ID);
            deviceRegistrationState_destroy(new_device_reg_state);
            new_device_reg_state = NULL;
        }
        else if ((new_device_reg_state->status = registrationStatus_fromJson(json_object_get_string(root_object, DEVICE_REGISTRATION_STATE_JSON_KEY_REG_STATUS))) == REGISTRATION_STATUS_ERROR)
        {
            LogError("Failed to set '%s' in Device Registration State", DEVICE_REGISTRATION_STATE_JSON_KEY_REG_STATUS);
            deviceRegistrationState_destroy(new_device_reg_state);
            new_device_reg_state = NULL;
        }
        else if (copy_json_string_field(&(new_device_reg_state->updated_date_time_utc), root_object, DEVICE_REGISTRATION_STATE_JSON_KEY_UPDATED_TIME) != 0)
        {
            LogError("Failed to set '%s' in Device Registration State", DEVICE_REGISTRATION_STATE_JSON_KEY_UPDATED_TIME);
            deviceRegistrationState_destroy(new_device_reg_state);
            new_device_reg_state = NULL;
        }
        else if (copy_json_string_field(&(new_device_reg_state->error_message), root_object, DEVICE_REGISTRATION_STATE_JSON_KEY_ERROR_MSG) != 0)
        {
            LogError("Failed to set '%s' in Device Registration State", DEVICE_REGISTRATION_STATE_JSON_KEY_ERROR_MSG);
            deviceRegistrationState_destroy(new_device_reg_state);
            new_device_reg_state = NULL;
        }
        else if (copy_json_string_field(&(new_device_reg_state->etag), root_object, DEVICE_REGISTRATION_STATE_JSON_KEY_ETAG) != 0)
        {
            LogError("Failed to set '%s' in Device Registration State", DEVICE_REGISTRATION_STATE_JSON_KEY_ETAG);
            deviceRegistrationState_destroy(new_device_reg_state);
            new_device_reg_state = NULL;
        }
        else
        {
            new_device_reg_state->error_code = (int)json_object_get_number(root_object, DEVICE_REGISTRATION_STATE_JSON_KEY_ERROR_CODE);
        }
    }

    return new_device_reg_state;
}


DEVICE_REGISTRATION_STATE_HANDLE deviceRegistrationState_deserializeFromJson(const char* json_string)
{
    DEVICE_REGISTRATION_STATE_HANDLE new_drs = NULL;
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;

    if (json_string == NULL)
    {
        LogError("Cannot deserialize NULL");
    }
    else if ((root_value = json_parse_string(json_string)) == NULL)
    {
        LogError("Parsing JSON string failed");
    }
    else if ((root_object = json_value_get_object(root_value)) == NULL)
    {
        LogError("Creating JSON object failed");
    }
    else
    {
        if ((new_drs = deviceRegistrationState_fromJson(root_object)) == NULL)
        {
            LogError("Creating new Device Registration State failed");
        }
        json_value_free(root_value); //implicitly frees root_object
        root_value = NULL;
    }

    return new_drs;
}


/* Acessor Functions */
const char* deviceRegistrationState_getRegistrationId(DEVICE_REGISTRATION_STATE_HANDLE drs)
{
    char* result = NULL;

    if (drs == NULL)
    {
        LogError("device registration state is NULL");
    }
    else
    {
        result = drs->registration_id;
    }

    return result;
}

const char* deviceRegistrationState_getCreatedDateTime(DEVICE_REGISTRATION_STATE_HANDLE drs)
{
    char* result = NULL;

    if (drs == NULL)
    {
        LogError("device registration state is NULL");
    }
    else
    {
        result = drs->created_date_time_utc;
    }

    return result;
}

const char* deviceRegistrationState_getDeviceId(DEVICE_REGISTRATION_STATE_HANDLE drs)
{
    char* result = NULL;

    if (drs == NULL)
    {
        LogError("device registration state is NULL");
    }
    else
    {
        result = drs->device_id;
    }

    return result;
}

REGISTRATION_STATUS deviceRegistrationState_getRegistrationStatus(DEVICE_REGISTRATION_STATE_HANDLE drs)
{
    REGISTRATION_STATUS result = REGISTRATION_STATUS_ERROR;

    if (drs == NULL)
    {
        LogError("device registration state is NULL");
    }
    else
    {
        result = drs->status;
    }

    return result;
}

const char* deviceRegistrationState_getUpdatedDateTime(DEVICE_REGISTRATION_STATE_HANDLE drs)
{
    char* result = NULL;

    if (drs == NULL)
    {
        LogError("device registration state is NULL");
    }
    else
    {
        result = drs->updated_date_time_utc;
    }

    return result;
}

int deviceRegistrationState_getErrorCode(DEVICE_REGISTRATION_STATE_HANDLE drs)
{
    int result = -1;

    if (drs == NULL)
    {
        LogError("device registration state is NULL");
    }
    else
    {
        result = drs->error_code;
    }

    return result;
}

const char* deviceRegistrationState_getErrorMessage(DEVICE_REGISTRATION_STATE_HANDLE drs)
{
    char* result = NULL;

    if (drs == NULL)
    {
        LogError("device registration state is NULL");
    }
    else
    {
        result = drs->error_message;
    }

    return result;
}

const char* deviceRegistrationState_getEtag(DEVICE_REGISTRATION_STATE_HANDLE drs)
{
    char* result = NULL;

    if (drs == NULL)
    {
        LogError("device registration state is NULL");
    }
    else
    {
        result = drs->etag;
    }

    return result;
}
