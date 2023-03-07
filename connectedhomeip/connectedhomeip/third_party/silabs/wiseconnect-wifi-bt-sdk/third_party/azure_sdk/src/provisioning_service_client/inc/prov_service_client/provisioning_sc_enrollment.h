// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef PROVISIONING_SC_ENROLLMENT_H
#define PROVISIONING_SC_ENROLLMENT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "azure_c_shared_utility/agenttime.h"
#include "azure_macro_utils/macro_utils.h"
#include "umock_c/umock_c_prod.h"
#include "provisioning_sc_attestation_mechanism.h"
#include "provisioning_sc_device_registration_state.h"
#include "provisioning_sc_device_capabilities.h"
#include "provisioning_sc_twin.h"
#include "parson.h"

/** @brief  Handles to hide structs and use them in consequent APIs
*/
typedef struct INDIVIDUAL_ENROLLMENT_TAG* INDIVIDUAL_ENROLLMENT_HANDLE;
typedef struct ENROLLMENT_GROUP_TAG* ENROLLMENT_GROUP_HANDLE;

#define PROVISIONING_STATUS_VALUES \
        PROVISIONING_STATUS_NONE, \
        PROVISIONING_STATUS_ENABLED, \
        PROVISIONING_STATUS_DISABLED \

//Note: PROVISIONING_STATUS_NONE is invalid, indicating error
MU_DEFINE_ENUM_WITHOUT_INVALID(PROVISIONING_STATUS, PROVISIONING_STATUS_VALUES);


/* OPERATION FUNCTIONS
*
* Use these functions to create and destroy handles
*
* PLEASE NOTE: Attempting to free any of the returned handles manually will result in unexpected behaviour and memory leakage.
* Please ONLY use given "destroy" functions to free handles when you are done with them.
*/


/** @brief  Creates an Individual Enrollment handle with a TPM Attestation for use in consequent APIs.
*
* @param    reg_id              A registration id for the Individual Enrollment.
* @param    att_mech            The handle for the Attestation Mechanism to be used by the Individual Enrollment
*
* @return   A non-NULL handle representing an Individual Enrollment for use with the Provisioning Service, and NULL on failure.
*/
MOCKABLE_FUNCTION(, INDIVIDUAL_ENROLLMENT_HANDLE, individualEnrollment_create, const char*, reg_id, ATTESTATION_MECHANISM_HANDLE, att_mech);

/** @brief  Destroys an Individual Enrollment handle, freeing all associated memory. Please note that this also includes the attestation mechanism
*           that was given in the constructor.
*
* @param    enrollment      A handle for the Individual Enrollment to be destroyed.
*/
MOCKABLE_FUNCTION(, void, individualEnrollment_destroy, INDIVIDUAL_ENROLLMENT_HANDLE, enrollment);

/** @brief  Creates an Enrollment Group handle with an X509 Attestation for use in consequent APIs.
*
* @param    group_id        A group name for the Enrollment Group.
* @param    att_mech        The handle for the Attestation Mechanism to be used by the Enrollment Group. Note: only valid with type: X509
*
* @return   A non-NULL handle representing an Enrollment Group for use with the Provisioning Service, and NULL on failure.
*/
MOCKABLE_FUNCTION(, ENROLLMENT_GROUP_HANDLE, enrollmentGroup_create, const char*, group_id, ATTESTATION_MECHANISM_HANDLE, att_mech);

/** @brief  Destorys an Enrollment Group handle, freeing all associated memory. Please note that this also includes the attestation mechanism
*           that was given in the constructor.
*
* @param    enrollment      A handle for the Enrollment Group to be destroyed.
*/
MOCKABLE_FUNCTION(, void, enrollmentGroup_destroy, ENROLLMENT_GROUP_HANDLE, enrollment);


/* ACCESSOR FUNCTIONS
*
* Use these to retrieve and access properties of handles.
*
* PLEASE NOTE WELL: If given an invalid handle, "get" functions will return a default value (NULL, 0, etc.).
* However, these values are not indicative of error - a handle may have a valid property with a value of NULL, 0, etc.
* Please ensure that you only pass valid handles to "get" accessor functions to avoid unexpected behaviour.
*
* The "set" accessor functions on the other hand, will return a non-zero integer in the event of failure.
*/

/* Individual Enrollment Accessor Functions */
MOCKABLE_FUNCTION(, ATTESTATION_MECHANISM_HANDLE, individualEnrollment_getAttestationMechanism, INDIVIDUAL_ENROLLMENT_HANDLE, enrollment);
MOCKABLE_FUNCTION(, int, individualEnrollment_setAttestationMechanism, INDIVIDUAL_ENROLLMENT_HANDLE, enrollment, ATTESTATION_MECHANISM_HANDLE, att_mech);
MOCKABLE_FUNCTION(, INITIAL_TWIN_HANDLE, individualEnrollment_getInitialTwin, INDIVIDUAL_ENROLLMENT_HANDLE, enrollment);
MOCKABLE_FUNCTION(, int, individualEnrollment_setInitialTwin, INDIVIDUAL_ENROLLMENT_HANDLE, enrollment, INITIAL_TWIN_HANDLE, twin);
MOCKABLE_FUNCTION(, DEVICE_CAPABILITIES_HANDLE, individualEnrollment_getDeviceCapabilities, INDIVIDUAL_ENROLLMENT_HANDLE, enrollment);
MOCKABLE_FUNCTION(, int, individualEnrollment_setDeviceCapabilities, INDIVIDUAL_ENROLLMENT_HANDLE, enrollment, DEVICE_CAPABILITIES_HANDLE, capabilities);
MOCKABLE_FUNCTION(, DEVICE_REGISTRATION_STATE_HANDLE, individualEnrollment_getDeviceRegistrationState, INDIVIDUAL_ENROLLMENT_HANDLE, enrollment);
MOCKABLE_FUNCTION(, const char*, individualEnrollment_getRegistrationId, INDIVIDUAL_ENROLLMENT_HANDLE, enrollment);
MOCKABLE_FUNCTION(, const char*, individualEnrollment_getIotHubHostName, INDIVIDUAL_ENROLLMENT_HANDLE, enrollment);
MOCKABLE_FUNCTION(, const char*, individualEnrollment_getDeviceId, INDIVIDUAL_ENROLLMENT_HANDLE, enrollment);
MOCKABLE_FUNCTION(, int, individualEnrollment_setDeviceId, INDIVIDUAL_ENROLLMENT_HANDLE, enrollment, const char*, device_id);
MOCKABLE_FUNCTION(, const char*, individualEnrollment_getEtag, INDIVIDUAL_ENROLLMENT_HANDLE, enrollment);
MOCKABLE_FUNCTION(, int, individualEnrollment_setEtag, INDIVIDUAL_ENROLLMENT_HANDLE, enrollment, const char*, etag);
MOCKABLE_FUNCTION(, PROVISIONING_STATUS, individualEnrollment_getProvisioningStatus, INDIVIDUAL_ENROLLMENT_HANDLE, enrollment);
MOCKABLE_FUNCTION(, int, individualEnrollment_setProvisioningStatus, INDIVIDUAL_ENROLLMENT_HANDLE, enrollment, PROVISIONING_STATUS, prov_status);
MOCKABLE_FUNCTION(, const char*, individualEnrollment_getCreatedDateTime, INDIVIDUAL_ENROLLMENT_HANDLE, enrollment);
MOCKABLE_FUNCTION(, const char*, individualEnrollment_getUpdatedDateTime, INDIVIDUAL_ENROLLMENT_HANDLE, enrollment);

/* Enrollment Group Accessor Functions */
MOCKABLE_FUNCTION(, ATTESTATION_MECHANISM_HANDLE, enrollmentGroup_getAttestationMechanism, ENROLLMENT_GROUP_HANDLE, enrollment);
MOCKABLE_FUNCTION(, int, enrollmentGroup_setAttestationMechanism, ENROLLMENT_GROUP_HANDLE, enrollment, ATTESTATION_MECHANISM_HANDLE, att_mech);
MOCKABLE_FUNCTION(, INITIAL_TWIN_HANDLE, enrollmentGroup_getInitialTwin, ENROLLMENT_GROUP_HANDLE, enrollment);
MOCKABLE_FUNCTION(, int, enrollmentGroup_setInitialTwin, ENROLLMENT_GROUP_HANDLE, enrollment, INITIAL_TWIN_HANDLE, twin);
MOCKABLE_FUNCTION(, const char*, enrollmentGroup_getGroupId, ENROLLMENT_GROUP_HANDLE, enrollment);
MOCKABLE_FUNCTION(, const char*, enrollmentGroup_getIotHubHostName, ENROLLMENT_GROUP_HANDLE, enrollment);
MOCKABLE_FUNCTION(, const char*, enrollmentGroup_getEtag, ENROLLMENT_GROUP_HANDLE, enrollment);
MOCKABLE_FUNCTION(, int, enrollmentGroup_setEtag, ENROLLMENT_GROUP_HANDLE, enrollment, const char*, etag);
MOCKABLE_FUNCTION(, PROVISIONING_STATUS, enrollmentGroup_getProvisioningStatus, ENROLLMENT_GROUP_HANDLE, enrollment);
MOCKABLE_FUNCTION(, int, enrollmentGroup_setProvisioningStatus, ENROLLMENT_GROUP_HANDLE, enrollment, PROVISIONING_STATUS, prov_status);
MOCKABLE_FUNCTION(, const char*, enrollmentGroup_getCreatedDateTime, ENROLLMENT_GROUP_HANDLE, enrollment);
MOCKABLE_FUNCTION(, const char*, enrollmentGroup_getUpdatedDateTime, ENROLLMENT_GROUP_HANDLE, enrollment);


/* INTERNAL FUNCTIONS
*
* Do NOT use these. They are for internal use ONLY
*/
MOCKABLE_FUNCTION(, JSON_Value*, individualEnrollment_toJson, INDIVIDUAL_ENROLLMENT_HANDLE, enrollment);
MOCKABLE_FUNCTION(, INDIVIDUAL_ENROLLMENT_HANDLE, individualEnrollment_fromJson, JSON_Object*, root_object);
MOCKABLE_FUNCTION(, ENROLLMENT_GROUP_HANDLE, enrollmentGroup_fromJson, JSON_Object*, root_object);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PROVISIONING_SC_ENROLLMENT_H */
