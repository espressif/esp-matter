// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_macro_utils/macro_utils.h"
#include "umock_c/umock_c_prod.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/agenttime.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/sastoken.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/azure_base64.h"
#include "azure_c_shared_utility/buffer_.h"

#ifdef USE_PROV_MODULE
#include "azure_prov_client/internal/iothub_auth_client.h"
#endif

#include "internal/iothub_client_authorization.h"

#define DEFAULT_SAS_TOKEN_EXPIRY_TIME_SECS          3600
#define INDEFINITE_TIME                             ((time_t)(-1))
#define MIN_SAS_EXPIRY_TIME                         5  // 5 seconds

typedef struct IOTHUB_AUTHORIZATION_DATA_TAG
{
    char* device_sas_token;
    char* device_key;
    char* device_id;
    char* module_id;
    uint64_t token_expiry_time_sec;
    IOTHUB_CREDENTIAL_TYPE cred_type;
#ifdef USE_PROV_MODULE
    IOTHUB_SECURITY_HANDLE device_auth_handle;
#endif
} IOTHUB_AUTHORIZATION_DATA;

static int get_seconds_since_epoch(uint64_t* seconds)
{
    int result;
    uint32_t current_time;
    if ((current_time = get_time(NULL)) == INDEFINITE_TIME)
    {
        LogError("Failed getting the current local time (get_time() failed)");
        result = MU_FAILURE;
    }
    else
    {
        *seconds = (uint64_t)get_difftime(current_time, (time_t)0);
        result = 0;
    }
    return result;
}

static IOTHUB_AUTHORIZATION_DATA* initialize_auth_client(const char* device_id, const char* module_id)
{
    IOTHUB_AUTHORIZATION_DATA* result;

    /* Codes_SRS_IoTHub_Authorization_07_002: [IoTHubClient_Auth_Create shall allocate a IOTHUB_AUTHORIZATION_HANDLE that is needed for subsequent calls. ] */
    result = (IOTHUB_AUTHORIZATION_DATA*)malloc(sizeof(IOTHUB_AUTHORIZATION_DATA) );
    if (result == NULL)
    {
        /* Codes_SRS_IoTHub_Authorization_07_019: [ On error IoTHubClient_Auth_Create shall return NULL. ] */
        LogError("Failed allocating IOTHUB_AUTHORIZATION_DATA");
        result = NULL;
    }
    else
    {
        memset(result, 0, sizeof(IOTHUB_AUTHORIZATION_DATA) );
        if (mallocAndStrcpy_s(&result->device_id, device_id) != 0)
        {
            /* Codes_SRS_IoTHub_Authorization_07_019: [ On error IoTHubClient_Auth_Create shall return NULL. ] */
            LogError("Failed allocating device_key");
            free(result);
            result = NULL;
        }
        else if (module_id != NULL && mallocAndStrcpy_s(&result->module_id, module_id) != 0)
        {
            /* Codes_SRS_IoTHub_Authorization_07_019: [ On error IoTHubClient_Auth_Create shall return NULL. ] */
            LogError("Failed allocating module_id");
            free(result->device_id);
            free(result);
            result = NULL;
        }
        else
        {
            result->token_expiry_time_sec = DEFAULT_SAS_TOKEN_EXPIRY_TIME_SECS;
        }
    }
    return result;
}

IOTHUB_AUTHORIZATION_HANDLE IoTHubClient_Auth_Create(const char* device_key, const char* device_id, const char* device_sas_token, const char *module_id)
{
    IOTHUB_AUTHORIZATION_DATA* result;
    bool is_key_valid;

    if (device_key == NULL)
    {
        is_key_valid = true;
    }
    else
    {
        /* Codes_SRS_IoTHub_Authorization_21_021: [ If the provided key is not base64 encoded, IoTHubClient_Auth_Create shall return NULL. ] */
        BUFFER_HANDLE key = Azure_Base64_Decode(device_key);
        if (key != NULL)
        {
            is_key_valid = true;
        }
        else
        {
            is_key_valid = false;
        }
        BUFFER_delete(key);
    }
    
    /* Codes_SRS_IoTHub_Authorization_07_001: [if device_id is NULL IoTHubClient_Auth_Create, shall return NULL. ] */
    if ((device_id == NULL) || (!is_key_valid))
    {
        LogError("Invalid Parameter %s", ((device_id == NULL) ? "device_id: NULL" : "key"));
        result = NULL;
    }
    else
    {
        result = initialize_auth_client(device_id, module_id);
        if (result == NULL)
        {
            LogError("Failure initializing auth client");
        }
        else if (device_key != NULL && mallocAndStrcpy_s(&result->device_key, device_key) != 0)
        {
            /* Codes_SRS_IoTHub_Authorization_07_019: [ On error IoTHubClient_Auth_Create shall return NULL. ] */
            LogError("Failed allocating device_key");
            free(result->device_id);
            free(result->module_id);
            free(result);
            result = NULL;
        }
        else
        {
            if (device_key != NULL)
            {
                /* Codes_SRS_IoTHub_Authorization_07_003: [ IoTHubClient_Auth_Create shall set the credential type to IOTHUB_CREDENTIAL_TYPE_DEVICE_KEY if the device_sas_token is NULL. ]*/
                result->cred_type = IOTHUB_CREDENTIAL_TYPE_DEVICE_KEY;
            }
            else if (device_sas_token != NULL)
            {
                /* Codes_SRS_IoTHub_Authorization_07_020: [ else IoTHubClient_Auth_Create shall set the credential type to IOTHUB_CREDENTIAL_TYPE_SAS_TOKEN. ] */
                result->cred_type = IOTHUB_CREDENTIAL_TYPE_SAS_TOKEN;
                if (mallocAndStrcpy_s(&result->device_sas_token, device_sas_token) != 0)
                {
                    /* Codes_SRS_IoTHub_Authorization_07_019: [ On error IoTHubClient_Auth_Create shall return NULL. ] */
                    LogError("Failed allocating device_key");
                    free(result->device_key);
                    free(result->device_id);
                    free(result->module_id);
                    free(result);
                    result = NULL;
                }
            }
            else
            {
                /* Codes_SRS_IoTHub_Authorization_07_024: [ if device_sas_token and device_key are NULL IoTHubClient_Auth_Create shall set the credential type to IOTHUB_CREDENTIAL_TYPE_UNKNOWN. ] */
                result->cred_type = IOTHUB_CREDENTIAL_TYPE_UNKNOWN;
            }
        }
    }
    /* Codes_SRS_IoTHub_Authorization_07_004: [ If successful IoTHubClient_Auth_Create shall return a IOTHUB_AUTHORIZATION_HANDLE value. ] */
    return result;
}

IOTHUB_AUTHORIZATION_HANDLE IoTHubClient_Auth_CreateFromDeviceAuth(const char* device_id, const char* module_id)
{
    IOTHUB_AUTHORIZATION_DATA* result;
    if (device_id == NULL)
    {
        LogError("Invalid Parameter device_id: %p", device_id);
        result = NULL;
    }
    else
    {
#ifdef USE_PROV_MODULE
        result = initialize_auth_client(device_id, module_id);
        if (result == NULL)
        {
            LogError("Failure initializing auth client");
        }
        else
        {
            result->device_auth_handle = iothub_device_auth_create();
            if (result->device_auth_handle == NULL)
            {
                LogError("Failed allocating IOTHUB_AUTHORIZATION_DATA");
                free(result->device_id);
                free(result->module_id);
                free(result);
                result = NULL;
            }
            else
            {
                DEVICE_AUTH_TYPE auth_type = iothub_device_auth_get_type(result->device_auth_handle);
                if (auth_type == AUTH_TYPE_SAS || auth_type == AUTH_TYPE_SYMM_KEY)
                {
                    result->cred_type = IOTHUB_CREDENTIAL_TYPE_DEVICE_AUTH;
                }
                else
                {
                    result->cred_type = IOTHUB_CREDENTIAL_TYPE_X509_ECC;
                }
            }
        }
#else
        (void)module_id;
        LogError("Failed HSM module is not supported");
        result = NULL;
#endif
    }
    return result;
}

void IoTHubClient_Auth_Destroy(IOTHUB_AUTHORIZATION_HANDLE handle)
{
    /* Codes_SRS_IoTHub_Authorization_07_005: [ if handle is NULL IoTHubClient_Auth_Destroy shall do nothing. ] */
    if (handle != NULL)
    {
        /* Codes_SRS_IoTHub_Authorization_07_006: [ IoTHubClient_Auth_Destroy shall free all resources associated with the IOTHUB_AUTHORIZATION_HANDLE handle. ] */
#ifdef USE_PROV_MODULE
        iothub_device_auth_destroy(handle->device_auth_handle);
#endif
        free(handle->device_key);
        free(handle->device_id);
        free(handle->module_id);
        free(handle->device_sas_token);
        free(handle);
    }
}

IOTHUB_CREDENTIAL_TYPE IoTHubClient_Auth_Set_x509_Type(IOTHUB_AUTHORIZATION_HANDLE handle, bool enable_x509)
{
    IOTHUB_CREDENTIAL_TYPE result;
    if (handle != NULL)
    {
        if (enable_x509)
        {
            result = handle->cred_type = IOTHUB_CREDENTIAL_TYPE_X509;
        }
        else
        {
            if (handle->device_sas_token == NULL)
            {
                result = handle->cred_type = IOTHUB_CREDENTIAL_TYPE_DEVICE_KEY;
            }
            else if (handle->device_key == NULL)
            {
                result = handle->cred_type = IOTHUB_CREDENTIAL_TYPE_SAS_TOKEN;
            }
            else
            {
                result = handle->cred_type = IOTHUB_CREDENTIAL_TYPE_UNKNOWN;
            }
        }
    }
    else
    {
        result = IOTHUB_CREDENTIAL_TYPE_UNKNOWN;
    }
    return result;
}

int IoTHubClient_Auth_Set_xio_Certificate(IOTHUB_AUTHORIZATION_HANDLE handle, XIO_HANDLE xio)
{
    int result;
    if (handle == NULL || xio == NULL)
    {
        LogError("Invalid Parameter handle: %p xio: %p", handle, xio);
        result = MU_FAILURE;
    }
    else if (handle->cred_type != IOTHUB_CREDENTIAL_TYPE_X509_ECC)
    {
        LogError("Invalid credential types for this operation");
        result = MU_FAILURE;
    }
    else
    {
#ifdef USE_PROV_MODULE
        CREDENTIAL_RESULT* cred_result = iothub_device_auth_generate_credentials(handle->device_auth_handle, NULL);
        if (cred_result == NULL)
        {
            LogError("Failure generating credentials");
            result = MU_FAILURE;
        }
        else
        {
            if (xio_setoption(xio, OPTION_X509_ECC_CERT, cred_result->auth_cred_result.x509_result.x509_cert) != 0)
            {
                LogError("Failure setting x509 cert on xio");
                result = MU_FAILURE;
            }
            else if (xio_setoption(xio, OPTION_X509_ECC_KEY, cred_result->auth_cred_result.x509_result.x509_alias_key) != 0)
            {
                LogError("Failure setting x509 key on xio");
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }
            free(cred_result);
        }
#else
        LogError("Failed HSM module is not supported");
        result = MU_FAILURE;
#endif
    }
    return result;
}

int IoTHubClient_Auth_Get_x509_info(IOTHUB_AUTHORIZATION_HANDLE handle, char** x509_cert, char** x509_key)
{
    int result;
    if (handle == NULL || x509_cert == NULL || x509_key == NULL)
    {
        LogError("Invalid Parameter handle: %p, x509_cert: %p, x509_key: %p", handle, x509_cert, x509_key);
        result = MU_FAILURE;
    }
    else if (handle->cred_type != IOTHUB_CREDENTIAL_TYPE_X509_ECC)
    {
        LogError("Invalid credential types for this operation");
        result = MU_FAILURE;
    }
    else
    {
#ifdef USE_PROV_MODULE
        CREDENTIAL_RESULT* cred_result = iothub_device_auth_generate_credentials(handle->device_auth_handle, NULL);
        if (cred_result == NULL)
        {
            LogError("Failure generating credentials");
            result = MU_FAILURE;
        }
        else
        {
            if (mallocAndStrcpy_s(x509_cert, cred_result->auth_cred_result.x509_result.x509_cert) != 0)
            {
                LogError("Failure copying certificate");
                result = MU_FAILURE;
            }
            else if (mallocAndStrcpy_s(x509_key, cred_result->auth_cred_result.x509_result.x509_alias_key) != 0)
            {
                LogError("Failure copying private key");
                result = MU_FAILURE;
                free(*x509_cert);
            }
            else
            {
                result = 0;
            }
            free(cred_result);
        }
#else
        LogError("Failed HSM module is not supported");
        result = MU_FAILURE;
#endif
    }
    return result;
}

IOTHUB_CREDENTIAL_TYPE IoTHubClient_Auth_Get_Credential_Type(IOTHUB_AUTHORIZATION_HANDLE handle)
{
    IOTHUB_CREDENTIAL_TYPE result;
    /* Codes_SRS_IoTHub_Authorization_07_007: [ if handle is NULL IoTHub_Auth_Get_Credential_Type shall return IOTHUB_CREDENTIAL_TYPE_UNKNOWN. ] */
    if (handle == NULL)
    {
        LogError("Invalid Parameter handle: %p", handle);
        result = IOTHUB_CREDENTIAL_TYPE_UNKNOWN;
    }
    else
    {
        /* Codes_SRS_IoTHub_Authorization_07_008: [ IoTHub_Auth_Get_Credential_Type shall return the credential type that is set upon creation. ] */
        result = handle->cred_type;
    }
    return result;
}

char* IoTHubClient_Auth_Get_SasToken(IOTHUB_AUTHORIZATION_HANDLE handle, const char* scope, uint64_t expiry_time_relative_seconds, const char* key_name)
{
    char* result;
    (void)expiry_time_relative_seconds;
    /* Codes_SRS_IoTHub_Authorization_07_009: [ if handle or scope are NULL, IoTHubClient_Auth_Get_SasToken shall return NULL. ] */
    if (handle == NULL)
    {
        LogError("Invalid Parameter handle: %p", handle);
        result = NULL;
    }
    else
    {
        if (handle->cred_type == IOTHUB_CREDENTIAL_TYPE_DEVICE_AUTH)
        {
#ifdef USE_PROV_MODULE
            DEVICE_AUTH_CREDENTIAL_INFO dev_auth_cred;
            uint64_t sec_since_epoch;

            if (get_seconds_since_epoch(&sec_since_epoch) != 0)
            {
                LogError("failure getting seconds from epoch");
                result = NULL;
            }
            else
            {
                memset(&dev_auth_cred, 0, sizeof(DEVICE_AUTH_CREDENTIAL_INFO));
                uint64_t expiry_time = sec_since_epoch + handle->token_expiry_time_sec;
                dev_auth_cred.sas_info.expiry_seconds = expiry_time;
                dev_auth_cred.sas_info.token_scope = scope;
                dev_auth_cred.sas_info.key_name = key_name;
                dev_auth_cred.dev_auth_type = AUTH_TYPE_SAS;

                CREDENTIAL_RESULT* cred_result = iothub_device_auth_generate_credentials(handle->device_auth_handle, &dev_auth_cred);
                if (cred_result == NULL)
                {
                    LogError("failure getting credentials from device auth module");
                    result = NULL;
                }
                else
                {
                    if (mallocAndStrcpy_s(&result, cred_result->auth_cred_result.sas_result.sas_token) != 0)
                    {
                        LogError("failure allocating Sas Token");
                        result = NULL;
                    }
                    free(cred_result);
                }
            }
#else
            LogError("Failed HSM module is not supported");
            result = NULL;
#endif
        }
        else if (handle->cred_type == IOTHUB_CREDENTIAL_TYPE_SAS_TOKEN)
        {
            /* Codes_SRS_IoTHub_Authorization_07_021: [If the device_sas_token is NOT NULL IoTHubClient_Auth_Get_SasToken shall return a copy of the device_sas_token. ] */
            if (handle->device_sas_token != NULL)
            {
                if (mallocAndStrcpy_s(&result, handle->device_sas_token) != 0)
                {
                    LogError("failure allocating sas token");
                    result = NULL;
                }
            }
            else
            {
                LogError("failure device sas token is NULL");
                result = NULL;
            }
        }
        else if (handle->cred_type == IOTHUB_CREDENTIAL_TYPE_DEVICE_KEY)
        {
            /* Codes_SRS_IoTHub_Authorization_07_009: [ if handle or scope are NULL, IoTHubClient_Auth_Get_SasToken shall return NULL. ] */
            if (scope == NULL)
            {
                LogError("Invalid Parameter scope: %p", scope);
                result = NULL;
            }
            else
            {
                STRING_HANDLE sas_token;
                uint64_t sec_since_epoch;

                /* Codes_SRS_IoTHub_Authorization_07_010: [ IoTHubClient_Auth_Get_SasToken` shall construct the expiration time using the handle->token_expiry_time_sec added to epoch time. ] */
                if (get_seconds_since_epoch(&sec_since_epoch) != 0)
                {
                    /* Codes_SRS_IoTHub_Authorization_07_020: [ If any error is encountered IoTHubClient_Auth_Get_ConnString shall return NULL. ] */
                    LogError("failure getting seconds from epoch");
                    result = NULL;
                }
                else
                {
                    /* Codes_SRS_IoTHub_Authorization_07_011: [ IoTHubClient_Auth_Get_ConnString shall call SASToken_CreateString to construct the sas token. ] */
                    uint64_t expiry_time = sec_since_epoch + handle->token_expiry_time_sec;
                    if ( (sas_token = SASToken_CreateString(handle->device_key, scope, key_name, expiry_time)) == NULL)
                    {
                        /* Codes_SRS_IoTHub_Authorization_07_020: [ If any error is encountered IoTHubClient_Auth_Get_ConnString shall return NULL. ] */
                        LogError("Failed creating sas_token");
                        result = NULL;
                    }
                    else
                    {
                        /* Codes_SRS_IoTHub_Authorization_07_012: [ On success IoTHubClient_Auth_Get_ConnString shall allocate and return the sas token in a char*. ] */
                        if (mallocAndStrcpy_s(&result, STRING_c_str(sas_token) ) != 0)
                        {
                            /* Codes_SRS_IoTHub_Authorization_07_020: [ If any error is encountered IoTHubClient_Auth_Get_ConnString shall return NULL. ] */
                            LogError("Failed copying result");
                            result = NULL;
                        }
                        STRING_delete(sas_token);
                    }
                }
            }
        }
        else
        {
            LogError("Failed getting sas token invalid credential type");
            result = NULL;
        }
    }
    return result;
}

const char* IoTHubClient_Auth_Get_DeviceId(IOTHUB_AUTHORIZATION_HANDLE handle)
{
    const char* result;
    if (handle == NULL)
    {
        /* Codes_SRS_IoTHub_Authorization_07_013: [ if handle is NULL, IoTHubClient_Auth_Get_DeviceId shall return NULL. ] */
        LogError("Invalid Parameter handle: %p", handle);
        result = NULL;
    }
    else
    {
        /* Codes_SRS_IoTHub_Authorization_07_014: [ IoTHubClient_Auth_Get_DeviceId shall return the device_id specified upon creation. ] */
        result = handle->device_id;
    }
    return result;
}

const char* IoTHubClient_Auth_Get_ModuleId(IOTHUB_AUTHORIZATION_HANDLE handle)
{
    const char* result;
    if (handle == NULL)
    {
        /* Codes_SRS_IoTHub_Authorization_31_025: [ if handle is NULL, IoTHubClient_Auth_Get_ModuleId shall return NULL. ] */
        LogError("Invalid Parameter handle: %p", handle);
        result = NULL;
    }
    else
    {
        /* Codes_SRS_IoTHub_Authorization_31_026: [ IoTHubClient_Auth_Get_ModuleId shall return the module_id specified upon creation. ] */
        result = handle->module_id;
    }
    return result;
}

const char* IoTHubClient_Auth_Get_DeviceKey(IOTHUB_AUTHORIZATION_HANDLE handle)
{
    const char* result;
    if (handle == NULL)
    {
        /* Codes_SRS_IoTHub_Authorization_07_022: [ if handle is NULL, IoTHubClient_Auth_Get_DeviceKey shall return NULL. ] */
        LogError("Invalid Parameter handle: %p", handle);
        result = NULL;
    }
    else
    {
        /* Codes_SRS_IoTHub_Authorization_07_023: [ IoTHubClient_Auth_Get_DeviceKey shall return the device_key specified upon creation. ] */
        result = handle->device_key;
    }
    return result;
}

SAS_TOKEN_STATUS IoTHubClient_Auth_Is_SasToken_Valid(IOTHUB_AUTHORIZATION_HANDLE handle)
{
    SAS_TOKEN_STATUS result;
    if (handle == NULL)
    {
        /* Codes_SRS_IoTHub_Authorization_07_015: [ if handle is NULL, IoTHubClient_Auth_Is_SasToken_Valid shall return false. ] */
        LogError("Invalid Parameter handle: %p", handle);
        result = SAS_TOKEN_STATUS_FAILED;
    }
    else
    {
        if (handle->cred_type == IOTHUB_CREDENTIAL_TYPE_SAS_TOKEN)
        {
            if (handle->device_sas_token == NULL)
            {
                /* Codes_SRS_IoTHub_Authorization_07_017: [ If the sas_token is NULL IoTHubClient_Auth_Is_SasToken_Valid shall return false. ] */
                LogError("Failure: device_sas_toke is NULL");
                result = SAS_TOKEN_STATUS_FAILED;
            }
            else
            {
                /* Codes_SRS_IoTHub_Authorization_07_018: [ otherwise IoTHubClient_Auth_Is_SasToken_Valid shall return the value returned by SASToken_Validate. ] */
                STRING_HANDLE strSasToken = STRING_construct(handle->device_sas_token);
                if (strSasToken != NULL)
                {
                    if (!SASToken_Validate(strSasToken))
                    {
                        result = SAS_TOKEN_STATUS_INVALID;
                    }
                    else
                    {
                        result = SAS_TOKEN_STATUS_VALID;
                    }
                    STRING_delete(strSasToken);
                }
                else
                {
                    LogError("Failure constructing SAS Token");
                    result = SAS_TOKEN_STATUS_FAILED;
                }
            }
        }
        else
        {
            /* Codes_SRS_IoTHub_Authorization_07_016: [ if credential type is not IOTHUB_CREDENTIAL_TYPE_SAS_TOKEN IoTHubClient_Auth_Is_SasToken_Valid shall return SAS_TOKEN_STATUS_VALID. ] */
            result = SAS_TOKEN_STATUS_VALID;
        }
    }
    return result;
}

#ifdef USE_EDGE_MODULES

// For debugging C modules, the environment can set the environment variable 'EdgeModuleCACertificateFile' to provide
// trusted certificates.  We'd otherwise usually get these from trusted Edge service, but this complicates debugging experience.
// EdgeModuleCACertificateFile and the related EdgeHubConnectionString can be set either manually or by tooling (e.g. VS Code).
static char* read_ca_certificate_from_file(const char* certificate_file_name)
{
    char* result;
    FILE *file_stream = NULL;

    if ((file_stream = fopen(certificate_file_name, "r")) == NULL)
    {
        LogError("Cannot read file %s, errno=%d", certificate_file_name, errno);
        result = NULL;
    }
    else if (fseek(file_stream, 0, SEEK_END) != 0)
    {
        LogError("fseek on file %s fails, errno=%d", certificate_file_name, errno);
        result = NULL;
    }
    else
    {
        long int file_size = ftell(file_stream);
        if (file_size < 0)
        {
            LogError("ftell fails reading %s, errno=%d",  certificate_file_name, errno);
            result = NULL;
        }
        else if (file_size == 0)
        {
            LogError("file %s is 0 bytes, which is not valid certificate",  certificate_file_name);
            result = NULL;
        }
        else
        {
            rewind(file_stream);

            if ((result = calloc(1, file_size + 1)) == NULL)
            {
                LogError("Cannot allocate %lu bytes", (unsigned long)file_size);
            }
            else if ((fread(result, 1, file_size, file_stream) == 0) || (ferror(file_stream) != 0))
            {
                LogError("fread failed on file %s, errno=%d", certificate_file_name, errno);
                free(result);
                result = NULL;
            }
        }
    }

    if (file_stream != NULL)
    {
        fclose(file_stream);
    }

    return result;
}

// IoTHubClient_Auth_Get_TrustBundle retrieves a trust bundle - namely a PEM indicating the certificates the client should
// trust as root authorities - to caller.  If certificate_file_name, we read this from a local file.  This should in general
// be limited only to debugging modules on Edge.  If certificate_file_name is NULL, we invoke into the underlying
// HSM to retrieve this.
char* IoTHubClient_Auth_Get_TrustBundle(IOTHUB_AUTHORIZATION_HANDLE handle, const char* certificate_file_name)
{
    char* result;
    if (handle == NULL)
    {
        LogError("Security Handle is NULL");
        result = NULL;
    }
    else if (certificate_file_name != NULL)
    {
        result = read_ca_certificate_from_file(certificate_file_name);
    }
    else
    {
        result = iothub_device_auth_get_trust_bundle(handle->device_auth_handle);
    }
    return result;
}
#endif

int IoTHubClient_Auth_Set_SasToken_Expiry(IOTHUB_AUTHORIZATION_HANDLE handle, uint64_t expiry_time_seconds)
{
    int result;
    if (handle == NULL)
    {
        LogError("Invalid handle value handle: NULL");
        result = MU_FAILURE;
    }
    // Validate the expiry_time in seconds
    else if (expiry_time_seconds < MIN_SAS_EXPIRY_TIME)
    {
        LogError("Failure setting expiry time to value %lu min value is %d", (unsigned long)expiry_time_seconds, MIN_SAS_EXPIRY_TIME);
        result = MU_FAILURE;
    }
    else
    {
        handle->token_expiry_time_sec = expiry_time_seconds;
        result = 0;
    }
    return result;
}

uint64_t IoTHubClient_Auth_Get_SasToken_Expiry(IOTHUB_AUTHORIZATION_HANDLE handle)
{
    uint64_t result;
    if (handle == NULL)
    {
        LogError("Invalid handle value handle: NULL");
        result = 0;
    }
    else
    {
        result = handle->token_expiry_time_sec;
    }
    return result;
}
