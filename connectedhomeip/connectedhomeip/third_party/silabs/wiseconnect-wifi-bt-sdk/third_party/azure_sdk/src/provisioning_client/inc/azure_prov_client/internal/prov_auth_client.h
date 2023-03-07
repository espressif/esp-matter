// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef PROV_AUTH_CLIENT_H
#define PROV_AUTH_CLIENT_H

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
#include "azure_c_shared_utility/buffer_.h"

typedef struct PROV_AUTH_INFO_TAG* PROV_AUTH_HANDLE;

#define PROV_AUTH_RESULT_VALUES   \
    PROV_AUTH_SUCCESS,            \
    PROV_AUTH_INVALID_ARG,        \
    PROV_AUTH_ERROR,              \
    PROV_AUTH_STATUS_UNASSIGNED,   \
    PROV_AUTH_STATUS_ASSIGNING

MU_DEFINE_ENUM_WITHOUT_INVALID(PROV_AUTH_RESULT, PROV_AUTH_RESULT_VALUES);

#define PROV_AUTH_TYPE_VALUES \
    PROV_AUTH_TYPE_UNKNOWN,   \
    PROV_AUTH_TYPE_TPM,       \
    PROV_AUTH_TYPE_X509,      \
    PROV_AUTH_TYPE_KEY

MU_DEFINE_ENUM_WITHOUT_INVALID(PROV_AUTH_TYPE, PROV_AUTH_TYPE_VALUES);

MOCKABLE_FUNCTION(, PROV_AUTH_HANDLE, prov_auth_create);
MOCKABLE_FUNCTION(, void, prov_auth_destroy, PROV_AUTH_HANDLE, handle);
MOCKABLE_FUNCTION(, PROV_AUTH_TYPE, prov_auth_get_type, PROV_AUTH_HANDLE, handle);
MOCKABLE_FUNCTION(, char*, prov_auth_get_registration_id, PROV_AUTH_HANDLE, handle);
MOCKABLE_FUNCTION(, int, prov_auth_set_registration_id, PROV_AUTH_HANDLE, handle, const char*, registration_id);

// TPM
MOCKABLE_FUNCTION(, BUFFER_HANDLE, prov_auth_get_endorsement_key, PROV_AUTH_HANDLE, handle);
MOCKABLE_FUNCTION(, BUFFER_HANDLE, prov_auth_get_storage_key, PROV_AUTH_HANDLE, handle);
MOCKABLE_FUNCTION(, int, prov_auth_import_key, PROV_AUTH_HANDLE, handle, const unsigned char*, key, size_t, key_len);
MOCKABLE_FUNCTION(, char*, prov_auth_construct_sas_token, PROV_AUTH_HANDLE, handle, const char*, token_scope, const char*, key_name, size_t, expiry_time);

// X509
MOCKABLE_FUNCTION(, char*, prov_auth_get_certificate, PROV_AUTH_HANDLE, handle);
MOCKABLE_FUNCTION(, char*, prov_auth_get_alias_key, PROV_AUTH_HANDLE, handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // PROV_AUTH_CLIENT_H
