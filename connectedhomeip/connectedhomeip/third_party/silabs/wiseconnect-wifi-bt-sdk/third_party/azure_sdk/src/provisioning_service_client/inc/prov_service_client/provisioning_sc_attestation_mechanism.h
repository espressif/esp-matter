// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef PROVISIONING_SC_ATTESTATION_MECHANISM_H
#define PROVISIONING_SC_ATTESTATION_MECHANISM_H

#ifdef __cplusplus
#include <cstdbool>
extern "C" {
#else
#include <stdbool.h>
#endif /* __cplusplus */

#include "umock_c/umock_c_prod.h"
#include "azure_macro_utils/macro_utils.h"
#include "provisioning_sc_tpm_attestation.h"
#include "provisioning_sc_x509_attestation.h"
#include "parson.h"

typedef struct ATTESTATION_MECHANISM_TAG* ATTESTATION_MECHANISM_HANDLE;

#define ATTESTATION_TYPE_VALUES \
        ATTESTATION_TYPE_NONE, \
        ATTESTATION_TYPE_TPM, \
        ATTESTATION_TYPE_X509 \
//Note: ATTESTATION_TYPE_NONE is invalid, indicating error
MU_DEFINE_ENUM_WITHOUT_INVALID(ATTESTATION_TYPE, ATTESTATION_TYPE_VALUES);


/** @brief  Creates an Attestation Mechanism handle that uses a TPM Attestation for use in consequent APIs.
*
* @param    endorsement_key     An endorsement key to use with the TPM.
* @param    storage_root_key    A storage root key to use with the TPM (optional).
*
* @return   A non NULL handle representing an Attestation Mechanism using a TPM Attestation, and NULL on failure.
*/
MOCKABLE_FUNCTION(, ATTESTATION_MECHANISM_HANDLE, attestationMechanism_createWithTpm, const char*, endorsement_key, const char*, storage_root_key);

/** @brief  Creates an Attestation Mechanism handle that uses an x509 Attestation with client certificate(s) for use in consequent APIs.
*           Please note that an x509 Attestation with a client certificate is NOT VALID when attached to an enrollment group.
*
* @param    primary_cert        A primary certificate for use with the x509.
* @param    secondary_cert      A secondary certificate for use with the x509 (optional - if not using two certs, pass NULL).
*
* @return   A non NULL handle representing an Attestation Mechanism using an X509 Attestation with a client certificate, and NULL on failure.
*/
MOCKABLE_FUNCTION(, ATTESTATION_MECHANISM_HANDLE, attestationMechanism_createWithX509ClientCert, const char*, primary_cert, const char*, secondary_cert);

/** @brief  Creates an Attestation Mechanism handle that uses an x509 Attestation with signing certificate(s) for use in consequent APIs.
*           Please note that an x509 Attestation with a signing certificate is NOT VALID when attached to an individual enrollment.
*
* @param    primary_cert        A primary certificate for use with the x509.
* @param    secondary_cert      A secondary certificate for use with the x509 (optional - if not using two certs, pass NULL).
*
* @return   A non NULL handle representing an Attestation Mechanism using an X509 Attestation with a signing certificate, and NULL on failure.
*/
MOCKABLE_FUNCTION(, ATTESTATION_MECHANISM_HANDLE, attestationMechanism_createWithX509SigningCert, const char*, primary_cert, const char*, secondary_cert);

/** @brief  Creates an Attestation Mechanism handle that uses an x509 Attestation with CA Certificate Reference(s) for use in consequent APIs.
*           Please note that an x509 Attestation with a CA Certificate Reference is NOT VALID when attached to an Individual Enrollment.
*
* @param    primary_ref         A reference to a primary CA Certificate for use with the x509.
* @param    secondary_ref       A reference to a secondary CA Certificate for use with the x509 (optional - if not using two cert refs, pass NULL).
*
* @return   A non NULL handle representing an Attestation Mechanism using an X509 Attestation with a CA Reference, and NULL on failure.
*/
MOCKABLE_FUNCTION(, ATTESTATION_MECHANISM_HANDLE, attestationMechanism_createWithX509CAReference, const char*, primary_ref, const char*, secondary_ref);

/** @brief  Destroys an Attestation Mechanism handle, freeing all allocated memory. Please note that this also includes any memory
*           in more specific handles generated from the handle (e.g. TPM_ATTESTATION_HANDLE). Please note further that this will also
*           cause any Enrollment that the Attestation Mechanism has been attached to to have unexpected behvaiours. Do not use this function
*           unless the attestation mechanism is unattached.
*
* @param    att_mech          The handle of the Attestation Mechanism
*/
MOCKABLE_FUNCTION(, void, attestationMechanism_destroy, ATTESTATION_MECHANISM_HANDLE, att_mech);

/** @brief  Returns result indicating if an attestation mechanism is valid to be attached to an Individual Enrollment
*
* @param    att_mech          The handle of the Attestation Mechanism
*/
MOCKABLE_FUNCTION(, bool, attestationMechanism_isValidForIndividualEnrollment, ATTESTATION_MECHANISM_HANDLE, att_mech);

/** @brief  Returns result indicating if an attestation mechanism is valid to be attached to an Enrollment Group
*
* @param    att_mech          The handle of the Attestation Mechanism
*/
MOCKABLE_FUNCTION(, bool, attestationMechanism_isValidForEnrollmentGroup, ATTESTATION_MECHANISM_HANDLE, att_mech);


/* Attestation Mechanism Accessor Functions */
MOCKABLE_FUNCTION(, ATTESTATION_TYPE, attestationMechanism_getType, ATTESTATION_MECHANISM_HANDLE, att_mech);
MOCKABLE_FUNCTION(, TPM_ATTESTATION_HANDLE, attestationMechanism_getTpmAttestation, ATTESTATION_MECHANISM_HANDLE, att_mech);
MOCKABLE_FUNCTION(, X509_ATTESTATION_HANDLE, attestationMechanism_getX509Attestation, ATTESTATION_MECHANISM_HANDLE, att_mech);



/*---INTERNAL USAGE ONLY---*/
MOCKABLE_FUNCTION(, ATTESTATION_MECHANISM_HANDLE, attestationMechanism_fromJson, JSON_Object*, root_object);
MOCKABLE_FUNCTION(, JSON_Value*, attestationMechanism_toJson, const ATTESTATION_MECHANISM_HANDLE, att_mech);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PROVISIONING_SC_ATTESTATION_MECHANISM_H */