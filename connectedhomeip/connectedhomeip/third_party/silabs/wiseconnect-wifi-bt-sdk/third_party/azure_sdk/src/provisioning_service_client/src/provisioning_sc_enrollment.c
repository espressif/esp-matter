// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>

#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"

#include "prov_service_client/provisioning_sc_enrollment.h"
#include "prov_service_client/provisioning_sc_attestation_mechanism.h"
#include "prov_service_client/provisioning_sc_device_registration_state.h"
#include "prov_service_client/provisioning_sc_device_capabilities.h"
#include "prov_service_client/provisioning_sc_twin.h"
#include "prov_service_client/provisioning_sc_json_const.h"
#include "prov_service_client/provisioning_sc_shared_helpers.h"
#include "parson.h"

typedef struct INDIVIDUAL_ENROLLMENT_TAG
{
    DEVICE_CAPABILITIES_HANDLE capabilities;
    char* registration_id; //read only
    char* device_id;
    DEVICE_REGISTRATION_STATE_HANDLE registration_state; //read only
    ATTESTATION_MECHANISM_HANDLE attestation_mechanism;
    char * iothub_hostname; //read only
    INITIAL_TWIN_HANDLE initial_twin;
    char* etag;
    PROVISIONING_STATUS provisioning_status;
    char* created_date_time_utc; //read only
    char* updated_date_time_utc; //read only
} INDIVIDUAL_ENROLLMENT;

typedef struct ENROLLMENT_GROUP_TAG
{
    char* group_id; //read only
    ATTESTATION_MECHANISM_HANDLE attestation_mechanism;
    char* iothub_hostname;
    INITIAL_TWIN_HANDLE initial_twin;
    char* etag;
    PROVISIONING_STATUS provisioning_status;
    char* created_date_time_utc; //read only
    char* updated_date_time_utc; //read only
} ENROLLMENT_GROUP;

MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(PROVISIONING_STATUS, PROVISIONING_STATUS_VALUES)

static const char* provisioningStatus_toJson(PROVISIONING_STATUS status)
{
    const char* result = NULL;
    if (status == PROVISIONING_STATUS_ENABLED)
    {
        result = PROVISIONING_STATUS_JSON_VALUE_ENABLED;
    }
    else if (status == PROVISIONING_STATUS_DISABLED)
    {
        result = PROVISIONING_STATUS_JSON_VALUE_DISABLED;
    }
    else
    {
        LogError("Could not convert '%s' to JSON", MU_ENUM_TO_STRING(PROVISIONING_STATUS, status));
    }

    return result;
}

static PROVISIONING_STATUS provisioningStatus_fromJson(const char* str_rep)
{
    PROVISIONING_STATUS new_status = PROVISIONING_STATUS_NONE;

    if (str_rep != NULL)
    {
        if (strcmp(str_rep, PROVISIONING_STATUS_JSON_VALUE_ENABLED) == 0)
        {
            new_status = PROVISIONING_STATUS_ENABLED;
        }
        else if (strcmp(str_rep, PROVISIONING_STATUS_JSON_VALUE_DISABLED) == 0)
        {
            new_status = PROVISIONING_STATUS_DISABLED;
        }
        else
        {
            LogError("Could not convert '%s' from JSON", str_rep);
        }
    }

    return new_status;
}

void individualEnrollment_destroy(INDIVIDUAL_ENROLLMENT_HANDLE enrollment)
{
    if (enrollment != NULL) {
        deviceCapabilities_destroy(enrollment->capabilities);
        free(enrollment->registration_id);
        free(enrollment->device_id);
        free(enrollment->etag);
        free(enrollment->iothub_hostname);
        free(enrollment->created_date_time_utc);
        free(enrollment->updated_date_time_utc);
        attestationMechanism_destroy(enrollment->attestation_mechanism);
        initialTwin_destroy(enrollment->initial_twin);
        deviceRegistrationState_destroy(enrollment->registration_state);
        free(enrollment);
    }
}

JSON_Value* individualEnrollment_toJson(INDIVIDUAL_ENROLLMENT_HANDLE enrollment)
{
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;

    const char* ps_str = NULL;

    //Setup
    if (enrollment == NULL)
    {
        LogError("enrollment is NULL");
    }
    else if ((root_value = json_value_init_object()) == NULL)
    {
        LogError("json_value_init_object failed");
    }
    else if ((root_object = json_value_get_object(root_value)) == NULL)
    {
        LogError("json_value_get_object failed");
        json_value_free(root_value);
        root_value = NULL;
    }

    //Set data
    else if (json_serialize_and_set_struct(root_object, INDIVIDUAL_ENROLLMENT_JSON_KEY_CAPABILITIES, enrollment->capabilities, (TO_JSON_FUNCTION)deviceCapabilities_toJson, false) != 0)
    {
        LogError("Failed to set '%s' in JSON string", INDIVIDUAL_ENROLLMENT_JSON_KEY_CAPABILITIES);
        json_value_free(root_value);
        root_value = NULL;
    }
    else if (json_object_set_string(root_object, INDIVIDUAL_ENROLLMENT_JSON_KEY_REG_ID, enrollment->registration_id) != JSONSuccess)
    {
        LogError("Failed to set '%s' in JSON string", INDIVIDUAL_ENROLLMENT_JSON_KEY_REG_ID);
        json_value_free(root_value);
        root_value = NULL;
    }
    else if ((enrollment->device_id != NULL) && (json_object_set_string(root_object, INDIVIDUAL_ENROLLMENT_JSON_KEY_DEVICE_ID, enrollment->device_id) != JSONSuccess))
    {
        LogError("Failed to set '%s' in JSON String", INDIVIDUAL_ENROLLMENT_JSON_KEY_DEVICE_ID);
        json_value_free(root_value);
        root_value = NULL;
    }
    else if (json_serialize_and_set_struct(root_object, INDIVIDUAL_ENROLLMENT_JSON_KEY_ATTESTATION, enrollment->attestation_mechanism, (TO_JSON_FUNCTION)attestationMechanism_toJson, true) != 0)
    {
        LogError("Failed to set '%s' in JSON String", INDIVIDUAL_ENROLLMENT_JSON_KEY_ATTESTATION);
        json_value_free(root_value);
        root_value = NULL;
    }
    else if (json_serialize_and_set_struct(root_object, INDIVIDUAL_ENROLLMENT_JSON_KEY_INITIAL_TWIN, enrollment->initial_twin, (TO_JSON_FUNCTION)initialTwin_toJson, false) != 0)
    {
        LogError("Failed to set '%s' in JSON String", INDIVIDUAL_ENROLLMENT_JSON_KEY_INITIAL_TWIN);
        json_value_free(root_value);
        root_value = NULL;
    }
    else if ((enrollment->etag != NULL) && (json_object_set_string(root_object, INDIVIDUAL_ENROLLMENT_JSON_KEY_ETAG, enrollment->etag) != JSONSuccess))
    {
        LogError("Failed to set '%s' in JSON String", INDIVIDUAL_ENROLLMENT_JSON_KEY_ETAG);
        json_value_free(root_value);
        root_value = NULL;
    }
    else if (((ps_str = provisioningStatus_toJson(enrollment->provisioning_status)) == NULL) || (json_object_set_string(root_object, INDIVIDUAL_ENROLLMENT_JSON_KEY_PROV_STATUS, ps_str) != JSONSuccess))
    {
        LogError("Failed to set '%s' in JSON String", INDIVIDUAL_ENROLLMENT_JSON_KEY_PROV_STATUS);
        json_value_free(root_value);
        root_value = NULL;
    }
    //Do not set registration_state, create_date_time_utc or update_date_time_utc as they are READ ONLY

    return root_value;
}

INDIVIDUAL_ENROLLMENT_HANDLE individualEnrollment_fromJson(JSON_Object* root_object)
{
    INDIVIDUAL_ENROLLMENT_HANDLE new_enrollment = NULL;

    if (root_object == NULL)
    {
        LogError("No enrollment in JSON");
    }
    else if ((new_enrollment = malloc(sizeof(INDIVIDUAL_ENROLLMENT))) == NULL)
    {
        LogError("Allocation of Individual Enrollment failed");
    }
    else
    {
        memset(new_enrollment, 0, sizeof(INDIVIDUAL_ENROLLMENT));

        if (json_deserialize_and_get_struct((void**)&(new_enrollment->capabilities), root_object, INDIVIDUAL_ENROLLMENT_JSON_KEY_CAPABILITIES, (FROM_JSON_FUNCTION)deviceCapabilities_fromJson, false) != 0)
        {
            LogError("Failed to set '%s' in Individual Enrollment", INDIVIDUAL_ENROLLMENT_JSON_KEY_CAPABILITIES);
            individualEnrollment_destroy(new_enrollment);
            new_enrollment = NULL;
        }
        else if (copy_json_string_field(&(new_enrollment->registration_id), root_object, INDIVIDUAL_ENROLLMENT_JSON_KEY_REG_ID) != 0)
        {
            LogError("Failed to set '%s' in Individual Enrollment", INDIVIDUAL_ENROLLMENT_JSON_KEY_REG_ID);
            individualEnrollment_destroy(new_enrollment);
            new_enrollment = NULL;
        }
        else if (copy_json_string_field(&(new_enrollment->device_id), root_object, INDIVIDUAL_ENROLLMENT_JSON_KEY_DEVICE_ID) != 0)
        {
            LogError("Failed to set '%s' in Individual Enrollment", INDIVIDUAL_ENROLLMENT_JSON_KEY_DEVICE_ID);
            individualEnrollment_destroy(new_enrollment);
            new_enrollment = NULL;
        }
        else if (json_deserialize_and_get_struct((void**)&(new_enrollment->registration_state), root_object, INDIVIDUAL_ENROLLMENT_JSON_KEY_REG_STATE, (FROM_JSON_FUNCTION)deviceRegistrationState_fromJson, false) != 0)
        {
            LogError("Failed to set '%s' in Individual Enrollment", INDIVIDUAL_ENROLLMENT_JSON_KEY_REG_STATE);
            individualEnrollment_destroy(new_enrollment);
            new_enrollment = NULL;
        }
        else if (json_deserialize_and_get_struct((void**)&(new_enrollment->attestation_mechanism), root_object, INDIVIDUAL_ENROLLMENT_JSON_KEY_ATTESTATION, (FROM_JSON_FUNCTION)attestationMechanism_fromJson, true) != 0)
        {
            LogError("Failed to set '%s' in Individual Enrollment", INDIVIDUAL_ENROLLMENT_JSON_KEY_ATTESTATION);
            individualEnrollment_destroy(new_enrollment);
            new_enrollment = NULL;
        }
        else if (copy_json_string_field(&(new_enrollment->iothub_hostname), root_object, INDIVIDUAL_ENROLLMENT_JSON_KEY_IOTHUB_HOSTNAME) != 0)
        {
            LogError("Failed to set '%s' in Individual Enrollment", INDIVIDUAL_ENROLLMENT_JSON_KEY_IOTHUB_HOSTNAME);
            individualEnrollment_destroy(new_enrollment);
            new_enrollment = NULL;
        }
        else if (json_deserialize_and_get_struct((void**)&(new_enrollment->initial_twin), root_object, INDIVIDUAL_ENROLLMENT_JSON_KEY_INITIAL_TWIN, (FROM_JSON_FUNCTION)initialTwin_fromJson, false) != 0)
        {
            LogError("Failed to set '%s' in Individual Enrollment", INDIVIDUAL_ENROLLMENT_JSON_KEY_INITIAL_TWIN);
            individualEnrollment_destroy(new_enrollment);
            new_enrollment = NULL;
        }
        else if (copy_json_string_field(&(new_enrollment->etag), root_object, INDIVIDUAL_ENROLLMENT_JSON_KEY_ETAG) != 0)
        {
            LogError("Failed to set '%s' in Individual Enrollment", INDIVIDUAL_ENROLLMENT_JSON_KEY_ETAG);
            individualEnrollment_destroy(new_enrollment);
            new_enrollment = NULL;
        }
        else if ((new_enrollment->provisioning_status = provisioningStatus_fromJson(json_object_get_string(root_object, INDIVIDUAL_ENROLLMENT_JSON_KEY_PROV_STATUS))) == PROVISIONING_STATUS_NONE)
        {
            LogError("Failed to set '%s' in Individual Enrollment", INDIVIDUAL_ENROLLMENT_JSON_KEY_PROV_STATUS);
            individualEnrollment_destroy(new_enrollment);
            new_enrollment = NULL;
        }
        else if (copy_json_string_field(&(new_enrollment->created_date_time_utc), root_object, INDIVIDUAL_ENROLLMENT_JSON_KEY_CREATED_TIME) != 0)
        {
            LogError("Failed to set '%s' in Individual Enrollment", INDIVIDUAL_ENROLLMENT_JSON_KEY_CREATED_TIME);
            individualEnrollment_destroy(new_enrollment);
            new_enrollment = NULL;
        }
        else if (copy_json_string_field(&(new_enrollment->updated_date_time_utc), root_object, INDIVIDUAL_ENROLLMENT_JSON_KEY_UPDATED_TIME) != 0)
        {
            LogError("Failed to set '%s' in Individual Enrollment", INDIVIDUAL_ENROLLMENT_JSON_KEY_UPDATED_TIME);
            individualEnrollment_destroy(new_enrollment);
            new_enrollment = NULL;
        }
    }

    return new_enrollment;
}

void enrollmentGroup_destroy(ENROLLMENT_GROUP_HANDLE enrollment)
{
    if (enrollment != NULL)
    {
        free(enrollment->group_id);
        attestationMechanism_destroy(enrollment->attestation_mechanism);
        initialTwin_destroy(enrollment->initial_twin);
        free(enrollment->etag);
        free(enrollment->iothub_hostname);
        free(enrollment->created_date_time_utc);
        free(enrollment->updated_date_time_utc);
        free(enrollment);
    }
}

static JSON_Value* enrollmentGroup_toJson(const ENROLLMENT_GROUP_HANDLE enrollment)
{
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;

    //Setup
    if (enrollment == NULL)
    {
        LogError("enrollment is NULL");
    }
    else if ((root_value = json_value_init_object()) == NULL)
    {
        LogError("json_value_init_object failed");
    }
    else if ((root_object = json_value_get_object(root_value)) == NULL)
    {
        LogError("json_value_get_object failed");
        json_value_free(root_value);
        root_value = NULL;
    }

    //Set data
    else if (json_object_set_string(root_object, ENROLLMENT_GROUP_JSON_KEY_GROUP_ID, enrollment->group_id) != JSONSuccess)
    {
        LogError("Failed to set '%s' in JSON string", ENROLLMENT_GROUP_JSON_KEY_GROUP_ID);
        json_value_free(root_value);
        root_value = NULL;
    }
    else if (json_serialize_and_set_struct(root_object, ENROLLMENT_GROUP_JSON_KEY_ATTESTATION, enrollment->attestation_mechanism, (TO_JSON_FUNCTION)attestationMechanism_toJson, true) != 0)
    {
        LogError("Failed to set '%s' in JSON string", ENROLLMENT_GROUP_JSON_KEY_ATTESTATION);
        json_value_free(root_value);
        root_value = NULL;
    }
    else if (json_serialize_and_set_struct(root_object, ENROLLMENT_GROUP_JSON_KEY_INITIAL_TWIN, enrollment->initial_twin, (TO_JSON_FUNCTION)initialTwin_toJson, false) != 0)
    {
        LogError("Failed to set '%s' in JSON string", ENROLLMENT_GROUP_JSON_KEY_INITIAL_TWIN);
        json_value_free(root_value);
        root_value = NULL;
    }
    else if ((enrollment->etag != NULL) && (json_object_set_string(root_object, ENROLLMENT_GROUP_JSON_KEY_ETAG, enrollment->etag) != JSONSuccess))
    {
        LogError("Failed to set '%s' in JSON string", ENROLLMENT_GROUP_JSON_KEY_ETAG);
        json_value_free(root_value);
        root_value = NULL;
    }
    else if (json_object_set_string(root_object, ENROLLMENT_GROUP_JSON_KEY_PROV_STATUS, provisioningStatus_toJson(enrollment->provisioning_status)) != JSONSuccess)
    {
        LogError("Failed to set '%s' in JSON string", ENROLLMENT_GROUP_JSON_KEY_PROV_STATUS);
        json_value_free(root_value);
        root_value = NULL;
    }

    return root_value;
}

ENROLLMENT_GROUP_HANDLE enrollmentGroup_fromJson(JSON_Object* root_object)
{
    ENROLLMENT_GROUP_HANDLE new_enrollment = NULL;

    if ((new_enrollment = malloc(sizeof(ENROLLMENT_GROUP))) == NULL)
    {
        LogError("Allocation of Enrollment Group failed");
    }
    else
    {
        memset(new_enrollment, 0, sizeof(ENROLLMENT_GROUP));

        if (copy_json_string_field(&(new_enrollment->group_id), root_object, ENROLLMENT_GROUP_JSON_KEY_GROUP_ID) != 0)
        {
            LogError("Failed to set '%s' in Enrollment Group", ENROLLMENT_GROUP_JSON_KEY_GROUP_ID);
            enrollmentGroup_destroy(new_enrollment);
            new_enrollment = NULL;
        }
        else if (json_deserialize_and_get_struct((void**)&(new_enrollment->attestation_mechanism), root_object, ENROLLMENT_GROUP_JSON_KEY_ATTESTATION, (FROM_JSON_FUNCTION)attestationMechanism_fromJson, true) != 0)
        {
            LogError("Failed to set '%s' in Enrollment Group", ENROLLMENT_GROUP_JSON_KEY_ATTESTATION);
            enrollmentGroup_destroy(new_enrollment);
            new_enrollment = NULL;
        }
        else if (copy_json_string_field(&(new_enrollment->iothub_hostname), root_object, ENROLLMENT_GROUP_JSON_KEY_IOTHUB_HOSTNAME) != 0)
        {
            LogError("Failed to set '%s' in Enrollment Group", ENROLLMENT_GROUP_JSON_KEY_IOTHUB_HOSTNAME);
            enrollmentGroup_destroy(new_enrollment);
            new_enrollment = NULL;
        }
        else if (json_deserialize_and_get_struct((void**)&(new_enrollment->initial_twin), root_object, ENROLLMENT_GROUP_JSON_KEY_INITIAL_TWIN, (FROM_JSON_FUNCTION)initialTwin_fromJson, false) != 0)
        {
            LogError("Failed to set '%s' in Enrollment Group", ENROLLMENT_GROUP_JSON_KEY_INITIAL_TWIN);
            enrollmentGroup_destroy(new_enrollment);
            new_enrollment = NULL;
        }
        else if (copy_json_string_field(&(new_enrollment->etag), root_object, ENROLLMENT_GROUP_JSON_KEY_ETAG) != 0)
        {
            LogError("Failed to set '%s' in Enrollment Group", ENROLLMENT_GROUP_JSON_KEY_ETAG);
            enrollmentGroup_destroy(new_enrollment);
            new_enrollment = NULL;
        }
        else if ((new_enrollment->provisioning_status = provisioningStatus_fromJson(json_object_get_string(root_object, ENROLLMENT_GROUP_JSON_KEY_PROV_STATUS))) == PROVISIONING_STATUS_NONE)
        {
            LogError("Failed to set '%s' in Enrollment Group", ENROLLMENT_GROUP_JSON_KEY_PROV_STATUS);
            enrollmentGroup_destroy(new_enrollment);
            new_enrollment = NULL;
        }
        else if (copy_json_string_field(&(new_enrollment->created_date_time_utc), root_object, ENROLLMENT_GROUP_JSON_KEY_CREATED_TIME) != 0)
        {
            LogError("Failed to set '%s' in Enrollment Group", ENROLLMENT_GROUP_JSON_KEY_CREATED_TIME);
            enrollmentGroup_destroy(new_enrollment);
            new_enrollment = NULL;
        }
        else if (copy_json_string_field(&(new_enrollment->updated_date_time_utc), root_object, ENROLLMENT_GROUP_JSON_KEY_UPDATED_TIME) != 0)
        {
            LogError("Failed to set '%s' in Enrollment Group", ENROLLMENT_GROUP_JSON_KEY_UPDATED_TIME);
            enrollmentGroup_destroy(new_enrollment);
            new_enrollment = NULL;
        }
    }

    return new_enrollment;
}

INDIVIDUAL_ENROLLMENT_HANDLE individualEnrollment_create(const char* reg_id, ATTESTATION_MECHANISM_HANDLE att_mech)
{
    INDIVIDUAL_ENROLLMENT_HANDLE new_enrollment = NULL;

    if (reg_id == NULL)
    {
        LogError("reg_id invalid");
    }
    else if (!attestationMechanism_isValidForIndividualEnrollment(att_mech))
    {
        LogError("attestation mechanism is invalid for Individual Enrollment");
    }
    else if ((new_enrollment = malloc(sizeof(INDIVIDUAL_ENROLLMENT))) == NULL)
    {
        LogError("Allocation of individual enrollment failed");
    }
    else
    {
        memset(new_enrollment, 0, sizeof(INDIVIDUAL_ENROLLMENT));

        if ((new_enrollment->capabilities = deviceCapabilities_create()) == NULL)
        {
            LogError("Creating device capabilities failed");
            individualEnrollment_destroy(new_enrollment);
            new_enrollment = NULL;
        }
        else if (mallocAndStrcpy_s(&(new_enrollment->registration_id), reg_id) != 0)
        {
            LogError("Allocation of registration id failed");
            deviceCapabilities_destroy(new_enrollment->capabilities);
            individualEnrollment_destroy(new_enrollment);
            new_enrollment = NULL;
        }
        else
        {
            new_enrollment->attestation_mechanism = att_mech;
            new_enrollment->provisioning_status = PROVISIONING_STATUS_ENABLED;
        }
    }

    return new_enrollment;
}

char* individualEnrollment_serializeToJson(const INDIVIDUAL_ENROLLMENT_HANDLE enrollment)
{
    char* result = NULL;
    char* serialized_string = NULL;
    JSON_Value* root_value = NULL;

    if (enrollment == NULL)
    {
        LogError("Cannot serialize NULL");
    }
    else if ((root_value = individualEnrollment_toJson(enrollment)) == NULL)
    {
        LogError("Creating json object failed");
    }
    else if ((serialized_string = json_serialize_to_string(root_value)) == NULL)
    {
        LogError("Failed to serialize to JSON");
    }
    else if (mallocAndStrcpy_s(&result, serialized_string) != 0)
    {
        LogError("Failed to copy serialized string");
    }

    if (root_value != NULL)
    {
        json_value_free(root_value);
        root_value = NULL;
    }
    if (serialized_string != NULL)
    {
        json_free_serialized_string(serialized_string);
        serialized_string = NULL;
    }

    return result;
}

INDIVIDUAL_ENROLLMENT_HANDLE individualEnrollment_deserializeFromJson(const char* json_string)
{
    INDIVIDUAL_ENROLLMENT_HANDLE new_enrollment = NULL;
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
        if ((new_enrollment = individualEnrollment_fromJson(root_object)) == NULL)
        {
            LogError("Creating new Individual Enrollment failed");
        }
        json_value_free(root_value); //implicitly frees root_object
        root_value = NULL;
    }

    return new_enrollment;
}

ENROLLMENT_GROUP_HANDLE enrollmentGroup_create(const char* group_id, ATTESTATION_MECHANISM_HANDLE att_mech)
{
    ENROLLMENT_GROUP_HANDLE new_enrollment = NULL;

    if (group_id == NULL)
    {
        LogError("group id is NULL");
    }
    else if (!attestationMechanism_isValidForEnrollmentGroup(att_mech))
    {
        LogError("attestation mechanism is invalid for Enrollment Group");
    }
    else if ((new_enrollment = malloc(sizeof(ENROLLMENT_GROUP))) == NULL)
    {
        LogError("Allocation of enrollment group failed");
    }
    else
    {
        memset(new_enrollment, 0, sizeof(ENROLLMENT_GROUP));

        if (mallocAndStrcpy_s(&(new_enrollment->group_id), group_id) != 0)
        {
            LogError("Allocation of group id failed");
            enrollmentGroup_destroy(new_enrollment);
            new_enrollment = NULL;
        }
        else
        {
            new_enrollment->attestation_mechanism = att_mech;
            new_enrollment->provisioning_status = PROVISIONING_STATUS_ENABLED;
        }
    }

    return new_enrollment;
}

char* enrollmentGroup_serializeToJson(ENROLLMENT_GROUP_HANDLE enrollment)
{
    char* result = NULL;
    char* serialized_string = NULL;
    JSON_Value* root_value = NULL;

    if (enrollment == NULL)
    {
        LogError("Cannot serialize NULL");
    }
    else if ((root_value = enrollmentGroup_toJson(enrollment)) == NULL)
    {
        LogError("Creating json object failed");
    }
    else if ((serialized_string = json_serialize_to_string(root_value)) == NULL)
    {
        LogError("Serializing to JSON failed");
    }
    else if (mallocAndStrcpy_s(&result, serialized_string) != 0)
    {
        LogError("Failed to copy serialized string");
    }

    if (root_value != NULL)
    {
        json_value_free(root_value);
        root_value = NULL;
    }

    if (serialized_string != NULL)
    {
        json_free_serialized_string(serialized_string);
        serialized_string = NULL;
    }

    return result;
}

ENROLLMENT_GROUP_HANDLE enrollmentGroup_deserializeFromJson(const char* json_string)
{
    ENROLLMENT_GROUP_HANDLE new_enrollment = NULL;
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;

    if (json_string == NULL)
    {
        LogError("Cannot deserialize NULL");
    }
    else if ((root_value = json_parse_string(json_string)) == NULL)
    {
        LogError("Parsong JSON string failed");
    }
    else if ((root_object = json_value_get_object(root_value)) == NULL)
    {
        LogError("Creating JSON object failed");
    }
    else
    {
        if ((new_enrollment = enrollmentGroup_fromJson(root_object)) == NULL)
        {
            LogError("Creating new Enrollment Group failed");
        }
        json_value_free(root_value); //implicitly frees root_object
        root_value = NULL;
    }

    return new_enrollment;
}

/*Accessor Functions - Individual Enrollment*/
ATTESTATION_MECHANISM_HANDLE individualEnrollment_getAttestationMechanism(INDIVIDUAL_ENROLLMENT_HANDLE enrollment)
{
    ATTESTATION_MECHANISM_HANDLE result = NULL;

    if (enrollment == NULL)
    {
        LogError("enrollment is NULL");
    }
    else
    {
        result = enrollment->attestation_mechanism;
    }

    return result;
}

int individualEnrollment_setAttestationMechanism(INDIVIDUAL_ENROLLMENT_HANDLE enrollment, ATTESTATION_MECHANISM_HANDLE att_mech)
{
    int result = 0;

    if (enrollment == NULL)
    {
        LogError("enrollment handle is NULL");
        result = MU_FAILURE;
    }
    else if (!attestationMechanism_isValidForIndividualEnrollment(att_mech))
    {
        LogError("Invalid attestation mechanism for Individual Enrollment");
        result = MU_FAILURE;
    }
    else
    {
        attestationMechanism_destroy(enrollment->attestation_mechanism);
        enrollment->attestation_mechanism = att_mech;
    }

    return result;
}

INITIAL_TWIN_HANDLE individualEnrollment_getInitialTwin(INDIVIDUAL_ENROLLMENT_HANDLE enrollment)
{
    INITIAL_TWIN_HANDLE result = NULL;

    if (enrollment == NULL)
    {
        LogError("enrollment is NULL");
    }
    else
    {
        result = enrollment->initial_twin;
    }

    return result;
}

int individualEnrollment_setInitialTwin(INDIVIDUAL_ENROLLMENT_HANDLE enrollment, INITIAL_TWIN_HANDLE twin)
{
    int result = 0;

    if (enrollment == NULL)
    {
        LogError("enrollment handle is NULL");
        result = MU_FAILURE;
    }
    else
    {
        initialTwin_destroy(enrollment->initial_twin);
        enrollment->initial_twin = twin;
    }

    return result;
}

DEVICE_REGISTRATION_STATE_HANDLE individualEnrollment_getDeviceRegistrationState(INDIVIDUAL_ENROLLMENT_HANDLE enrollment)
{
    DEVICE_REGISTRATION_STATE_HANDLE result = NULL;

    if (enrollment == NULL)
    {
        LogError("enrollment is NULL");
    }
    else
    {
        result = enrollment->registration_state;
    }

    return result;
}

int individualEnrollment_setDeviceCapabilities(INDIVIDUAL_ENROLLMENT_HANDLE enrollment, DEVICE_CAPABILITIES_HANDLE capabilities)
{
    int result = 0;

    if (enrollment == NULL)
    {
        LogError("enrollment handle is NULL");
        result = MU_FAILURE;
    }
    else
    {
        deviceCapabilities_destroy(enrollment->capabilities);
        enrollment->capabilities = capabilities;
    }
    return result;
}

DEVICE_CAPABILITIES_HANDLE individualEnrollment_getDeviceCapabilities(INDIVIDUAL_ENROLLMENT_HANDLE enrollment)
{
    DEVICE_CAPABILITIES_HANDLE result = NULL;

    if (enrollment == NULL)
    {
        LogError("enrollment is NULL");
    }
    else
    {
        result = enrollment->capabilities;
    }

    return result;
}

const char* individualEnrollment_getRegistrationId(INDIVIDUAL_ENROLLMENT_HANDLE enrollment)
{
    char* result = NULL;

    if (enrollment == NULL)
    {
        LogError("enrollment is NULL");
    }
    else
    {
        result = enrollment->registration_id;
    }

    return result;
}

const char* individualEnrollment_getDeviceId(INDIVIDUAL_ENROLLMENT_HANDLE enrollment)
{
    char* result = NULL;

    if (enrollment == NULL)
    {
        LogError("enrollment is NULL");
    }
    else
    {
        result = enrollment->device_id;
    }

    return result;
}

int individualEnrollment_setDeviceId(INDIVIDUAL_ENROLLMENT_HANDLE enrollment, const char* device_id)
{
    int result = 0;

    if (enrollment == NULL)
    {
        LogError("handle is NULL");
        result = MU_FAILURE;
    }
    else if (device_id == NULL)
    {
        free(enrollment->device_id);
        enrollment->device_id = NULL;
    }
    else if (mallocAndStrcpy_overwrite(&(enrollment->device_id), device_id) != 0)
    {
        LogError("Failed to set device id");
        result = MU_FAILURE;
    }

    return result;
}

const char* individualEnrollment_getIotHubHostName(INDIVIDUAL_ENROLLMENT_HANDLE enrollment)
{
    char* result = NULL;

    if (enrollment == NULL)
    {
        LogError("enrollment is NULL");
    }
    else
    {
        result = enrollment->iothub_hostname;
    }

    return result;
}

const char* individualEnrollment_getEtag(INDIVIDUAL_ENROLLMENT_HANDLE enrollment)
{
    char* result = NULL;

    if (enrollment == NULL)
    {
        LogError("enrollment is NULL");
    }
    else
    {
        result = enrollment->etag;
    }

    return result;
}

int individualEnrollment_setEtag(INDIVIDUAL_ENROLLMENT_HANDLE enrollment, const char* etag)
{
    int result = 0;

    if (enrollment == NULL)
    {
        LogError("Invalid handle");
        result = MU_FAILURE;
    }
    else if (etag == NULL)
    {
        free(enrollment->etag);
        enrollment->etag = NULL;
    }
    else if (mallocAndStrcpy_overwrite(&(enrollment->etag), etag) != 0)
    {
        LogError("Failed to set etag");
        result = MU_FAILURE;
    }

    return result;
}

PROVISIONING_STATUS individualEnrollment_getProvisioningStatus(INDIVIDUAL_ENROLLMENT_HANDLE enrollment)
{
    PROVISIONING_STATUS result = PROVISIONING_STATUS_NONE;

    if (enrollment == NULL)
    {
        LogError("enrollment is NULL");
    }
    else
    {
        result = enrollment->provisioning_status;
    }

    return result;
}

int individualEnrollment_setProvisioningStatus(INDIVIDUAL_ENROLLMENT_HANDLE enrollment, PROVISIONING_STATUS prov_status)
{
    int result = 0;

    if (enrollment == NULL)
    {
        LogError("Invalid handle");
        result = MU_FAILURE;
    }
    else if (prov_status == PROVISIONING_STATUS_NONE)
    {
        LogError("Invalid provisioning status");
        result = MU_FAILURE;
    }
    else
    {
        enrollment->provisioning_status = prov_status;
    }

    return result;
}

const char* individualEnrollment_getCreatedDateTime(INDIVIDUAL_ENROLLMENT_HANDLE enrollment)
{
    char* result = NULL;

    if (enrollment == NULL)
    {
        LogError("enrollment is NULL");
    }
    else
    {
        result = enrollment->created_date_time_utc;
    }

    return result;
}

const char* individualEnrollment_getUpdatedDateTime(INDIVIDUAL_ENROLLMENT_HANDLE enrollment)
{
    char* result = NULL;

    if (enrollment == NULL)
    {
        LogError("enrollment is NULL");
    }
    else
    {
        result = enrollment->updated_date_time_utc;
    }

    return result;
}

/*Accessor Functions - Enrollment Group*/
ATTESTATION_MECHANISM_HANDLE enrollmentGroup_getAttestationMechanism(ENROLLMENT_GROUP_HANDLE enrollment)
{
    ATTESTATION_MECHANISM_HANDLE result = NULL;

    if (enrollment == NULL)
    {
        LogError("enrollment is NULL");
    }
    else
    {
        result = enrollment->attestation_mechanism;
    }

    return result;
}

int enrollmentGroup_setAttestationMechanism(ENROLLMENT_GROUP_HANDLE enrollment, ATTESTATION_MECHANISM_HANDLE att_mech)
{
    int result = 0;

    if (enrollment == NULL)
    {
        LogError("enrollment handle is NULL");
        result = MU_FAILURE;
    }
    else if (!attestationMechanism_isValidForEnrollmentGroup(att_mech))
    {
        LogError("Attestation Mechanism is invalid for Enrollment Group");
        result = MU_FAILURE;
    }
    else
    {
        attestationMechanism_destroy(enrollment->attestation_mechanism);
        enrollment->attestation_mechanism = att_mech;
    }

    return result;
}

INITIAL_TWIN_HANDLE enrollmentGroup_getInitialTwin(ENROLLMENT_GROUP_HANDLE enrollment)
{
    INITIAL_TWIN_HANDLE result = NULL;

    if (enrollment == NULL)
    {
        LogError("enrollment is NULL");
    }
    else
    {
        result = enrollment->initial_twin;
    }

    return result;
}

int enrollmentGroup_setInitialTwin(ENROLLMENT_GROUP_HANDLE enrollment, INITIAL_TWIN_HANDLE twin)
{
    int result = 0;

    if (enrollment == NULL)
    {
        LogError("enrollment handle is NULL");
        result = MU_FAILURE;
    }
    else
    {
        initialTwin_destroy(enrollment->initial_twin);
        enrollment->initial_twin = twin;
    }

    return result;
}

const char* enrollmentGroup_getGroupId(ENROLLMENT_GROUP_HANDLE enrollment)
{
    char* result = NULL;

    if (enrollment == NULL)
    {
        LogError("enrollment is NULL");
    }
    else
    {
        result = enrollment->group_id;
    }

    return result;
}

const char* enrollmentGroup_getIotHubHostName(ENROLLMENT_GROUP_HANDLE enrollment)
{
    char* result = NULL;

    if (enrollment == NULL)
    {
        LogError("enrollment is NULL");
    }
    else
    {
        result = enrollment->iothub_hostname;
    }

    return result;
}

const char* enrollmentGroup_getEtag(ENROLLMENT_GROUP_HANDLE enrollment)
{
    char* result = NULL;

    if (enrollment == NULL)
    {
        LogError("enrollment is NULL");
    }
    else
    {
        result = enrollment->etag;
    }

    return result;
}

int enrollmentGroup_setEtag(ENROLLMENT_GROUP_HANDLE enrollment, const char* etag)
{
    int result = 0;

    if (enrollment == NULL)
    {
        LogError("Invalid handle");
        result = MU_FAILURE;
    }
    else if (etag == NULL)
    {
        free(enrollment->etag);
        enrollment->etag = NULL;
    }
    else if (mallocAndStrcpy_overwrite(&(enrollment->etag), etag) != 0)
    {
        LogError("Failed to set etag");
        result = MU_FAILURE;
    }

    return result;
}

PROVISIONING_STATUS enrollmentGroup_getProvisioningStatus(ENROLLMENT_GROUP_HANDLE enrollment)
{
    PROVISIONING_STATUS result = PROVISIONING_STATUS_NONE;

    if (enrollment == NULL)
    {
        LogError("enrollment is NULL");
    }
    else
    {
        result = enrollment->provisioning_status;
    }

    return result;
}

int enrollmentGroup_setProvisioningStatus(ENROLLMENT_GROUP_HANDLE enrollment, PROVISIONING_STATUS prov_status)
{
    int result = 0;

    if (enrollment == NULL)
    {
        LogError("Invalid handle");
        result = MU_FAILURE;
    }
    else if (prov_status == PROVISIONING_STATUS_NONE)
    {
        LogError("Invalid provisioning status");
        result = MU_FAILURE;
    }
    else
    {
        enrollment->provisioning_status = prov_status;
    }

    return result;
}

const char* enrollmentGroup_getCreatedDateTime(ENROLLMENT_GROUP_HANDLE enrollment)
{
    char* result = NULL;

    if (enrollment == NULL)
    {
        LogError("enrollment is NULL");
    }
    else
    {
        result = enrollment->created_date_time_utc;
    }

    return result;
}

const char* enrollmentGroup_getUpdatedDateTime(ENROLLMENT_GROUP_HANDLE enrollment)
{
    char* result = NULL;

    if (enrollment == NULL)
    {
        LogError("enrollment is NULL");
    }
    else
    {
        result = enrollment->updated_date_time_utc;
    }

    return result;
}
