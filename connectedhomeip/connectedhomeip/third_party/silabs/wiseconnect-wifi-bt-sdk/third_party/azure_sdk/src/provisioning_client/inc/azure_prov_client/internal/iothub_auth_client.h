// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef DEVICE_AUTH_H
#define DEVICE_AUTH_H

#ifdef __cplusplus
extern "C" {
#include <cstdint>
#include <cstddef>
#else
#include <stdint.h>
#include <stddef.h>
#endif /* __cplusplus */

#include "umock_c/umock_c_prod.h"
#include "azure_macro_utils/macro_utils.h"

typedef struct IOTHUB_SECURITY_INFO_TAG* IOTHUB_SECURITY_HANDLE;

#define DEVICE_AUTH_TYPE_VALUES \
    AUTH_TYPE_UNKNOWN,          \
    AUTH_TYPE_SAS,              \
    AUTH_TYPE_X509,             \
    AUTH_TYPE_SYMM_KEY

MU_DEFINE_ENUM_WITHOUT_INVALID(DEVICE_AUTH_TYPE, DEVICE_AUTH_TYPE_VALUES);

typedef struct DEVICE_AUTH_SAS_INFO_TAG
{
    uint64_t expiry_seconds;
    const char* token_scope;
    const char* key_name;
} DEVICE_AUTH_SAS_INFO;

typedef struct DEVICE_AUTH_SAS_RESULT_TAG
{
    const char* sas_token;
} DEVICE_AUTH_SAS_RESULT;

typedef struct DEVICE_AUTH_X509_RESULT_TAG
{
    const char* x509_cert;
    const char* x509_alias_key;
} DEVICE_AUTH_X509_RESULT;

typedef struct DEVICE_AUTH_CREDENTIAL_INFO_TAG
{
    DEVICE_AUTH_TYPE dev_auth_type;
    DEVICE_AUTH_SAS_INFO sas_info;
} DEVICE_AUTH_CREDENTIAL_INFO;

typedef struct CREDENTIAL_RESULT_TAG
{
    DEVICE_AUTH_TYPE dev_auth_type;
    union
    {
        DEVICE_AUTH_SAS_RESULT sas_result;
        DEVICE_AUTH_X509_RESULT x509_result;
    } auth_cred_result;
} CREDENTIAL_RESULT;


MOCKABLE_FUNCTION(, IOTHUB_SECURITY_HANDLE, iothub_device_auth_create);
MOCKABLE_FUNCTION(, void, iothub_device_auth_destroy, IOTHUB_SECURITY_HANDLE, handle);
MOCKABLE_FUNCTION(, DEVICE_AUTH_TYPE, iothub_device_auth_get_type, IOTHUB_SECURITY_HANDLE, handle);
MOCKABLE_FUNCTION(, CREDENTIAL_RESULT*, iothub_device_auth_generate_credentials, IOTHUB_SECURITY_HANDLE, handle, const DEVICE_AUTH_CREDENTIAL_INFO*, dev_auth_cred);

#ifdef USE_EDGE_MODULES
MOCKABLE_FUNCTION(, char*, iothub_device_auth_get_trust_bundle, IOTHUB_SECURITY_HANDLE, handle);
#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // DEVICE_AUTH
