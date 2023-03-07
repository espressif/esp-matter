// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef PROVISIONING_SC_TPM_ATTESTATION_H
#define PROVISIONING_SC_TPM_ATTESTATION_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "umock_c/umock_c_prod.h"
#include "azure_macro_utils/macro_utils.h"
#include "parson.h"

typedef struct TPM_ATTESTATION_TAG* TPM_ATTESTATION_HANDLE;

/* TPM Attestation Accessor Functions */
MOCKABLE_FUNCTION(, const char*, tpmAttestation_getEndorsementKey, TPM_ATTESTATION_HANDLE, tpm_att);



/*---INTERNAL USAGE ONLY---*/
MOCKABLE_FUNCTION(, TPM_ATTESTATION_HANDLE, tpmAttestation_create, const char*, endorsement_key, const char*, storage_root_key);
MOCKABLE_FUNCTION(, void, tpmAttestation_destroy, TPM_ATTESTATION_HANDLE, tpm_att);
MOCKABLE_FUNCTION(, TPM_ATTESTATION_HANDLE, tpmAttestation_fromJson, JSON_Object*, root_object);
MOCKABLE_FUNCTION(, JSON_Value*, tpmAttestation_toJson, const TPM_ATTESTATION_HANDLE, tpm_att);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PROVISIONING_SC_TPM_ATTESTATION_H */