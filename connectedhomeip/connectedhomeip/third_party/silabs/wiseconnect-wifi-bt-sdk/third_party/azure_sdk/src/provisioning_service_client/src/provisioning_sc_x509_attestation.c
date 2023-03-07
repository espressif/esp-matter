// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>

#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/azure_base64.h"

#include "prov_service_client/provisioning_sc_x509_attestation.h"
#include "prov_service_client/provisioning_sc_json_const.h"
#include "prov_service_client/provisioning_sc_shared_helpers.h"
#include "parson.h"

MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(X509_CERTIFICATE_TYPE, X509_CERTIFICATE_TYPE_VALUES)

typedef struct X509_CERTIFICATE_INFO_TAG
{
    char* subject_name;
    char* sha1_thumbprint;
    char* sha256_thumbprint;
    char* issuer_name;
    char* not_before_utc;
    char* not_after_utc;
    char* serial_number;
    int version;
} X509_CERTIFICATE_INFO;

typedef struct X509_CERTIFICATE_WITH_INFO_TAG
{
    char* certificate;
    X509_CERTIFICATE_INFO* info;
} X509_CERTIFICATE_WITH_INFO;

typedef struct X509_CERTIFICATES_TAG
{
    X509_CERTIFICATE_WITH_INFO* primary;
    X509_CERTIFICATE_WITH_INFO* secondary;
} X509_CERTIFICATES;

typedef struct X509_CA_REFERENCES_TAG
{
    char* primary;
    char* secondary;
} X509_CA_REFERENCES;

typedef struct X509_ATTESTATION_TAG
{
    X509_CERTIFICATE_TYPE type;
    union {
        X509_CERTIFICATES* client_certificates;
        X509_CERTIFICATES* signing_certificates;
        X509_CA_REFERENCES* ca_references;
    } certificates;

} X509_ATTESTATION;

static int convert_cert_to_b64(const char* cert_in, char** cert_b64_out)
{
    int ret = 0;
    if (cert_in != NULL)
    {
        STRING_HANDLE cert_b64;
        if ((cert_b64 = Azure_Base64_Encode_Bytes((const unsigned char*)cert_in, strlen(cert_in))) == NULL)
        {
            LogError("Could not convert certificate to Base64");
            ret = MU_FAILURE;
            *cert_b64_out = NULL;
        }
        else
        {
            if (mallocAndStrcpy_s(cert_b64_out, STRING_c_str(cert_b64)) != 0)
            {
                LogError("copying b64 cert failed");
                ret = MU_FAILURE;
                *cert_b64_out = NULL;
            }
            STRING_delete(cert_b64);
        }
    }
    else
    {
        *cert_b64_out = NULL;
    }
    return ret;
}

static void x509CAReferences_free(X509_CA_REFERENCES* x509_ca_ref)
{
    if (x509_ca_ref != NULL)
    {
        free(x509_ca_ref->primary);
        free(x509_ca_ref->secondary);
        free(x509_ca_ref);
    }
}

static X509_CA_REFERENCES* x509CAReferences_create(const char* primary, const char* secondary)
{
    X509_CA_REFERENCES* new_x509CARef = NULL;

    if (primary == NULL)
    {
        LogError("Requires valid primary CA Reference");
    }
    else if ((new_x509CARef = malloc(sizeof(X509_CA_REFERENCES))) == NULL)
    {
        LogError("Failed to allocate memory for X509 CA References");
    }
    else
    {
        memset(new_x509CARef, 0, sizeof(X509_CA_REFERENCES));

        if (mallocAndStrcpy_s(&(new_x509CARef->primary), primary) != 0)
        {
            LogError("Error setting primary CA Reference in X509CAReferences");
            x509CAReferences_free(new_x509CARef);
            new_x509CARef = NULL;
        }
        else if ((secondary != NULL) && (mallocAndStrcpy_s(&(new_x509CARef->secondary), secondary) != 0))
        {
            LogError("Error setting secondary CA Reference in X509CAReferences");
            x509CAReferences_free(new_x509CARef);
            new_x509CARef = NULL;
        }
    }

    return new_x509CARef;
}

static JSON_Value* x509CAReferences_toJson(const X509_CA_REFERENCES* x509_ca_ref)
{
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;

    if ((root_value = json_value_init_object()) == NULL)
    {
        LogError("json_value_init_object failed");
    }
    else if ((root_object = json_value_get_object(root_value)) == NULL)
    {
        LogError("json_value_get_object failed");
        json_value_free(root_value);
        root_value = NULL;
    }

    else if (json_object_set_string(root_object, X509_CA_REFERENCES_JSON_KEY_PRIMARY, x509_ca_ref->primary) != JSONSuccess)
    {
        LogError("Failed to set '%s' in JSON String", X509_CA_REFERENCES_JSON_KEY_PRIMARY);
        json_value_free(root_value);
        root_value = NULL;
    }
    else if ((x509_ca_ref->secondary != NULL) && (json_object_set_string(root_object, X509_CA_REFERENCES_JSON_KEY_SECONDARY, x509_ca_ref->secondary) != JSONSuccess))
    {
        LogError("Failed to set '%s' in JSON String", X509_CA_REFERENCES_JSON_KEY_SECONDARY);
        json_value_free(root_value);
        root_value = NULL;
    }

    return root_value;
}

static X509_CA_REFERENCES* x509CAReferences_fromJson(JSON_Object* root_object)
{
    X509_CA_REFERENCES* new_x509CARef = NULL;

    if ((new_x509CARef = malloc(sizeof(X509_CA_REFERENCES))) == NULL)
    {
        LogError("Allocation of X509 CA References failed");
    }
    else
    {
        memset(new_x509CARef, 0, sizeof(X509_CA_REFERENCES));

        if (copy_json_string_field(&(new_x509CARef->primary), root_object, X509_CA_REFERENCES_JSON_KEY_PRIMARY) != 0)
        {
            LogError("Failed to set '%s' in X509 CA References", X509_CA_REFERENCES_JSON_KEY_PRIMARY);
            x509CAReferences_free(new_x509CARef);
            new_x509CARef = NULL;
        }
        else if (copy_json_string_field(&(new_x509CARef->secondary), root_object, X509_CA_REFERENCES_JSON_KEY_SECONDARY) != 0)
        {
            LogError("Failed to set '%s' in X509 CA References", X509_CA_REFERENCES_JSON_KEY_SECONDARY);
            x509CAReferences_free(new_x509CARef);
            new_x509CARef = NULL;
        }
    }

    return new_x509CARef;
}

static void x509CertificateInfo_free(X509_CERTIFICATE_INFO* x509_info)
{
    if (x509_info != NULL)
    {
        free(x509_info->subject_name);
        free(x509_info->sha1_thumbprint);
        free(x509_info->sha256_thumbprint);
        free(x509_info->issuer_name);
        free(x509_info->not_before_utc);
        free(x509_info->not_after_utc);
        free(x509_info->serial_number);
        free(x509_info);
    }
}

static JSON_Value* x509CertificateInfo_toJson(const X509_CERTIFICATE_INFO* x509_info)
{
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;

    //Setup
    if ((root_value = json_value_init_object()) == NULL)
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
    else if (json_object_set_string(root_object, X509_CERTIFICATE_INFO_JSON_KEY_SUBJECT_NAME, x509_info->subject_name) != JSONSuccess)
    {
        LogError("Failed to set '%s' in JSON string", X509_CERTIFICATE_INFO_JSON_KEY_SUBJECT_NAME);
        json_value_free(root_value);
        root_value = NULL;
    }
    else if (json_object_set_string(root_object, X509_CERTIFICATE_INFO_JSON_KEY_SHA1, x509_info->sha1_thumbprint) != JSONSuccess)
    {
        LogError("Failed to set '%s' in JSON string", X509_CERTIFICATE_INFO_JSON_KEY_SHA1);
        json_value_free(root_value);
        root_value = NULL;
    }
    else if (json_object_set_string(root_object, X509_CERTIFICATE_INFO_JSON_KEY_SHA256, x509_info->sha256_thumbprint) != JSONSuccess)
    {
        LogError("Failed to set '%s' in JSON string", X509_CERTIFICATE_INFO_JSON_KEY_SHA256);
        json_value_free(root_value);
        root_value = NULL;
    }
    else if (json_object_set_string(root_object, X509_CERTIFICATE_INFO_JSON_KEY_ISSUER, x509_info->issuer_name) != JSONSuccess)
    {
        LogError("Failed to set '%s' in JSON string", X509_CERTIFICATE_INFO_JSON_KEY_ISSUER);
        json_value_free(root_value);
        root_value = NULL;
    }
    else if (json_object_set_string(root_object, X509_CERTIFICATE_INFO_JSON_KEY_NOT_BEFORE, x509_info->not_before_utc) != JSONSuccess)
    {
        LogError("Failed to set '%s' in JSON string", X509_CERTIFICATE_INFO_JSON_KEY_NOT_BEFORE);
        json_value_free(root_value);
        root_value = NULL;
    }
    else if (json_object_set_string(root_object, X509_CERTIFICATE_INFO_JSON_KEY_NOT_AFTER, x509_info->not_after_utc) != JSONSuccess)
    {
        LogError("Failed to set '%s' in JSON string", X509_CERTIFICATE_INFO_JSON_KEY_NOT_AFTER);
        json_value_free(root_value);
        root_value = NULL;
    }
    else if (json_object_set_string(root_object, X509_CERTIFICATE_INFO_JSON_KEY_SERIAL_NO, x509_info->serial_number) != JSONSuccess)
    {
        LogError("Failed to set '%s' in JSON string", X509_CERTIFICATE_INFO_JSON_KEY_SERIAL_NO);
        json_value_free(root_value);
        root_value = NULL;
    }
    else if (json_object_set_number(root_object, X509_CERTIFICATE_INFO_JSON_KEY_VERSION, x509_info->version) != JSONSuccess)
    {
        LogError("Failed to set '%s' in JSON string", X509_CERTIFICATE_INFO_JSON_KEY_VERSION);
        json_value_free(root_value);
        root_value = NULL;
    }

    return root_value;
}

static X509_CERTIFICATE_INFO* x509CertificateInfo_fromJson(JSON_Object* root_object)
{
    X509_CERTIFICATE_INFO* new_x509Info = NULL;

    if (root_object == NULL)
    {
        LogError("No X509 Certificate Info in JSON");
    }
    else if ((new_x509Info = malloc(sizeof(X509_CERTIFICATE_INFO))) == NULL)
    {
        LogError("Allocation of X509 Certificate Info failed");
    }
    else
    {
        memset(new_x509Info, 0, sizeof(X509_CERTIFICATE_INFO));

        if (copy_json_string_field(&(new_x509Info->subject_name), root_object, X509_CERTIFICATE_INFO_JSON_KEY_SUBJECT_NAME) != 0)
        {
            LogError("Failed to set '%s' in X509 Certificate Info", X509_CERTIFICATE_INFO_JSON_KEY_SUBJECT_NAME);
            x509CertificateInfo_free(new_x509Info);
            new_x509Info = NULL;
        }
        else if (copy_json_string_field(&(new_x509Info->sha1_thumbprint), root_object, X509_CERTIFICATE_INFO_JSON_KEY_SHA1) != 0)
        {
            LogError("Failed to set '%s' in X509 Certificate Info", X509_CERTIFICATE_INFO_JSON_KEY_SHA1);
            x509CertificateInfo_free(new_x509Info);
            new_x509Info = NULL;
        }
        else if (copy_json_string_field(&(new_x509Info->sha256_thumbprint), root_object, X509_CERTIFICATE_INFO_JSON_KEY_SHA256) != 0)
        {
            LogError("Failed to set '%s' in X509 Certificate Info", X509_CERTIFICATE_INFO_JSON_KEY_SHA256);
            x509CertificateInfo_free(new_x509Info);
            new_x509Info = NULL;
        }
        else if (copy_json_string_field(&(new_x509Info->issuer_name), root_object, X509_CERTIFICATE_INFO_JSON_KEY_ISSUER) != 0)
        {
            LogError("Failed to set '%s' in X509 Certificate Info", X509_CERTIFICATE_INFO_JSON_KEY_ISSUER);
            x509CertificateInfo_free(new_x509Info);
            new_x509Info = NULL;
        }
        else if (copy_json_string_field(&(new_x509Info->not_before_utc), root_object, X509_CERTIFICATE_INFO_JSON_KEY_NOT_BEFORE) != 0)
        {
            LogError("Failed to set '%s' in X509 Certificate Info", X509_CERTIFICATE_INFO_JSON_KEY_NOT_BEFORE);
            x509CertificateInfo_free(new_x509Info);
            new_x509Info = NULL;
        }
        else if (copy_json_string_field(&(new_x509Info->not_after_utc), root_object, X509_CERTIFICATE_INFO_JSON_KEY_NOT_AFTER) != 0)
        {
            LogError("Failed to set '%s' in X509 Certificate Info", X509_CERTIFICATE_INFO_JSON_KEY_NOT_AFTER);
            x509CertificateInfo_free(new_x509Info);
            new_x509Info = NULL;
        }
        else if (copy_json_string_field(&(new_x509Info->serial_number), root_object, X509_CERTIFICATE_INFO_JSON_KEY_SERIAL_NO) != 0)
        {
            LogError("Failed to set '%s' in X509 Certificate Info", X509_CERTIFICATE_INFO_JSON_KEY_SERIAL_NO);
            x509CertificateInfo_free(new_x509Info);
            new_x509Info = NULL;
        }
        else
        {
            new_x509Info->version = (int)json_object_get_number(root_object, X509_CERTIFICATE_INFO_JSON_KEY_VERSION);
        }
    }

    return new_x509Info;
}

static JSON_Value* x509CertificateWithInfo_toJson(const X509_CERTIFICATE_WITH_INFO* x509_certinfo)
{
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;

    //Setup
    if ((root_value = json_value_init_object()) == NULL)
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
    else if ((x509_certinfo->certificate != NULL) && (json_object_set_string(root_object, X509_CERTIFICATE_WITH_INFO_JSON_KEY_CERTIFICATE, x509_certinfo->certificate) != JSONSuccess))
    {
        LogError("Failed to set '%s' in JSON string representation of X509 Certificate With Info", X509_CERTIFICATE_WITH_INFO_JSON_KEY_CERTIFICATE);
        json_value_free(root_value);
        root_value = NULL;
    }
    else if (x509_certinfo->info != NULL)
    {
        if (json_serialize_and_set_struct(root_object, X509_CERTIFICATE_WITH_INFO_JSON_KEY_INFO, x509_certinfo->info, (TO_JSON_FUNCTION)x509CertificateInfo_toJson, true) != 0)
        {
            LogError("Failed to set '%s' in JSON string representation of X509 Certificate With Info", X509_CERTIFICATE_WITH_INFO_JSON_KEY_INFO);
            json_value_free(root_value);
            root_value = NULL;
        }
    }


    return root_value;
}

static void x509CertificateWithInfo_free(X509_CERTIFICATE_WITH_INFO* x509_certinfo)
{
    if (x509_certinfo != NULL)
    {
        free(x509_certinfo->certificate);
        x509CertificateInfo_free(x509_certinfo->info);
        free(x509_certinfo);
    }
}

static X509_CERTIFICATE_WITH_INFO* x509CertificateWithInfo_fromJson(JSON_Object* root_object)
{
    X509_CERTIFICATE_WITH_INFO* new_x509CertInfo = NULL;

    if (root_object == NULL)
    {
        LogError("No Certificate with Info in JSON");
    }
    else if ((new_x509CertInfo = malloc(sizeof(X509_CERTIFICATE_WITH_INFO))) == NULL)
    {
        LogError("Allocation of X509 Certificate With Info failed");
    }
    else
    {
        memset(new_x509CertInfo, 0, sizeof(X509_CERTIFICATE_WITH_INFO));
        if (copy_json_string_field(&(new_x509CertInfo->certificate), root_object, X509_CERTIFICATE_WITH_INFO_JSON_KEY_CERTIFICATE) != 0)
        {
            LogError("Failed to set '%s' in X509 Certificate With Info", X509_CERTIFICATE_WITH_INFO_JSON_KEY_CERTIFICATE);
            x509CertificateWithInfo_free(new_x509CertInfo);
            new_x509CertInfo = NULL;
        }
        else if (json_deserialize_and_get_struct((void**)&(new_x509CertInfo->info), root_object, X509_CERTIFICATE_WITH_INFO_JSON_KEY_INFO, (FROM_JSON_FUNCTION)x509CertificateInfo_fromJson, true) != 0)
        {
            LogError("Failed to set '%s' in X509 Certificate With Info", X509_CERTIFICATE_WITH_INFO_JSON_KEY_INFO);
            x509CertificateWithInfo_free(new_x509CertInfo);
            new_x509CertInfo = NULL;
        }
    }

    return new_x509CertInfo;
}

static X509_CERTIFICATE_WITH_INFO* x509CertificateWithInfo_create(const char* cert)
{
    X509_CERTIFICATE_WITH_INFO* new_x509CertWithInfo = NULL;

    if (cert == NULL)
    {
        LogError("certificate is NULL");
    }
    else if ((new_x509CertWithInfo = malloc(sizeof(X509_CERTIFICATE_WITH_INFO))) == NULL)
    {
        LogError("Allocating memory for X509 Certificate With Info failed");
    }
    else
    {
        memset(new_x509CertWithInfo, 0, sizeof(X509_CERTIFICATE_WITH_INFO));

        if (mallocAndStrcpy_s(&(new_x509CertWithInfo->certificate), cert) != 0)
        {
            LogError("Error setting certificate in X509CertificateWithInfo");
            x509CertificateWithInfo_free(new_x509CertWithInfo);
            new_x509CertWithInfo = NULL;
        }
        //adding the actual info struct is only when building from JSON
    }

    return new_x509CertWithInfo;
}

static void x509Certificates_free(X509_CERTIFICATES* x509_certs)
{
    if (x509_certs != NULL)
    {
        if (x509_certs->primary != NULL)
        {
            x509CertificateWithInfo_free(x509_certs->primary);
        }
        if (x509_certs->secondary != NULL)
        {
            x509CertificateWithInfo_free(x509_certs->secondary);
        }
        free(x509_certs);
    }
}

static X509_CERTIFICATES* x509Certificates_create(const char* primary_cert, const char* secondary_cert)
{
    X509_CERTIFICATES* new_x509Certs = NULL;

    if (primary_cert == NULL)
    {
        LogError("Requires valid primary certificate");
    }
    else if ((new_x509Certs = malloc(sizeof(X509_CERTIFICATES))) == NULL)
    {
        LogError("Failed to allocate memory for X509 Certificates");
    }
    else
    {
        memset(new_x509Certs, 0, sizeof(X509_CERTIFICATES));

        //Primary Cert is mandatory
        if ((new_x509Certs->primary = x509CertificateWithInfo_create(primary_cert)) == NULL)
        {
            LogError("Failed to create Primary Certificate");
            x509Certificates_free(new_x509Certs);
            new_x509Certs = NULL;
        }

        //Secondary Cert is optional
        else if ((secondary_cert != NULL) && ((new_x509Certs->secondary = x509CertificateWithInfo_create(secondary_cert)) == NULL))
        {
            LogError("Failed to create Secondary Certificate");
            x509Certificates_free(new_x509Certs);
            new_x509Certs = NULL;
        }
    }

    return new_x509Certs;
}

static JSON_Value* x509Certificates_toJson(const X509_CERTIFICATES* x509_certs)
{
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;

    //Setup
    if ((root_value = json_value_init_object()) == NULL)
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
    else
    {
        if (json_serialize_and_set_struct(root_object, X509_CERTIFICATES_JSON_KEY_PRIMARY, x509_certs->primary, (TO_JSON_FUNCTION)x509CertificateWithInfo_toJson, true) != 0)
        {
            LogError("Failed to set '%s' in JSON string representation of X509 Certificates", X509_CERTIFICATES_JSON_KEY_PRIMARY);
            json_value_free(root_value);
            root_value = NULL;
        }
        else if (json_serialize_and_set_struct(root_object, X509_CERTIFICATES_JSON_KEY_SECONDARY, x509_certs->secondary, (TO_JSON_FUNCTION)x509CertificateWithInfo_toJson, false) != 0)
        {
            LogError("Failed to set '%s' in JSON string representation of X509 Certificates", X509_CERTIFICATES_JSON_KEY_SECONDARY);
            json_value_free(root_value);
            root_value = NULL;
        }
    }

    return root_value;
}

static X509_CERTIFICATES* x509Certificates_fromJson(JSON_Object* root_object)
{
    X509_CERTIFICATES* new_x509certs = NULL;

    if (root_object == NULL)
    {
        LogError("No X509 Certificates in JSON");
    }
    else if ((new_x509certs = malloc(sizeof(X509_CERTIFICATES))) == NULL)
    {
        LogError("Allocation of X509 Certificates failed");
    }
    else
    {
        memset(new_x509certs, 0, sizeof(X509_CERTIFICATES));

        if (json_deserialize_and_get_struct((void**)&(new_x509certs->primary), root_object, X509_CERTIFICATES_JSON_KEY_PRIMARY, (FROM_JSON_FUNCTION)x509CertificateWithInfo_fromJson, true) != 0)
        {
            LogError("Failed to set '%s' in X509 Certificates", X509_CERTIFICATES_JSON_KEY_PRIMARY);
            x509Certificates_free(new_x509certs);
            new_x509certs = NULL;
        }
        else if (json_deserialize_and_get_struct((void**)&(new_x509certs->secondary), root_object, X509_CERTIFICATES_JSON_KEY_SECONDARY, (FROM_JSON_FUNCTION)x509CertificateWithInfo_fromJson, false) != 0)
        {
            LogError("Failed to set '%s' in X509 Certificates", X509_CERTIFICATES_JSON_KEY_SECONDARY);
            x509Certificates_free(new_x509certs);
            new_x509certs = NULL;
        }
    }
    return new_x509certs;
}

void x509Attestation_destroy(X509_ATTESTATION_HANDLE x509_att)
{
    if (x509_att != NULL)
    {
        if (x509_att->type == X509_CERTIFICATE_TYPE_CLIENT)
        {
            if (x509_att->certificates.client_certificates != NULL)
            {
                x509Certificates_free(x509_att->certificates.client_certificates);
            }
        }
        else if (x509_att->type == X509_CERTIFICATE_TYPE_SIGNING)
        {
            if (x509_att->certificates.signing_certificates != NULL)
            {
                x509Certificates_free(x509_att->certificates.signing_certificates);
            }
        }
        else if (x509_att->type == X509_CERTIFICATE_TYPE_CA_REFERENCES)
        {
            if (x509_att->certificates.ca_references != NULL)
            {
                x509CAReferences_free(x509_att->certificates.ca_references);
            }
        }

        free(x509_att);
    }
}

JSON_Value* x509Attestation_toJson(const X509_ATTESTATION_HANDLE x509_att)
{
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;

    //Setup
    if (x509_att == NULL)
    {
        LogError("NULL x509 Attestation");
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
    else
    {
        if (x509_att->type == X509_CERTIFICATE_TYPE_CLIENT)
        {
            if (json_serialize_and_set_struct(root_object, X509_ATTESTATION_JSON_KEY_CLIENT_CERTS, x509_att->certificates.client_certificates, (TO_JSON_FUNCTION)x509Certificates_toJson, true) != 0)
            {
                LogError("Failed to set '%s' in JSON string representation of X509 Attestation", X509_ATTESTATION_JSON_KEY_CLIENT_CERTS);
                json_value_free(root_value);
                root_value = NULL;
            }
        }
        else if (x509_att->type == X509_CERTIFICATE_TYPE_SIGNING)
        {
            if (json_serialize_and_set_struct(root_object, X509_ATTESTATION_JSON_KEY_SIGNING_CERTS, x509_att->certificates.signing_certificates, (TO_JSON_FUNCTION)x509Certificates_toJson, true) != 0)
            {
                LogError("Failed to set '%s' in JSON string representation of X509 Attestation", X509_ATTESTATION_JSON_KEY_SIGNING_CERTS);
                json_value_free(root_value);
                root_value = NULL;
            }
        }
        else if (x509_att->type == X509_CERTIFICATE_TYPE_CA_REFERENCES)
        {
            if (json_serialize_and_set_struct(root_object, X509_ATTESTATION_JSON_KEY_CA_REFERENCES, x509_att->certificates.ca_references, (TO_JSON_FUNCTION)x509CAReferences_toJson, true) != 0)
            {
                LogError("Failed to set '%s' in JSON string representation of X509 Attestation", X509_ATTESTATION_JSON_KEY_CA_REFERENCES);
                json_value_free(root_value);
                root_value = NULL;
            }
        }
    }

    return root_value;
}

X509_ATTESTATION_HANDLE x509Attestation_fromJson(JSON_Object* root_object)
{
    X509_ATTESTATION_HANDLE new_x509Att = NULL;

    if (root_object == NULL)
    {
        LogError("No X509 Attestation in JSON");
    }
    else if ((new_x509Att = malloc(sizeof(X509_ATTESTATION))) == NULL)
    {
        LogError("Allocation of X509 Attestation failed");
    }
    else
    {
        memset(new_x509Att, 0, sizeof(X509_ATTESTATION));

        if (json_object_has_value(root_object, X509_ATTESTATION_JSON_KEY_CLIENT_CERTS))
        {
            if (json_deserialize_and_get_struct((void**)&(new_x509Att->certificates.client_certificates), root_object, X509_ATTESTATION_JSON_KEY_CLIENT_CERTS, (FROM_JSON_FUNCTION)x509Certificates_fromJson, true) != 0)
            {
                LogError("Failed to set '%s' in X509 Attestation", X509_ATTESTATION_JSON_KEY_CLIENT_CERTS);
                x509Attestation_destroy(new_x509Att);
                new_x509Att = NULL;
            }
            else
            {
                new_x509Att->type = X509_CERTIFICATE_TYPE_CLIENT;
            }
        }
        else if (json_object_has_value(root_object, X509_ATTESTATION_JSON_KEY_SIGNING_CERTS))
        {
            if (json_deserialize_and_get_struct((void**)&(new_x509Att->certificates.signing_certificates), root_object, X509_ATTESTATION_JSON_KEY_SIGNING_CERTS, (FROM_JSON_FUNCTION)x509Certificates_fromJson, true) != 0)
            {
                LogError("Failed to set '%s' in X509 Attestation", X509_ATTESTATION_JSON_KEY_SIGNING_CERTS);
                x509Attestation_destroy(new_x509Att);
                new_x509Att = NULL;
            }
            else
            {
                new_x509Att->type = X509_CERTIFICATE_TYPE_SIGNING;
            }
        }
        else if (json_object_has_value(root_object, X509_ATTESTATION_JSON_KEY_CA_REFERENCES))
        {
            if (json_deserialize_and_get_struct((void**)&(new_x509Att->certificates.ca_references), root_object, X509_ATTESTATION_JSON_KEY_CA_REFERENCES, (FROM_JSON_FUNCTION)x509CAReferences_fromJson, true) != 0)
            {
                LogError("Failed to set '%s' in X509 Attestation", X509_ATTESTATION_JSON_KEY_CA_REFERENCES);
                x509Attestation_destroy(new_x509Att);
                new_x509Att = NULL;
            }
            else
            {
                new_x509Att->type = X509_CERTIFICATE_TYPE_CA_REFERENCES;
            }
        }
        else
        {
            LogError("No client or signing certificates");
            x509Attestation_destroy(new_x509Att);
            new_x509Att = NULL;
        }
    }

    return new_x509Att;
}

X509_ATTESTATION_HANDLE x509Attestation_create(X509_CERTIFICATE_TYPE cert_type, const char* primary_cert, const char* secondary_cert)
{
    X509_ATTESTATION_HANDLE new_x509Att = NULL;
    char* primary_cert_b64 = NULL;
    char* secondary_cert_b64 = NULL;

    if ((cert_type == X509_CERTIFICATE_TYPE_NONE) || (primary_cert == NULL))
    {
        LogError("Requires valid certificate type and primary certificate to create X509 Attestation");
    }
    else
    {
        if (convert_cert_to_b64(primary_cert, &primary_cert_b64) != 0)
        {
            LogError("Could not convert primary cert to Base64");
        }
        else if (convert_cert_to_b64(secondary_cert, &secondary_cert_b64) != 0)
        {
            LogError("Could not convert secondary cert to Base64");
        }
        else if ((new_x509Att = malloc(sizeof(X509_ATTESTATION))) == NULL)
        {
            LogError("Failed to allocate memory for X509 Attestation");
        }
        else
        {
            memset(new_x509Att, 0, sizeof(X509_ATTESTATION));

            new_x509Att->type = cert_type;
            if (cert_type == X509_CERTIFICATE_TYPE_CLIENT)
            {
                if ((new_x509Att->certificates.client_certificates = x509Certificates_create(primary_cert_b64, secondary_cert_b64)) == NULL)
                {
                    LogError("Failed to create Client Certificates");
                    x509Attestation_destroy(new_x509Att);
                    new_x509Att = NULL;
                }
            }
            else if (cert_type == X509_CERTIFICATE_TYPE_SIGNING)
            {
                if ((new_x509Att->certificates.signing_certificates = x509Certificates_create(primary_cert_b64, secondary_cert_b64)) == NULL)
                {
                    LogError("Failed to create Client Certificates");
                    x509Attestation_destroy(new_x509Att);
                    new_x509Att = NULL;
                }
            }
            else if (cert_type == X509_CERTIFICATE_TYPE_CA_REFERENCES)
            {
                if ((new_x509Att->certificates.ca_references = x509CAReferences_create(primary_cert_b64, secondary_cert_b64)) == NULL)
                {
                    LogError("Failed to create CA References");
                    x509Attestation_destroy(new_x509Att);
                    new_x509Att = NULL;
                }
            }
        }
        free(primary_cert_b64);
        free(secondary_cert_b64);
    }

    return new_x509Att;
}


/*Acessor Functions*/
X509_CERTIFICATE_TYPE x509Attestation_getCertificateType(X509_ATTESTATION_HANDLE x509_att)
{
    X509_CERTIFICATE_TYPE result = X509_CERTIFICATE_TYPE_NONE;

    if (x509_att == NULL)
    {
        LogError("X509 Attestation is NULL");
    }
    else
    {
        result = x509_att->type;
    }

    return result;
}

X509_CERTIFICATE_HANDLE x509Attestation_getPrimaryCertificate(X509_ATTESTATION_HANDLE x509_att)
{
    X509_CERTIFICATE_HANDLE result = NULL;

    if (x509_att == NULL)
    {
        LogError("x509 attestation is NULL");
    }
    else
    {
        if (x509_att->type == X509_CERTIFICATE_TYPE_CLIENT)
        {
            if (x509_att->certificates.client_certificates != NULL)
            {
                result = x509_att->certificates.client_certificates->primary;
            }
            else
            {
                LogError("No certificate");
            }
        }
        else if (x509_att->type == X509_CERTIFICATE_TYPE_SIGNING)
        {
            if (x509_att->certificates.signing_certificates != NULL)
            {
                result = x509_att->certificates.signing_certificates->primary;
            }
            else
            {
            LogError("No certificate");
            }
        }
        else
        {
            LogError("invalid certificate type");
        }
    }

    return result;
}

X509_CERTIFICATE_HANDLE x509Attestation_getSecondaryCertificate(X509_ATTESTATION_HANDLE x509_att)
{
    X509_CERTIFICATE_HANDLE result = NULL;

    if (x509_att == NULL)
    {
        LogError("x509 attestation is NULL");
    }
    else
    {
        if (x509_att->type == X509_CERTIFICATE_TYPE_CLIENT)
        {
            if (x509_att->certificates.client_certificates != NULL)
            {
                result = x509_att->certificates.client_certificates->secondary;
            }
            else
            {
                LogError("No certificate");
            }
        }
        else if (x509_att->type == X509_CERTIFICATE_TYPE_SIGNING)
        {
            if (x509_att->certificates.signing_certificates != NULL)
            {
                result = x509_att->certificates.signing_certificates->secondary;
            }
            else
            {
                LogError("No certificate");
            }
        }
        else
        {
            LogError("invalid certificate type");
        }
    }

    return result;
}

const char* x509Certificate_getSubjectName(X509_CERTIFICATE_HANDLE x509_cert)
{
    char* result = NULL;

    if (x509_cert == NULL)
    {
        LogError("Certificate is NULL");
    }
    else if (x509_cert->info == NULL)
    {
        LogError("Certificate has not yet been processed");
    }
    else
    {
        result = x509_cert->info->subject_name;
    }

    return result;
}

const char* x509Certificate_getSha1Thumbprint(X509_CERTIFICATE_HANDLE x509_cert)
{
    char* result = NULL;

    if (x509_cert == NULL)
    {
        LogError("Certificate is NULL");
    }
    else if (x509_cert->info == NULL)
    {
        LogError("Certificate has not yet been processed");
    }
    else
    {
        result = x509_cert->info->sha1_thumbprint;
    }

    return result;
}

const char* x509Certificate_getSha256Thumbprint(X509_CERTIFICATE_HANDLE x509_cert)
{
    char* result = NULL;

    if (x509_cert == NULL)
    {
        LogError("Certificate is NULL");
    }
    else if (x509_cert->info == NULL)
    {
        LogError("Certificate has not yet been processed");
    }
    else
    {
        result = x509_cert->info->sha256_thumbprint;
    }

    return result;
}

const char* x509Certificate_getIssuerName(X509_CERTIFICATE_HANDLE x509_cert)
{
    char* result = NULL;

    if (x509_cert == NULL)
    {
        LogError("Certificate is NULL");
    }
    else if (x509_cert->info == NULL)
    {
        LogError("Certificate has not yet been processed");
    }
    else
    {
        result = x509_cert->info->issuer_name;
    }

    return result;
}

const char* x509Certificate_getNotBeforeUtc(X509_CERTIFICATE_HANDLE x509_cert)
{
    char* result = NULL;

    if (x509_cert == NULL)
    {
        LogError("Certificate is NULL");
    }
    else if (x509_cert->info == NULL)
    {
        LogError("Certificate has not yet been processed");
    }
    else
    {
        result = x509_cert->info->not_before_utc;
    }

    return result;
}

const char* x509Certificate_getNotAfterUtc(X509_CERTIFICATE_HANDLE x509_cert)
{
    char* result = NULL;

    if (x509_cert == NULL)
    {
        LogError("Certificate is NULL");
    }
    else if (x509_cert->info == NULL)
    {
        LogError("Certificate has not yet been processed");
    }
    else
    {
        result = x509_cert->info->not_after_utc;
    }

    return result;
}

const char* x509Certificate_getSerialNumber(X509_CERTIFICATE_HANDLE x509_cert)
{
    char* result = NULL;

    if (x509_cert == NULL)
    {
        LogError("Certificate is NULL");
    }
    else if (x509_cert->info == NULL)
    {
        LogError("Certificate has not yet been processed");
    }
    else
    {
        result = x509_cert->info->serial_number;
    }

    return result;
}

int x509Certificate_getVersion(X509_CERTIFICATE_HANDLE x509_cert)
{
    int result = 0;

    if (x509_cert == NULL)
    {
        LogError("Certificate is NULL");
    }
    else if (x509_cert->info == NULL)
    {
        LogError("Certificate has not yet been processed");
    }
    else
    {
        result = x509_cert->info->version;
    }

    return result;
}
