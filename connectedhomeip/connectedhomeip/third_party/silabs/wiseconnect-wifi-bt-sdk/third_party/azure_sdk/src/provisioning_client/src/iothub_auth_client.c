// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "umock_c/umock_c_prod.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/sastoken.h"
#include "azure_c_shared_utility/azure_base64.h"
#include "azure_c_shared_utility/sha.h"
#include "azure_c_shared_utility/urlencode.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/hmacsha256.h"

#include "azure_prov_client/internal/iothub_auth_client.h"
#include "azure_prov_client/iothub_security_factory.h"
#include "hsm_client_data.h"

typedef struct IOTHUB_SECURITY_INFO_TAG
{
    DEVICE_AUTH_TYPE cred_type;

    HSM_CLIENT_HANDLE hsm_client_handle;

    HSM_CLIENT_CREATE hsm_client_create;
    HSM_CLIENT_DESTROY hsm_client_destroy;

    HSM_CLIENT_SIGN_WITH_IDENTITY hsm_client_sign_data;

    HSM_CLIENT_GET_CERTIFICATE hsm_client_get_cert;
    HSM_CLIENT_GET_ALIAS_KEY hsm_client_get_alias_key;

    HSM_CLIENT_GET_TRUST_BUNDLE hsm_client_get_trust_bundle;
    HSM_CLIENT_GET_SYMMETRICAL_KEY hsm_client_get_symm_key;
    HSM_CLIENT_SET_SYMMETRICAL_KEY_INFO hsm_client_set_symm_key_info;

    char* sas_token;
    char* x509_certificate;
    char* x509_alias_key;
    bool base64_encode_signature;
    bool urlencode_token_scope;
} IOTHUB_SECURITY_INFO;

#define HMAC_LENGTH                 32
static const char* const SAS_TOKEN_FORMAT = "SharedAccessSignature sr=%s&sig=%s&se=%s%s%s";
static const char* const SKN_SECTION_FORMAT = "&skn=";

static int sign_sas_data(IOTHUB_SECURITY_INFO* security_info, const char* payload, unsigned char** output, size_t* len)
{
    int result;
    size_t payload_len = strlen(payload);
    if (security_info->cred_type == AUTH_TYPE_SAS)
    {
        if (security_info->hsm_client_sign_data(security_info->hsm_client_handle, (const unsigned char*)payload, strlen(payload), output, len) != 0)
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
        char* symmetrical_key = security_info->hsm_client_get_symm_key(security_info->hsm_client_handle);
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
                if (HMACSHA256_ComputeHash(BUFFER_u_char(decoded_key), BUFFER_length(decoded_key), (const unsigned char*)payload, payload_len, output_hash) != HMACSHA256_OK)
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
            free(symmetrical_key);
        }
    }
    return result;
}

IOTHUB_SECURITY_HANDLE iothub_device_auth_create()
{
    IOTHUB_SECURITY_INFO* result;

    IOTHUB_SECURITY_TYPE iothub_security_t = iothub_security_type();

    if ((result = (IOTHUB_SECURITY_INFO*)malloc(sizeof(IOTHUB_SECURITY_INFO))) == NULL)
    {
        /* Codes_IOTHUB_DEV_AUTH_07_001: [ if any failure is encountered iothub_device_auth_create shall return NULL. ] */
        LogError("Failed allocating IOTHUB_SECURITY_INFO.");
    }
    else
    {
        memset(result, 0, sizeof(IOTHUB_SECURITY_INFO) );
#if defined(HSM_TYPE_SAS_TOKEN)  || defined(HSM_AUTH_TYPE_CUSTOM)
        if (iothub_security_t == IOTHUB_SECURITY_TYPE_SAS)
        {
            result->cred_type = AUTH_TYPE_SAS;
            result->base64_encode_signature = true;
            result->urlencode_token_scope = false;
            const HSM_CLIENT_TPM_INTERFACE* tpm_interface = hsm_client_tpm_interface();
            if ((tpm_interface == NULL) ||
                ((result->hsm_client_create = tpm_interface->hsm_client_tpm_create) == NULL) ||
                ((result->hsm_client_destroy = tpm_interface->hsm_client_tpm_destroy) == NULL) ||
                ((result->hsm_client_sign_data = tpm_interface->hsm_client_sign_with_identity) == NULL)
                )
            {
                /* Codes_IOTHUB_DEV_AUTH_07_034: [ if any of the iothub_security_interface function are NULL iothub_device_auth_create shall return NULL. ] */
                LogError("Invalid secure device interface");
                free(result);
                result = NULL;
            }
        }
#endif
#if defined(HSM_TYPE_X509) || defined(HSM_AUTH_TYPE_CUSTOM)
        if (result != NULL && iothub_security_t == IOTHUB_SECURITY_TYPE_X509)
        {
            result->cred_type = AUTH_TYPE_X509;
            result->base64_encode_signature = true;
            result->urlencode_token_scope = false;
            const HSM_CLIENT_X509_INTERFACE* x509_interface = hsm_client_x509_interface();
            if ((x509_interface == NULL) ||
                ((result->hsm_client_create = x509_interface->hsm_client_x509_create) == NULL) ||
                ((result->hsm_client_destroy = x509_interface->hsm_client_x509_destroy) == NULL) ||
                ((result->hsm_client_get_cert = x509_interface->hsm_client_get_cert) == NULL) ||
                ((result->hsm_client_get_alias_key = x509_interface->hsm_client_get_key) == NULL)
                )
            {
                /* Codes_IOTHUB_DEV_AUTH_07_034: [ if any of the iothub_security_interface function are NULL iothub_device_auth_create shall return NULL. ] */
                LogError("Invalid handle parameter: DEVICE_AUTH_CREATION_INFO is NULL");
                free(result);
                result = NULL;
            }
        }
#endif
#if defined(HSM_TYPE_SYMM_KEY) || defined(HSM_AUTH_TYPE_CUSTOM)
        if (result != NULL && iothub_security_t == IOTHUB_SECURITY_TYPE_SYMMETRIC_KEY)
        {
            result->cred_type = AUTH_TYPE_SYMM_KEY;
            result->base64_encode_signature = true;
            result->urlencode_token_scope = false;
            const HSM_CLIENT_KEY_INTERFACE* key_interface = hsm_client_key_interface();
            if ((key_interface == NULL) ||
                ((result->hsm_client_create = key_interface->hsm_client_key_create) == NULL) ||
                ((result->hsm_client_destroy = key_interface->hsm_client_key_destroy) == NULL) ||
                ((result->hsm_client_get_symm_key = key_interface->hsm_client_get_symm_key) == NULL) ||
                ((result->hsm_client_set_symm_key_info = key_interface->hsm_client_set_symm_key_info) == NULL)
                )
            {
                LogError("Invalid x509 secure device interface was specified");
                free(result);
                result = NULL;
            }
        }
#endif
#ifdef HSM_TYPE_HTTP_EDGE
        if (result != NULL && iothub_security_t == IOTHUB_SECURITY_TYPE_HTTP_EDGE)
        {
            result->cred_type = AUTH_TYPE_SAS;
            // Because HTTP_edge operates over HTTP, the server has already base64 encoded signature its returning to us.
            result->base64_encode_signature = false;
            result->urlencode_token_scope = true;
            const HSM_CLIENT_HTTP_EDGE_INTERFACE* http_edge_interface = hsm_client_http_edge_interface();
            if ((http_edge_interface == NULL) ||
                ((result->hsm_client_create = http_edge_interface->hsm_client_http_edge_create) == NULL) ||
                ((result->hsm_client_destroy = http_edge_interface->hsm_client_http_edge_destroy) == NULL) ||
                ((result->hsm_client_sign_data = http_edge_interface->hsm_client_sign_with_identity) == NULL) ||
                ((result->hsm_client_get_trust_bundle = http_edge_interface->hsm_client_get_trust_bundle) == NULL))
            {
                LogError("Invalid secure device interface");
                free(result);
                result = NULL;
            }
        }
#endif
        if (result == NULL)
        {
            LogError("Error allocating result or else unsupported security type %d", iothub_security_t);
        }
        else if (result->hsm_client_create == NULL)
        {
            LogError("hsm_client_create is not a valid address");
            free(result);
            result = NULL;
        }
        else
        {
            /* Codes_IOTHUB_DEV_AUTH_07_025: [ iothub_device_auth_create shall call the concrete_iothub_device_auth_create function associated with the interface_desc. ] */
            /* Codes_IOTHUB_DEV_AUTH_07_026: [ if concrete_iothub_device_auth_create fails iothub_device_auth_create shall return NULL. ] */
            if ((result->hsm_client_handle = result->hsm_client_create()) == NULL)
            {
                /* Codes_IOTHUB_DEV_AUTH_07_002: [ iothub_device_auth_create shall allocate the IOTHUB_SECURITY_INFO and shall fail if the allocation fails. ]*/
                LogError("failed create device auth module.");
                free(result);
                result = NULL;
            }
            else if (result->cred_type == AUTH_TYPE_SYMM_KEY && result->hsm_client_set_symm_key_info(result->hsm_client_handle, iothub_security_get_symm_registration_name(), iothub_security_get_symmetric_key()) != 0)
            {
                LogError("Invalid x509 secure device interface was specified");
                result->hsm_client_destroy(result->hsm_client_handle);
                free(result);
                result = NULL;
            }
        }
    }
    /* Codes_IOTHUB_DEV_AUTH_07_003: [ If the function succeeds iothub_device_auth_create shall return a IOTHUB_SECURITY_HANDLE. ] */
    return result;
}

void iothub_device_auth_destroy(IOTHUB_SECURITY_HANDLE handle)
{
    /* Codes_IOTHUB_DEV_AUTH_07_006: [ If the argument handle is NULL, iothub_device_auth_destroy shall do nothing ] */
    if (handle != NULL)
    {
        /* Codes_IOTHUB_DEV_AUTH_07_005: [ iothub_device_auth_destroy shall call the concrete_iothub_device_auth_destroy function associated with the XDA_INTERFACE_DESCRIPTION. ] */
        free(handle->x509_certificate);
        free(handle->x509_alias_key);
        free(handle->sas_token);
        handle->hsm_client_destroy(handle->hsm_client_handle);
        /* Codes_IOTHUB_DEV_AUTH_07_004: [ iothub_device_auth_destroy shall free all resources associated with the IOTHUB_SECURITY_HANDLE handle ] */
        free(handle);
    }
}

DEVICE_AUTH_TYPE iothub_device_auth_get_type(IOTHUB_SECURITY_HANDLE handle)
{
    DEVICE_AUTH_TYPE result;
    /* Codes_IOTHUB_DEV_AUTH_07_007: [ If the argument handle is NULL, iothub_device_auth_get_auth_type shall return AUTH_TYPE_UNKNOWN. ] */
    if (handle == NULL)
    {
        LogError("Invalid handle specified");
        result = AUTH_TYPE_UNKNOWN;
    }
    else
    {
        /* Codes_IOTHUB_DEV_AUTH_07_008: [ iothub_device_auth_get_auth_type shall call concrete_iothub_device_auth_type function associated with the XDA_INTERFACE_DESCRIPTION. ] */
        /* Codes_IOTHUB_DEV_AUTH_07_009: [ iothub_device_auth_get_auth_type shall return the DEVICE_AUTH_TYPE returned by the concrete_iothub_device_auth_type function. ] */
        result = handle->cred_type;
    }
    return result;
}

CREDENTIAL_RESULT* iothub_device_auth_generate_credentials(IOTHUB_SECURITY_HANDLE handle, const DEVICE_AUTH_CREDENTIAL_INFO* dev_auth_cred)
{
    CREDENTIAL_RESULT* result;
    /* Codes_IOTHUB_DEV_AUTH_07_010: [ If the argument handle or dev_auth_cred is NULL, iothub_device_auth_generate_credentials shall return a NULL value. ] */
    if (handle == NULL)
    {
        LogError("Invalid handle parameter: handle");
        result = NULL;
    }
    else
    {
        if (handle->cred_type == AUTH_TYPE_SAS || handle->cred_type == AUTH_TYPE_SYMM_KEY)
        {
            char expire_token[64] = { 0 };
            if (handle->sas_token != NULL)
            {
                free(handle->sas_token);
                handle->sas_token = NULL;
            }
            if (dev_auth_cred == NULL || dev_auth_cred->sas_info.token_scope == NULL)
            {
                LogError("Invalid handle parameter: dev_auth_cred");
                result = NULL;
            }
            else if (dev_auth_cred->dev_auth_type != AUTH_TYPE_SAS && dev_auth_cred->dev_auth_type != AUTH_TYPE_SYMM_KEY)
            {
                LogError("Invalid handle parameter: dev_auth_cred.dev_auth_type");
                result = NULL;
            }
            else if (size_tToString(expire_token, sizeof(expire_token), (size_t)dev_auth_cred->sas_info.expiry_seconds) != 0)
            {
                result = NULL;
                LogError("Failure creating expire token");
            }
            else
            {
                size_t len = strlen(dev_auth_cred->sas_info.token_scope)+strlen(expire_token)+3;
                char* payload = malloc(len+1);
                if (payload == NULL)
                {
                    result = NULL;
                    LogError("Failure allocating payload.");
                }
                else
                {
                    unsigned char* data_value;
                    size_t data_len;

                    size_t total_len = sprintf(payload, "%s\n%s", dev_auth_cred->sas_info.token_scope, expire_token);
                    if (total_len <= 0)
                    {
                        result = NULL;
                        LogError("Failure constructing hash payload.");
                    }
                    /* Codes_IOTHUB_DEV_AUTH_07_035: [ For tpm type iothub_device_auth_generate_credentials shall call the concrete_dev_auth_sign_data function to hash the data. ] */
                    else if (sign_sas_data(handle, payload, &data_value, &data_len) == 0)
                    {
                        STRING_HANDLE urlEncodedSignature = NULL;
                        STRING_HANDLE signature = NULL;
                        if (handle->base64_encode_signature == true)
                        {
                            signature = Azure_Base64_Encode_Bytes(data_value, data_len);
                        }
                        else
                        {
                            signature = STRING_construct((const char*)data_value);
                        }

                        if (signature == NULL)
                        {
                            result = NULL;
                            LogError("Failure constructing encoding.");
                        }
                        else if ((urlEncodedSignature = URL_Encode(signature)) == NULL)
                        {
                            result = NULL;
                            LogError("Failure constructing url Signature.");
                            STRING_delete(signature);
                        }
                        else
                        {
                            const char* skn_key = "";
                            const char* skn_value = "";

                            if ((dev_auth_cred->sas_info.key_name != NULL) && (strlen(dev_auth_cred->sas_info.key_name) > 0))
                            {
                                // If the key name is valid then add to the sas token
                                skn_key = SKN_SECTION_FORMAT;
                                skn_value = dev_auth_cred->sas_info.key_name;
                            }

                            STRING_HANDLE sas_token_handle = NULL;

                            if (handle->urlencode_token_scope == true)
                            {
                                STRING_HANDLE url_encoded = URL_EncodeString(dev_auth_cred->sas_info.token_scope);
                                if (url_encoded == NULL)
                                {
                                    LogError("failed to url string %s", dev_auth_cred->sas_info.token_scope);
                                }
                                else
                                {
                                    sas_token_handle = STRING_construct_sprintf(SAS_TOKEN_FORMAT, STRING_c_str(url_encoded), STRING_c_str(urlEncodedSignature), expire_token, skn_key, skn_value);
                                }

                                STRING_delete(url_encoded);
                            }
                            else
                            {
                                sas_token_handle = STRING_construct_sprintf(SAS_TOKEN_FORMAT, dev_auth_cred->sas_info.token_scope, STRING_c_str(urlEncodedSignature), expire_token, skn_key, skn_value);
                            }

                            if (sas_token_handle == NULL)
                            {
                                result = NULL;
                                LogError("Failure constructing url Signature.");
                            }
                            else if ((result = malloc(sizeof(CREDENTIAL_RESULT))) == NULL)
                            {
                                STRING_delete(sas_token_handle);
                                LogError("Failure allocating credential result.");
                            }
                            else
                            {
                                const char* temp_sas_token = STRING_c_str(sas_token_handle);
                                if (mallocAndStrcpy_s(&handle->sas_token, temp_sas_token) != 0)
                                {
                                    free(result);
                                    result = NULL;
                                    LogError("Failure allocating and copying string.");
                                }
                                else
                                {
                                    result->auth_cred_result.sas_result.sas_token = handle->sas_token;
                                }
                                STRING_delete(sas_token_handle);
                            }
                            STRING_delete(signature);
                            STRING_delete(urlEncodedSignature);
                        }
                        free(data_value);
                    }
                    else
                    {
                        result = NULL;
                        LogError("Failure generate hash from tpm.");
                    }
                    free(payload);
                }
            }
        }
        else
        {
            if (handle->x509_certificate != NULL)
            {
                free(handle->x509_certificate);
                handle->x509_certificate = NULL;
            }
            if (handle->x509_alias_key != NULL)
            {
                free(handle->x509_alias_key);
                handle->x509_alias_key = NULL;
            }

            if ((result = malloc(sizeof(CREDENTIAL_RESULT))) == NULL)
            {
                LogError("Failure allocating credential result.");
            }
            else if ((handle->x509_certificate = handle->hsm_client_get_cert(handle->hsm_client_handle)) == NULL)
            {
                LogError("Failure allocating device credential result.");
                free(result);
                result = NULL;
            }
            else if ((handle->x509_alias_key = handle->hsm_client_get_alias_key(handle->hsm_client_handle)) == NULL)
            {
                LogError("Failure allocating device credential result.");
                free(handle->x509_certificate);
                handle->x509_certificate = NULL;
                free(result);
                result = NULL;
            }
            else
            {
                result->auth_cred_result.x509_result.x509_cert = handle->x509_certificate;
                result->auth_cred_result.x509_result.x509_alias_key = handle->x509_alias_key;
            }
        }
    }
    return result;
}

#ifdef USE_EDGE_MODULES
char* iothub_device_auth_get_trust_bundle(IOTHUB_SECURITY_HANDLE handle)
{
    if (handle->hsm_client_get_trust_bundle == NULL)
    {
        LogError("This authentication type does not support getting trusted certificates");
        return NULL;
    }
    return handle->hsm_client_get_trust_bundle(handle->hsm_client_handle);
}
#endif
