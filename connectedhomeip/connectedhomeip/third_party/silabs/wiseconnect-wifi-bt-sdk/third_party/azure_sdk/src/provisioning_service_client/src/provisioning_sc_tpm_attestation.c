// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>

#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"

#include "prov_service_client/provisioning_sc_tpm_attestation.h"
#include "prov_service_client/provisioning_sc_json_const.h"
#include "prov_service_client/provisioning_sc_shared_helpers.h"
#include "parson.h"

typedef struct TPM_ATTESTATION_TAG
{
    char* endorsement_key;
    char* storage_root_key;
} TPM_ATTESTATION;

void tpmAttestation_destroy(TPM_ATTESTATION_HANDLE tpm_att)
{
    if (tpm_att != NULL)
    {
        free(tpm_att->endorsement_key);
        free(tpm_att->storage_root_key);
        free(tpm_att);
    }
}

JSON_Value* tpmAttestation_toJson(const TPM_ATTESTATION_HANDLE tpm_att)
{
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;

    //Setup
    if (tpm_att == NULL)
    {
        LogError("tpm attestation is NULL");
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
    else if (json_object_set_string(root_object, TPM_ATTESTATION_JSON_KEY_EK, tpm_att->endorsement_key) != JSONSuccess)
    {
        LogError("Failed to set '%s' in JSON string representation of TPM Attestation", TPM_ATTESTATION_JSON_KEY_EK);
        json_value_free(root_value);
        root_value = NULL;
    }
    else if ((tpm_att->storage_root_key != NULL) && (json_object_set_string(root_object, TPM_ATTESTATION_JSON_KEY_SRK, tpm_att->storage_root_key) != JSONSuccess))
    {
        LogError("Failed to set '%s' in JSON string representation of TPM Attestation", TPM_ATTESTATION_JSON_KEY_SRK);
        json_value_free(root_value);
        root_value = NULL;
    }

    return root_value;
}

TPM_ATTESTATION_HANDLE tpmAttestation_fromJson(JSON_Object * root_object)
{
    TPM_ATTESTATION_HANDLE new_tpmAtt = NULL;

    if (root_object == NULL)
    {
        LogError("No TPM Attestation in JSON");
    }
    else if ((new_tpmAtt = malloc(sizeof(TPM_ATTESTATION))) == NULL)
    {
        LogError("Allocation of TPM Attestation failed");
    }
    else
    {
        memset(new_tpmAtt, 0, sizeof(TPM_ATTESTATION));

        if (copy_json_string_field(&(new_tpmAtt->endorsement_key), root_object, TPM_ATTESTATION_JSON_KEY_EK) != 0)
        {
            LogError("Failed to set '%s' in TPM Attestation", TPM_ATTESTATION_JSON_KEY_EK);
            tpmAttestation_destroy(new_tpmAtt);
            new_tpmAtt = NULL;
        }
        else if (copy_json_string_field(&(new_tpmAtt->storage_root_key), root_object, TPM_ATTESTATION_JSON_KEY_SRK) != 0)
        {
            LogError("Failed to set '%s' in TPM Attestation", TPM_ATTESTATION_JSON_KEY_SRK);
            tpmAttestation_destroy(new_tpmAtt);
            new_tpmAtt = NULL;
        }
    }

    return new_tpmAtt;
}

TPM_ATTESTATION_HANDLE tpmAttestation_create(const char* endorsement_key, const char* storage_root_key)
{
    TPM_ATTESTATION_HANDLE new_tpmAtt = NULL;

    if (endorsement_key == NULL)
    {
        LogError("Requires valid endorsement key");
    }
    else if ((new_tpmAtt = malloc(sizeof(TPM_ATTESTATION))) == NULL)
    {
        LogError("Failed to allocate memory for TPM Attestation");
    }
    else
    {
        memset(new_tpmAtt, 0, sizeof(TPM_ATTESTATION));

        if (mallocAndStrcpy_s(&(new_tpmAtt->endorsement_key), endorsement_key) != 0)
        {
            LogError("Setting endorsement key in TPM Attestation failed");
            tpmAttestation_destroy(new_tpmAtt);
            new_tpmAtt = NULL;
        }
        else if ((storage_root_key != NULL) && (mallocAndStrcpy_s(&(new_tpmAtt->storage_root_key), storage_root_key) != 0))
        {
            LogError("Setting storage root key in TPM Attestation failed");
            tpmAttestation_destroy(new_tpmAtt);
            new_tpmAtt = NULL;
        }
    }

    return new_tpmAtt;
}

const char* tpmAttestation_getEndorsementKey(TPM_ATTESTATION_HANDLE tpm_att)
{
    char* result = NULL;

    if (tpm_att != NULL)
    {
        result = tpm_att->endorsement_key;
    }

    return result;
}
