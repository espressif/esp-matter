// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdbool.h>

#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/gballoc.h"

#include "prov_service_client/provisioning_sc_attestation_mechanism.h"
#include "prov_service_client/provisioning_sc_tpm_attestation.h"
#include "prov_service_client/provisioning_sc_x509_attestation.h"
#include "prov_service_client/provisioning_sc_json_const.h"
#include "prov_service_client/provisioning_sc_shared_helpers.h"
#include "parson.h"

MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(ATTESTATION_TYPE, ATTESTATION_TYPE_VALUES)

typedef struct ATTESTATION_MECHANISM_TAG
{
    ATTESTATION_TYPE type;
    union {
        TPM_ATTESTATION_HANDLE tpm;
        X509_ATTESTATION_HANDLE x509;
    } attestation;
} ATTESTATION_MECHANISM;

static const char* attestationType_toJson(ATTESTATION_TYPE type)
{
    const char* result = NULL;
    if (type == ATTESTATION_TYPE_TPM)
    {
        result = ATTESTATION_TYPE_JSON_VALUE_TPM;
    }
    else if (type == ATTESTATION_TYPE_X509)
    {
        result = ATTESTATION_TYPE_JSON_VALUE_X509;
    }
    else
    {
        LogError("Could not convert '%s' to JSON", MU_ENUM_TO_STRING(ATTESTATION_TYPE, type));
    }

    return result;
}

static ATTESTATION_TYPE attestationType_fromJson(const char* str_rep)
{
    ATTESTATION_TYPE new_type = ATTESTATION_TYPE_NONE;

    if (str_rep != NULL)
    {
        if (strcmp(str_rep, ATTESTATION_TYPE_JSON_VALUE_TPM) == 0)
        {
            new_type = ATTESTATION_TYPE_TPM;
        }
        else if (strcmp(str_rep, ATTESTATION_TYPE_JSON_VALUE_X509) == 0)
        {
            new_type = ATTESTATION_TYPE_X509;
        }
        else
        {
            LogError("Could not convert '%s' from JSON", str_rep);
        }
    }

    return new_type;
}

void attestationMechanism_destroy(ATTESTATION_MECHANISM_HANDLE att_mech)
{
    if (att_mech != NULL)
    {
        if (att_mech->type == ATTESTATION_TYPE_TPM)
        {
            if (att_mech->attestation.tpm != NULL)
            {
                tpmAttestation_destroy(att_mech->attestation.tpm);
            }
        }
        else if (att_mech->type == ATTESTATION_TYPE_X509)
        {
            if (att_mech->attestation.x509 != NULL)
            {
                x509Attestation_destroy(att_mech->attestation.x509);
            }
        }

        free(att_mech);
    }
}

JSON_Value* attestationMechanism_toJson(const ATTESTATION_MECHANISM_HANDLE att_mech)
{
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;

    const char* at_str = NULL;

    //Setup
    if (att_mech == NULL)
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
    else if (((at_str = attestationType_toJson(att_mech->type)) == NULL) || (json_object_set_string(root_object, ATTESTATION_MECHANISM_JSON_KEY_TYPE, at_str) != JSONSuccess))
    {
        LogError("Failed to set '%s' in JSON string representation of Attestation Mechanism", ATTESTATION_MECHANISM_JSON_KEY_TYPE);
        json_value_free(root_value);
        root_value = NULL;
    }
    else
    {
        if (att_mech->type == ATTESTATION_TYPE_TPM)
        {
            if (json_serialize_and_set_struct(root_object, ATTESTATION_MECHANISM_JSON_KEY_TPM, att_mech->attestation.tpm, (TO_JSON_FUNCTION)tpmAttestation_toJson, true) != 0)
            {
                LogError("Failed to set '%s' in JSON string representation of Attestation Mechanism", ATTESTATION_MECHANISM_JSON_KEY_TPM);
                json_value_free(root_value);
                root_value = NULL;
            }
        }
        else if (att_mech->type == ATTESTATION_TYPE_X509)
        {
            if (json_serialize_and_set_struct(root_object, ATTESTATION_MECHANISM_JSON_KEY_X509, att_mech->attestation.x509, (TO_JSON_FUNCTION)x509Attestation_toJson, true) != 0)
            {
                LogError("Failed to set '%s' in JSON string representation of Attestation Mechanism", ATTESTATION_MECHANISM_JSON_KEY_X509);
                json_value_free(root_value);
                root_value = NULL;
            }
        }
    }


    return root_value;
}

ATTESTATION_MECHANISM_HANDLE attestationMechanism_fromJson(JSON_Object* root_object)
{
    ATTESTATION_MECHANISM_HANDLE new_attMech = NULL;

    if (root_object == NULL)
    {
        LogError("No attestation mechanism in JSON");
    }
    else if ((new_attMech = malloc(sizeof(ATTESTATION_MECHANISM))) == NULL)
    {
        LogError("Allocation of Attestation Mechanism failed");
    }
    else
    {
        memset(new_attMech, 0, sizeof(ATTESTATION_MECHANISM));

        if ((new_attMech->type = attestationType_fromJson(json_object_get_string(root_object, ATTESTATION_MECHANISM_JSON_KEY_TYPE))) == ATTESTATION_TYPE_NONE)
        {
            LogError("Failed to set '%s' in Attestation Mechanism", ATTESTATION_MECHANISM_JSON_KEY_TYPE);
            attestationMechanism_destroy(new_attMech);
            new_attMech = NULL;
        }
        else if (new_attMech->type == ATTESTATION_TYPE_TPM)
        {
            if (json_deserialize_and_get_struct((void**)&(new_attMech->attestation.tpm), root_object, ATTESTATION_MECHANISM_JSON_KEY_TPM, (FROM_JSON_FUNCTION)tpmAttestation_fromJson, true) != 0)
            {
                LogError("Failed to set '%s' in Attestation Mechanism", ATTESTATION_MECHANISM_JSON_KEY_TPM);
                attestationMechanism_destroy(new_attMech);
                new_attMech = NULL;
            }
        }
        else if (new_attMech->type == ATTESTATION_TYPE_X509)
        {
            if (json_deserialize_and_get_struct((void**)&(new_attMech->attestation.x509), root_object, ATTESTATION_MECHANISM_JSON_KEY_X509, (FROM_JSON_FUNCTION)x509Attestation_fromJson, true) != 0)
            {
                LogError("Failed to set '%s' in Attestation Mechanism", ATTESTATION_MECHANISM_JSON_KEY_X509);
                attestationMechanism_destroy(new_attMech);
                new_attMech = NULL;
            }
        }
    }

    return new_attMech;
}

ATTESTATION_MECHANISM_HANDLE attestationMechanism_createWithTpm(const char* endorsement_key, const char* storage_root_key)
{
    ATTESTATION_MECHANISM_HANDLE att_mech = NULL;

    if (endorsement_key == NULL)
    {
        LogError("endorsement_key is NULL");
    }
    else if ((att_mech = malloc(sizeof(ATTESTATION_MECHANISM))) == NULL)
    {
        LogError("Allocation of Attestation Mechanism failed");
    }
    else
    {
        memset(att_mech, 0, sizeof(ATTESTATION_MECHANISM));

        if ((att_mech->attestation.tpm = tpmAttestation_create(endorsement_key, storage_root_key)) == NULL)
        {
            LogError("Allocation of TPM Attestation failed");
            attestationMechanism_destroy(att_mech);
            att_mech = NULL;
        }
        else
        {
            att_mech->type = ATTESTATION_TYPE_TPM;
        }
    }

    return att_mech;
}

ATTESTATION_MECHANISM_HANDLE attestationMechanism_createWithX509ClientCert(const char* primary_cert, const char* secondary_cert)
{
    ATTESTATION_MECHANISM_HANDLE att_mech = NULL;

    if (primary_cert == NULL)
    {
        LogError("primary_cert is NULL");
    }
    else if ((att_mech = malloc(sizeof(ATTESTATION_MECHANISM))) == NULL)
    {
        LogError("Allocation of Attestation Mechanism failed");
    }
    else
    {
        memset(att_mech, 0, sizeof(ATTESTATION_MECHANISM));

        if ((att_mech->attestation.x509 = x509Attestation_create(X509_CERTIFICATE_TYPE_CLIENT, primary_cert, secondary_cert)) == NULL)
        {
            LogError("Allocation of X509 Attestation failed");
            attestationMechanism_destroy(att_mech);
            att_mech = NULL;
        }
        else
        {
            att_mech->type = ATTESTATION_TYPE_X509;
        }
    }

    return (ATTESTATION_MECHANISM_HANDLE)att_mech;
}

ATTESTATION_MECHANISM_HANDLE attestationMechanism_createWithX509SigningCert(const char* primary_cert, const char* secondary_cert)
{
    ATTESTATION_MECHANISM_HANDLE att_mech = NULL;

    if (primary_cert == NULL)
    {
        LogError("primary_cert is NULL");
    }
    else if ((att_mech = malloc(sizeof(ATTESTATION_MECHANISM))) == NULL)
    {
        LogError("Allocation of Attestation Mechanism failed");
    }
    else
    {
        memset(att_mech, 0, sizeof(ATTESTATION_MECHANISM));

        if ((att_mech->attestation.x509 = x509Attestation_create(X509_CERTIFICATE_TYPE_SIGNING, primary_cert, secondary_cert)) == NULL)
        {
            LogError("Allocation of X509 Attestation failed");
            attestationMechanism_destroy(att_mech);
            att_mech = NULL;
        }
        else
        {
            att_mech->type = ATTESTATION_TYPE_X509;
        }
    }

    return att_mech;
}

ATTESTATION_MECHANISM_HANDLE attestationMechanism_createWithX509CAReference(const char* primary_ref, const char* secondary_ref)
{
    ATTESTATION_MECHANISM_HANDLE att_mech = NULL;

    if (primary_ref == NULL)
    {
        LogError("primary_cert is NULL");
    }
    else if ((att_mech = malloc(sizeof(ATTESTATION_MECHANISM))) == NULL)
    {
        LogError("Allocation of Attestation Mechanism failed");
    }
    else
    {
        memset(att_mech, 0, sizeof(ATTESTATION_MECHANISM));

        if ((att_mech->attestation.x509 = x509Attestation_create(X509_CERTIFICATE_TYPE_CA_REFERENCES, primary_ref, secondary_ref)) == NULL)
        {
            LogError("Allocation of X509 Attestation failed");
            attestationMechanism_destroy(att_mech);
            att_mech = NULL;
        }
        else
        {
            att_mech->type = ATTESTATION_TYPE_X509;
        }
    }

    return att_mech;
}


TPM_ATTESTATION_HANDLE attestationMechanism_getTpmAttestation(ATTESTATION_MECHANISM_HANDLE att_mech)
{
    TPM_ATTESTATION_HANDLE tpm_att = NULL;

    if (att_mech == NULL)
    {
        LogError("attestation mechanism is NULL");
    }
    else if (att_mech->type != ATTESTATION_TYPE_TPM)
    {
        LogError("attestation mechanism is not of type TPM");
    }
    else
    {
        tpm_att = att_mech->attestation.tpm;
    }

    return tpm_att;
}

X509_ATTESTATION_HANDLE attestationMechanism_getX509Attestation(ATTESTATION_MECHANISM_HANDLE att_mech)
{
    X509_ATTESTATION_HANDLE new_x509_att = NULL;

    if (att_mech == NULL)
    {
        LogError("attestation mechanism is NULL");
    }
    else if (att_mech->type != ATTESTATION_TYPE_X509)
    {
        LogError("attestation mechanism is not of type X509");
    }
    else
    {
        new_x509_att = att_mech->attestation.x509;
    }

    return new_x509_att;
}

bool attestationMechanism_isValidForIndividualEnrollment(ATTESTATION_MECHANISM_HANDLE att_mech)
{
    bool result;

    if (att_mech == NULL)
    {
        result = false;
    }
    else if (att_mech->type == ATTESTATION_TYPE_TPM)
    {
        result = true;
    }
    else if (att_mech->type == ATTESTATION_TYPE_X509)
    {
        if (x509Attestation_getCertificateType(att_mech->attestation.x509) == X509_CERTIFICATE_TYPE_CLIENT)
        {
            result = true;
        }
        else
        {
            result = false;
        }
    }
    else
    {
        result = false;
    }

    return result;
}

bool attestationMechanism_isValidForEnrollmentGroup(ATTESTATION_MECHANISM_HANDLE att_mech)
{
    bool result;

    if (att_mech == NULL)
    {
        result = false;
    }
    else if (att_mech->type == ATTESTATION_TYPE_TPM)
    {
        result = false;
    }
    else if (att_mech->type == ATTESTATION_TYPE_X509)
    {
        if (x509Attestation_getCertificateType(att_mech->attestation.x509) == X509_CERTIFICATE_TYPE_CLIENT)
        {
            result = false;
        }
        else
        {
            result = true;
        }
    }
    else
    {
        result = false;
    }

    return result;
}

ATTESTATION_TYPE attestationMechanism_getType(ATTESTATION_MECHANISM_HANDLE att_mech)
{
    ATTESTATION_TYPE result = ATTESTATION_TYPE_NONE;

    if (att_mech == NULL)
    {
        LogError("attestation mechanism is NULL");
    }
    else
    {
        result = att_mech->type;
    }

    return result;
}
