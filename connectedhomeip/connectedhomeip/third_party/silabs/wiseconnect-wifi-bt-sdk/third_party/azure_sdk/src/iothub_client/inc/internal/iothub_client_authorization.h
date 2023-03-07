// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_CLIENT_AUTHORIZATION_H
#define IOTHUB_CLIENT_AUTHORIZATION_H

#include "azure_macro_utils/macro_utils.h"
#include "umock_c/umock_c_prod.h"
#include "azure_c_shared_utility/xio.h"

#ifdef __cplusplus
extern "C" {
#include <cstdint>
#else
#include <stdint.h>
#include <stdbool.h>
#endif /* __cplusplus */

typedef struct IOTHUB_AUTHORIZATION_DATA_TAG* IOTHUB_AUTHORIZATION_HANDLE;

#define IOTHUB_CREDENTIAL_TYPE_VALUES       \
    IOTHUB_CREDENTIAL_TYPE_UNKNOWN,         \
    IOTHUB_CREDENTIAL_TYPE_DEVICE_KEY,      \
    IOTHUB_CREDENTIAL_TYPE_X509,            \
    IOTHUB_CREDENTIAL_TYPE_X509_ECC,        \
    IOTHUB_CREDENTIAL_TYPE_SAS_TOKEN,       \
    IOTHUB_CREDENTIAL_TYPE_DEVICE_AUTH


MU_DEFINE_ENUM_WITHOUT_INVALID(IOTHUB_CREDENTIAL_TYPE, IOTHUB_CREDENTIAL_TYPE_VALUES);

#define SAS_TOKEN_STATUS_VALUES  \
    SAS_TOKEN_STATUS_FAILED,     \
    SAS_TOKEN_STATUS_VALID,      \
    SAS_TOKEN_STATUS_INVALID

MU_DEFINE_ENUM_WITHOUT_INVALID(SAS_TOKEN_STATUS, SAS_TOKEN_STATUS_VALUES);

MOCKABLE_FUNCTION(, IOTHUB_AUTHORIZATION_HANDLE, IoTHubClient_Auth_Create, const char*, device_key, const char*, device_id, const char*, device_sas_token, const char *, module_id);
MOCKABLE_FUNCTION(, IOTHUB_AUTHORIZATION_HANDLE, IoTHubClient_Auth_CreateFromDeviceAuth, const char*, device_id, const char*, module_id);
MOCKABLE_FUNCTION(, void, IoTHubClient_Auth_Destroy, IOTHUB_AUTHORIZATION_HANDLE, handle);
MOCKABLE_FUNCTION(, IOTHUB_CREDENTIAL_TYPE, IoTHubClient_Auth_Set_x509_Type, IOTHUB_AUTHORIZATION_HANDLE, handle, bool, enable_x509);
MOCKABLE_FUNCTION(, IOTHUB_CREDENTIAL_TYPE, IoTHubClient_Auth_Get_Credential_Type, IOTHUB_AUTHORIZATION_HANDLE, handle);
MOCKABLE_FUNCTION(, char*, IoTHubClient_Auth_Get_SasToken, IOTHUB_AUTHORIZATION_HANDLE, handle, const char*, scope, uint64_t, expiry_time_relative_seconds, const char*, key_name);
MOCKABLE_FUNCTION(, int, IoTHubClient_Auth_Set_xio_Certificate, IOTHUB_AUTHORIZATION_HANDLE, handle, XIO_HANDLE, xio);
MOCKABLE_FUNCTION(, const char*, IoTHubClient_Auth_Get_DeviceId, IOTHUB_AUTHORIZATION_HANDLE, handle);
MOCKABLE_FUNCTION(, const char*, IoTHubClient_Auth_Get_ModuleId, IOTHUB_AUTHORIZATION_HANDLE, handle);
MOCKABLE_FUNCTION(, const char*, IoTHubClient_Auth_Get_DeviceKey, IOTHUB_AUTHORIZATION_HANDLE, handle);
MOCKABLE_FUNCTION(, SAS_TOKEN_STATUS, IoTHubClient_Auth_Is_SasToken_Valid, IOTHUB_AUTHORIZATION_HANDLE, handle);
MOCKABLE_FUNCTION(, int, IoTHubClient_Auth_Get_x509_info, IOTHUB_AUTHORIZATION_HANDLE, handle, char**, x509_cert, char**, x509_key);
MOCKABLE_FUNCTION(, int, IoTHubClient_Auth_Set_SasToken_Expiry, IOTHUB_AUTHORIZATION_HANDLE, handle, uint64_t, expiry_time_seconds);
MOCKABLE_FUNCTION(, uint64_t, IoTHubClient_Auth_Get_SasToken_Expiry, IOTHUB_AUTHORIZATION_HANDLE, handle);


#ifdef USE_EDGE_MODULES
MOCKABLE_FUNCTION(, char*, IoTHubClient_Auth_Get_TrustBundle, IOTHUB_AUTHORIZATION_HANDLE, handle, const char*, certificate_file_name);
#endif


#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_CLIENT_AUTHORIZATION_H */
