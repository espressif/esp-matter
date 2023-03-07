// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "umock_c/umock_c_prod.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/azure_base64.h"
#include "azure_c_shared_utility/azure_base32.h"
#include "azure_c_shared_utility/urlencode.h"
#include "azure_c_shared_utility/sha.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/sastoken.h"
#include "azure_c_shared_utility/hmacsha256.h"

#include "azure_prov_client/internal/prov_auth_client.h"
#include "hsm_client_data.h"

#include "azure_prov_client/prov_security_factory.h"

typedef struct PROV_AUTH_INFO_TAG
{
    HSM_CLIENT_HANDLE hsm_client_handle;

    PROV_AUTH_TYPE sec_type;

    char* registration_id;

    HSM_CLIENT_CREATE hsm_client_create;
    HSM_CLIENT_DESTROY hsm_client_destroy;

    HSM_CLIENT_ACTIVATE_IDENTITY_KEY hsm_client_import_key;
    HSM_CLIENT_GET_ENDORSEMENT_KEY hsm_client_get_endorsement_key;
    HSM_CLIENT_GET_STORAGE_ROOT_KEY hsm_client_get_srk;
    HSM_CLIENT_SIGN_WITH_IDENTITY hsm_client_sign_data;

    HSM_CLIENT_GET_CERTIFICATE hsm_client_get_cert;
    HSM_CLIENT_GET_ALIAS_KEY hsm_client_get_alias_key;
    HSM_CLIENT_GET_COMMON_NAME hsm_client_get_common_name;

    HSM_CLIENT_GET_SYMMETRICAL_KEY hsm_client_get_symm_key;
    HSM_CLIENT_SET_SYMMETRICAL_KEY_INFO hsm_client_set_symm_key_info;
} PROV_AUTH_INFO;

static char* encode_value(const uint8_t* msg_digest, size_t digest_len)
{
    char* result;

    char* encoded_hash = Azure_Base32_Encode_Bytes(msg_digest, digest_len);
    if (encoded_hash == NULL)
    {
        LogError("Failure encoded Base32");
        result = NULL;
    }
    else
    {
        size_t encode_len = strlen(encoded_hash);
        char* iterator = encoded_hash + encode_len - 1;

        while (iterator != encoded_hash)
        {
            if (*iterator != '=')
            {
                *(iterator+1) = '\0';
                break;
            }
            iterator--;
        }

        if (mallocAndStrcpy_s(&result, encoded_hash) != 0)
        {
            LogError("Failure allocating encoded base32 result");
            result = NULL;
        }
        free(encoded_hash);
    }
    return result;
}

static int load_registration_id(PROV_AUTH_INFO* handle)
{
    int result;
    if (handle->sec_type == PROV_AUTH_TYPE_TPM)
    {
        SHA256Context sha_ctx;
        uint8_t msg_digest[SHA256HashSize];
        unsigned char* endorsement_key;
        size_t ek_len;

        if (handle->hsm_client_get_endorsement_key(handle->hsm_client_handle, &endorsement_key, &ek_len) != 0)
        {
            LogError("Failed getting device reg id");
            result = MU_FAILURE;
        }
        else
        {
            if (SHA256Reset(&sha_ctx) != 0)
            {
                LogError("Failed sha256 reset");
                result = MU_FAILURE;
            }
            else if (SHA256Input(&sha_ctx, endorsement_key, (unsigned int)ek_len) != 0)
            {
                LogError("Failed SHA256Input");
                result = MU_FAILURE;
            }
            else if (SHA256Result(&sha_ctx, msg_digest) != 0)
            {
                LogError("Failed SHA256Result");
                result = MU_FAILURE;
            }
            else
            {
                handle->registration_id = encode_value(msg_digest, SHA256HashSize);
                if (handle->registration_id == NULL)
                {
                    LogError("Failed allocating registration Id");
                    result = MU_FAILURE;
                }
                else
                {
                    result = 0;
                }
            }
            free(endorsement_key);
        }
    }
    else
    {
        handle->registration_id = handle->hsm_client_get_common_name(handle->hsm_client_handle);
        if (handle->registration_id == NULL)
        {
            LogError("Failed getting common name from certificate");
            result = MU_FAILURE;
        }
        else
        {
            result = 0;
        }
    }
    return result;
}

static int sign_sas_data(PROV_AUTH_INFO* auth_info, const char* payload, unsigned char** output, size_t* len)
{
    int result;
    size_t payload_len = strlen(payload);
    if (auth_info->sec_type == PROV_AUTH_TYPE_TPM)
    {
        if (auth_info->hsm_client_sign_data(auth_info->hsm_client_handle, (const unsigned char*)payload, strlen(payload), output, len) != 0)
        {
            LogError("Failed signing data");
            result = MU_FAILURE;
        }
        else
        {
            result = 0;
        }
    }
    else
    {
        char* symmetrical_key = auth_info->hsm_client_get_symm_key(auth_info->hsm_client_handle);
        if (symmetrical_key == NULL)
        {
            LogError("Failed getting asymmetrical key");
            result = MU_FAILURE;
        }
        else
        {
            BUFFER_HANDLE decoded_key;
            BUFFER_HANDLE output_hash;
            if ((decoded_key = Azure_Base64_Decode(symmetrical_key)) == NULL)
            {
                LogError("Failed decoding symmetrical key");
                result = MU_FAILURE;
            }
            else if ((output_hash = BUFFER_new()) == NULL)
            {
                LogError("Failed allocating output hash buffer");
                BUFFER_delete(decoded_key);
                result = MU_FAILURE;
            }
            else
            {
                size_t decoded_key_len = BUFFER_length(decoded_key);
                const unsigned char* decoded_key_bytes = BUFFER_u_char(decoded_key);

                if (HMACSHA256_ComputeHash(decoded_key_bytes, decoded_key_len, (const unsigned char*)payload, payload_len, output_hash) != HMACSHA256_OK)
                {
                    LogError("Failed computing HMAC Hash");
                    result = MU_FAILURE;
                }
                else
                {
                    *len = BUFFER_length(output_hash);
                    if ((*output = malloc(*len)) == NULL)
                    {
                        LogError("Failed allocating output buffer");
                        result = MU_FAILURE;
                    }
                    else
                    {
                        const unsigned char* output_data = BUFFER_u_char(output_hash);
                        memcpy(*output, output_data, *len);
                        result = 0;
                    }
                }
                BUFFER_delete(decoded_key);
                BUFFER_delete(output_hash);
            }
        }
        free(symmetrical_key);
    }
    return result;
}

PROV_AUTH_HANDLE prov_auth_create(void)
{
    PROV_AUTH_INFO* result;
    /* Codes_SRS_PROV_AUTH_CLIENT_07_001: [ prov_auth_create shall allocate the PROV_AUTH_INFO. ] */
    if ((result = (PROV_AUTH_INFO*)malloc(sizeof(PROV_AUTH_INFO))) == NULL)
    {
        LogError("Failed allocating PROV_AUTH_INFO.");
    }
    else
    {
        memset(result, 0, sizeof(PROV_AUTH_INFO) );
        SECURE_DEVICE_TYPE sec_type = prov_dev_security_get_type();
#if defined(HSM_TYPE_SAS_TOKEN)  || defined(HSM_AUTH_TYPE_CUSTOM)
        if (sec_type == SECURE_DEVICE_TYPE_TPM)
        {
            /* Codes_SRS_PROV_AUTH_CLIENT_07_003: [ prov_auth_create shall validate the specified secure enclave interface to ensure. ] */
            result->sec_type = PROV_AUTH_TYPE_TPM;
            const HSM_CLIENT_TPM_INTERFACE* tpm_interface = hsm_client_tpm_interface();
            if ((tpm_interface == NULL) ||
                ((result->hsm_client_create = tpm_interface->hsm_client_tpm_create) == NULL) ||
                ((result->hsm_client_destroy = tpm_interface->hsm_client_tpm_destroy) == NULL) ||
                ((result->hsm_client_import_key = tpm_interface->hsm_client_activate_identity_key) == NULL) ||
                ((result->hsm_client_get_endorsement_key = tpm_interface->hsm_client_get_ek) == NULL) ||
                ((result->hsm_client_get_srk = tpm_interface->hsm_client_get_srk) == NULL) ||
                ((result->hsm_client_sign_data = tpm_interface->hsm_client_sign_with_identity) == NULL)
                )
            {
                /* Codes_SRS_PROV_AUTH_CLIENT_07_002: [ If any failure is encountered prov_auth_create shall return NULL ] */
                LogError("Invalid TPM secure device interface was specified");
                free(result);
                result = NULL;
            }
        }
#endif
#if defined(HSM_TYPE_X509) || defined(HSM_AUTH_TYPE_CUSTOM)
        if (sec_type == SECURE_DEVICE_TYPE_X509)
        {
            /* Codes_SRS_PROV_AUTH_CLIENT_07_003: [ prov_auth_create shall validate the specified secure enclave interface to ensure. ] */
            result->sec_type = PROV_AUTH_TYPE_X509;
            const HSM_CLIENT_X509_INTERFACE* x509_interface = hsm_client_x509_interface();
            if ((x509_interface == NULL) ||
                ((result->hsm_client_create = x509_interface->hsm_client_x509_create) == NULL) ||
                ((result->hsm_client_destroy = x509_interface->hsm_client_x509_destroy) == NULL) ||
                ((result->hsm_client_get_cert = x509_interface->hsm_client_get_cert) == NULL) ||
                ((result->hsm_client_get_common_name = x509_interface->hsm_client_get_common_name) == NULL) ||
                ((result->hsm_client_get_alias_key = x509_interface->hsm_client_get_key) == NULL)
                )
            {
                LogError("Invalid x509 secure device interface was specified");
                free(result);
                result = NULL;
            }
        }
#endif
#if defined(HSM_TYPE_SYMM_KEY) || defined(HSM_AUTH_TYPE_CUSTOM)
        if (sec_type == SECURE_DEVICE_TYPE_SYMMETRIC_KEY)
        {
            result->sec_type = PROV_AUTH_TYPE_KEY;
            const HSM_CLIENT_KEY_INTERFACE* key_interface = hsm_client_key_interface();
            if ((key_interface == NULL) ||
                ((result->hsm_client_create = key_interface->hsm_client_key_create) == NULL) ||
                ((result->hsm_client_destroy = key_interface->hsm_client_key_destroy) == NULL) ||
                ((result->hsm_client_get_common_name = key_interface->hsm_client_get_registration_name) == NULL) ||
                ((result->hsm_client_get_symm_key = key_interface->hsm_client_get_symm_key) == NULL) ||
                ((result->hsm_client_set_symm_key_info = key_interface->hsm_client_set_symm_key_info) == NULL)
                )
            {
                LogError("Invalid symmetric key secure device interface was specified");
                free(result);
                result = NULL;
            }
        }
#endif

        if (result == NULL)
        {
            LogError("Error allocating result or else unsupported security type %d", sec_type);
        }
        else if (result->hsm_client_create == NULL)
        {
            LogError("hsm_client_create is not a valid address");
            free(result);
            result = NULL;
        }
        else
        {
            /* Codes_SRS_PROV_AUTH_CLIENT_07_004: [ prov_auth_create shall call hsm_client_create on the secure enclave interface. ] */
            if ((result->hsm_client_handle = result->hsm_client_create() ) == NULL)
            {
                LogError("failed create device auth module.");
                free(result);
                result = NULL;
            }
            else if (result->sec_type == PROV_AUTH_TYPE_KEY && result->hsm_client_set_symm_key_info(result->hsm_client_handle, prov_dev_get_symm_registration_name(), prov_dev_get_symmetric_key()) != 0)
            {
                LogError("failed create device auth module.");
                result->hsm_client_destroy(result->hsm_client_handle);
                free(result);
                result = NULL;
            }
        }
    }
    return result;
}

void prov_auth_destroy(PROV_AUTH_HANDLE handle)
{
    /* Codes_SRS_PROV_AUTH_CLIENT_07_005: [ if handle is NULL, prov_auth_destroy shall do nothing. ] */
    if (handle != NULL)
    {
        /* Codes_SRS_PROV_AUTH_CLIENT_07_007: [ prov_auth_destroy shall free all resources allocated in this module. ] */
        free(handle->registration_id);
        handle->hsm_client_destroy(handle->hsm_client_handle);
        /* Codes_SRS_PROV_AUTH_CLIENT_07_006: [ prov_auth_destroy shall free the PROV_AUTH_HANDLE instance. ] */
        free(handle);
    }
}

PROV_AUTH_TYPE prov_auth_get_type(PROV_AUTH_HANDLE handle)
{
    PROV_AUTH_TYPE result;
    /* Codes_SRS_PROV_AUTH_CLIENT_07_008: [ if handle is NULL prov_auth_get_type shall return PROV_AUTH_TYPE_UNKNOWN ] */
    if (handle == NULL)
    {
        LogError("Invalid handle specified");
        result = PROV_AUTH_TYPE_UNKNOWN;
    }
    else
    {
        /* Codes_SRS_PROV_AUTH_CLIENT_07_009: [ prov_auth_get_type shall return the PROV_AUTH_TYPE of the underlying secure enclave. ] */
        result = handle->sec_type;
    }
    return result;
}

char* prov_auth_get_registration_id(PROV_AUTH_HANDLE handle)
{
    char* result;
    if (handle == NULL)
    {
        /* Codes_SRS_PROV_AUTH_CLIENT_07_010: [ if handle is NULL prov_auth_get_registration_id shall return NULL. ] */
        LogError("Invalid handle parameter");
        result = NULL;
    }
    else
    {
        int load_result = 0;
        int reg_id_allocated = 0;
        if (handle->registration_id == NULL)
        {
            /* Codes_SRS_PROV_AUTH_CLIENT_07_011: [ prov_auth_get_registration_id shall load the registration id if it has not been previously loaded. ] */
            load_result = load_registration_id(handle);
            if (load_result == 0)
            {
                reg_id_allocated = 1;
            }
        }

        if (load_result != 0)
        {
            /* Codes_SRS_PROV_AUTH_CLIENT_07_012: [ If a failure is encountered, prov_auth_get_registration_id shall return NULL. ] */
            LogError("Failed loading registration key");
            result = NULL;
        }
        /* Codes_SRS_PROV_AUTH_CLIENT_07_013: [ Upon success prov_auth_get_registration_id shall return the registration id of the secure enclave. ] */
        else if (mallocAndStrcpy_s(&result, handle->registration_id) != 0)
        {
            /* Codes_SRS_PROV_AUTH_CLIENT_07_012: [ If a failure is encountered, prov_auth_get_registration_id shall return NULL. ] */
            LogError("Failed allocating registration key");
            if (reg_id_allocated == 1)
            {
                free(handle->registration_id);
                handle->registration_id = NULL;
            }
            result = NULL;
        }
    }
    return result;
}

int prov_auth_set_registration_id(PROV_AUTH_HANDLE handle, const char* registration_id)
{
    int result;
    if (handle == NULL || registration_id == NULL)
    {
        LogError("Invalid parameter handle: %p, registration_id: %p", handle, registration_id);
        result = MU_FAILURE;
    }
    else
    {
        if (handle->registration_id != NULL)
        {
            LogError("Registration_id has been previously set, registration can not be changed");
            result = MU_FAILURE;
        }
        else if (mallocAndStrcpy_s(&handle->registration_id, registration_id) != 0)
        {
            LogError("Failed allocating registration key");
            result = MU_FAILURE;
        }
        else
        {
            result = 0;
        }
    }
    return result;
}

BUFFER_HANDLE prov_auth_get_endorsement_key(PROV_AUTH_HANDLE handle)
{
    BUFFER_HANDLE result;
    if (handle == NULL)
    {
        /* Codes_SRS_SECURE_ENCLAVE_CLIENT_07_021: [ If handle is NULL prov_auth_get_endorsement_key shall return NULL. ] */
        LogError("Invalid handle parameter");
        result = NULL;
    }
    else if (handle->sec_type != PROV_AUTH_TYPE_TPM)
    {
        /* Codes_SRS_SECURE_ENCLAVE_CLIENT_07_023: [ If the sec_type is PROV_AUTH_TYPE_X509, prov_auth_get_endorsement_key shall return NULL. ] */
        LogError("Invalid type for operation");
        result = NULL;
    }
    else
    {
        unsigned char* ek_value;
        size_t ek_len;

        if (handle->hsm_client_get_endorsement_key(handle->hsm_client_handle, &ek_value, &ek_len) != 0)
        {
            LogError("Failed getting endorsement key");
            result = NULL;
        }
        else
        {
            /* Codes_SRS_SECURE_ENCLAVE_CLIENT_07_022: [ prov_auth_get_endorsement_key shall return the endorsement key returned by the hsm_client_get_ek secure enclave function. ] */
            result = BUFFER_create(ek_value, ek_len);
            if (result == NULL)
            {
                LogError("Failed creating BUFFER HANDLE");
            }
            free(ek_value);
        }
    }
    return result;
}

BUFFER_HANDLE prov_auth_get_storage_key(PROV_AUTH_HANDLE handle)
{
    BUFFER_HANDLE result;
    /* Codes_SRS_SECURE_ENCLAVE_CLIENT_07_024: [ If handle is NULL prov_auth_get_storage_key shall return NULL. ] */
    if (handle == NULL)
    {
        LogError("Invalid handle parameter");
        result = NULL;
    }
    else if (handle->sec_type != PROV_AUTH_TYPE_TPM)
    {
        /* Codes_SRS_SECURE_ENCLAVE_CLIENT_07_026: [ If the sec_type is PROV_AUTH_TYPE_X509, prov_auth_get_storage_key shall return NULL. ] */
        LogError("Invalid type for operation");
        result = NULL;
    }
    else
    {
        unsigned char* srk_value;
        size_t srk_len;

        if (handle->hsm_client_get_srk(handle->hsm_client_handle, &srk_value, &srk_len) != 0)
        {
            LogError("Failed getting storage root key");
            result = NULL;
        }
        else
        {
            /* Codes_SRS_SECURE_ENCLAVE_CLIENT_07_025: [ prov_auth_get_storage_key shall return the endorsement key returned by the hsm_client_get_srk secure enclave function. ] */
            result = BUFFER_create(srk_value, srk_len);
            if (result == NULL)
            {
                LogError("Failed creating BUFFER HANDLE");
            }
            free(srk_value);
        }
    }
    return result;
}

int prov_auth_import_key(PROV_AUTH_HANDLE handle, const unsigned char* key_value, size_t key_len)
{
    int result;
    if (handle == NULL || key_value == NULL || key_len == 0)
    {
        /* Codes_SRS_SECURE_ENCLAVE_CLIENT_07_027: [ If handle or key are NULL prov_auth_import_key shall return a non-zero value. ] */
        LogError("Invalid handle parameter");
        result = MU_FAILURE;
    }
    else if (handle->sec_type != PROV_AUTH_TYPE_TPM)
    {
        /* Codes_SRS_SECURE_ENCLAVE_CLIENT_07_029: [ If the sec_type is not SECURE_ENCLAVE_TYPE_TPM, prov_auth_import_key shall return NULL. ] */
        LogError("Invalid type for operation");
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_SECURE_ENCLAVE_CLIENT_07_028: [ prov_auth_import_key shall import the specified key into the tpm using hsm_client_import_key secure enclave function. ] */
        if (handle->hsm_client_import_key(handle->hsm_client_handle, key_value, key_len) != 0)
        {
            /* SRS_SECURE_ENCLAVE_CLIENT_07_040: [ If hsm_client_import_key returns an error prov_auth_import_key shall return a non-zero value. ]*/
            LogError("failure importing key into tpm");
            result = MU_FAILURE;
        }
        else
        {
            result = 0;
        }
    }
    return result;
}

char* prov_auth_construct_sas_token(PROV_AUTH_HANDLE handle, const char* token_scope, const char* key_name, size_t expiry_time)
{
    char* result;
    char expire_token[64] = { 0 };

    if (handle == NULL || token_scope == NULL || key_name == NULL)
    {
        LogError("Invalid handle parameter handle: %p, token_scope: %p, key_name: %p", handle, token_scope, key_name);
        result = NULL;
    }
    else if (handle->sec_type == PROV_AUTH_TYPE_X509)
    {
        LogError("Invalid type for operation");
        result = NULL;
    }
    else if (size_tToString(expire_token, sizeof(expire_token), expiry_time) != 0)
    {
        result = NULL;
        LogError("Failure creating expire token");
    }
    else
    {
        size_t len = strlen(token_scope) + strlen(expire_token) + 3;
        char* payload = malloc(len + 1);
        if (payload == NULL)
        {
            result = NULL;
            LogError("Failure allocating payload for sas token.");
        }
        else
        {
            unsigned char* data_value;
            size_t data_len;
            (void)sprintf(payload, "%s\n%s", token_scope, expire_token);

            /* Codes_SRS_SECURE_ENCLAVE_CLIENT_07_031: [ prov_auth_get_certificate shall import the specified cert into the client using hsm_client_get_cert secure enclave function. ] */
            if (sign_sas_data(handle, payload, &data_value, &data_len) == 0)
            {
                STRING_HANDLE urlEncodedSignature;
                STRING_HANDLE base64Signature;
                STRING_HANDLE sas_token_handle;
                if ((base64Signature = Azure_Base64_Encode_Bytes(data_value, data_len)) == NULL)
                {
                    result = NULL;
                    LogError("Failure constructing base64 encoding.");
                }
                else if ((urlEncodedSignature = URL_Encode(base64Signature)) == NULL)
                {
                    result = NULL;
                    LogError("Failure constructing url Signature.");
                    STRING_delete(base64Signature);
                }
                else
                {
                    sas_token_handle = STRING_construct_sprintf("SharedAccessSignature sr=%s&sig=%s&se=%s&skn=%s", token_scope, STRING_c_str(urlEncodedSignature), expire_token, key_name);
                    if (sas_token_handle == NULL)
                    {
                        result = NULL;
                        LogError("Failure constructing url Signature.");
                    }
                    else
                    {
                        const char* temp_sas_token = STRING_c_str(sas_token_handle);
                        if (mallocAndStrcpy_s(&result, temp_sas_token) != 0)
                        {
                            LogError("Failure allocating and copying string.");
                            result = NULL;
                        }
                        STRING_delete(sas_token_handle);
                    }
                    STRING_delete(base64Signature);
                    STRING_delete(urlEncodedSignature);
                }
                free(data_value);
            }
            else
            {
                result = NULL;
                LogError("Failure generate sas token.");
            }
            free(payload);
        }
    }
    return result;
}

char* prov_auth_get_certificate(PROV_AUTH_HANDLE handle)
{
    char* result;
    if (handle == NULL)
    {
        /* Codes_SRS_SECURE_ENCLAVE_CLIENT_07_030: [ If handle or key are NULL prov_auth_get_certificate shall return a non-zero value. ] */
        LogError("Invalid handle parameter");
        result = NULL;
    }
    else if (handle->sec_type != PROV_AUTH_TYPE_X509)
    {
        /* Codes_SRS_SECURE_ENCLAVE_CLIENT_07_032: [ If the sec_type is not PROV_AUTH_TYPE_X509, prov_auth_get_certificate shall return NULL. ] */
        LogError("Invalid type for operation");
        result = NULL;
    }
    else
    {
        /* Codes_SRS_SECURE_ENCLAVE_CLIENT_07_031: [ prov_auth_get_certificate shall import the specified cert into the client using hsm_client_get_cert secure enclave function. ] */
        result = handle->hsm_client_get_cert(handle->hsm_client_handle);
    }
    return result;
}

char* prov_auth_get_alias_key(PROV_AUTH_HANDLE handle)
{
    char* result;
    if (handle == NULL)
    {
        /* Codes_SRS_SECURE_ENCLAVE_CLIENT_07_033: [ If handle or key are NULL prov_auth_get_alias_key shall return a non-zero value. ] */
        LogError("Invalid handle parameter");
        result = NULL;
    }
    else if (handle->sec_type != PROV_AUTH_TYPE_X509)
    {
        /* Codes_SRS_SECURE_ENCLAVE_CLIENT_07_035: [ If the sec_type is not PROV_AUTH_TYPE_X509, prov_auth_get_alias_key shall return NULL. ] */
        LogError("Invalid type for operation");
        result = NULL;
    }
    else
    {
        /* Codes_SRS_SECURE_ENCLAVE_CLIENT_07_034: [ prov_auth_get_alias_key shall import the specified alias key into the client using hsm_client_get_ak secure enclave function. ] */
        result = handle->hsm_client_get_alias_key(handle->hsm_client_handle);
    }
    return result;
}
