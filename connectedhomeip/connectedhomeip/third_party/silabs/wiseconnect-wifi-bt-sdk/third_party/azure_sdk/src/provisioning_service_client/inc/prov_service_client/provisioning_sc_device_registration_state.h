// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef PROVISIONING_SC_DEVICE_REGISTRATION_STATE_H
#define PROVISIONING_SC_DEVICE_REGISTRATION_STATE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "umock_c/umock_c_prod.h"
#include "azure_macro_utils/macro_utils.h"
#include "parson.h"

typedef struct DEVICE_REGISTRATION_STATE_TAG* DEVICE_REGISTRATION_STATE_HANDLE;

#define REGISTRATION_STATUS_VALUES \
        REGISTRATION_STATUS_ERROR, \
        REGISTRATION_STATUS_UNASSIGNED, \
        REGISTRATION_STATUS_ASSIGNING, \
        REGISTRATION_STATUS_ASSIGNED, \
        REGISTRATION_STATUS_FAILED, \
        REGISTRATION_STATUS_DISABLED \

//Note: REGISTRATION_STATUS_ERROR is invalid, indicating error
MU_DEFINE_ENUM_WITHOUT_INVALID(REGISTRATION_STATUS, REGISTRATION_STATUS_VALUES);

/* Accessor Functions */
MOCKABLE_FUNCTION(, const char*, deviceRegistrationState_getRegistrationId, DEVICE_REGISTRATION_STATE_HANDLE, drs);
MOCKABLE_FUNCTION(, const char*, deviceRegistrationState_getCreatedDateTime, DEVICE_REGISTRATION_STATE_HANDLE, drs);
MOCKABLE_FUNCTION(, const char*, deviceRegistrationState_getDeviceId, DEVICE_REGISTRATION_STATE_HANDLE, drs);
MOCKABLE_FUNCTION(, REGISTRATION_STATUS, deviceRegistrationState_getRegistrationStatus, DEVICE_REGISTRATION_STATE_HANDLE, drs);
MOCKABLE_FUNCTION(, const char*, deviceRegistrationState_getUpdatedDateTime, DEVICE_REGISTRATION_STATE_HANDLE, drs);
MOCKABLE_FUNCTION(, int, deviceRegistrationState_getErrorCode, DEVICE_REGISTRATION_STATE_HANDLE, drs);
MOCKABLE_FUNCTION(, const char*, deviceRegistrationState_getErrorMessage, DEVICE_REGISTRATION_STATE_HANDLE, drs);
MOCKABLE_FUNCTION(, const char*, deviceRegistrationState_getEtag, DEVICE_REGISTRATION_STATE_HANDLE, drs);
MOCKABLE_FUNCTION(, void, deviceRegistrationState_destroy, DEVICE_REGISTRATION_STATE_HANDLE, device_reg_state);


/*---INTERNAL USAGE ONLY---*/
MOCKABLE_FUNCTION(, DEVICE_REGISTRATION_STATE_HANDLE, deviceRegistrationState_fromJson, JSON_Object*, root_object);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PROVISIONING_SC_DEVICE_REGISTRATION_STATE_H */