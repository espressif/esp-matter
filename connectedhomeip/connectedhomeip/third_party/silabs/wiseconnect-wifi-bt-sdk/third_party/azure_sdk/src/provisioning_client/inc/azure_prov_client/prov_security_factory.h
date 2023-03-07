// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef PROV_SECURITY_FACTORY_H
#define PROV_SECURITY_FACTORY_H

#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stddef.h>
#endif /* __cplusplus */

#include "umock_c/umock_c_prod.h"
#include "azure_macro_utils/macro_utils.h"

#define SECURE_DEVICE_TYPE_VALUES \
    SECURE_DEVICE_TYPE_UNKNOWN,   \
    SECURE_DEVICE_TYPE_TPM,       \
    SECURE_DEVICE_TYPE_X509,      \
    SECURE_DEVICE_TYPE_HTTP_EDGE, \
    SECURE_DEVICE_TYPE_SYMMETRIC_KEY

MU_DEFINE_ENUM_WITHOUT_INVALID(SECURE_DEVICE_TYPE, SECURE_DEVICE_TYPE_VALUES);


MOCKABLE_FUNCTION(, int, prov_dev_security_init, SECURE_DEVICE_TYPE, hsm_type);
MOCKABLE_FUNCTION(, void, prov_dev_security_deinit);
MOCKABLE_FUNCTION(, SECURE_DEVICE_TYPE, prov_dev_security_get_type);

// Symmetric key information
MOCKABLE_FUNCTION(, int, prov_dev_set_symmetric_key_info, const char*, registration_name, const char*, symmetric_key);
MOCKABLE_FUNCTION(, const char*, prov_dev_get_symmetric_key);
MOCKABLE_FUNCTION(, const char*, prov_dev_get_symm_registration_name);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // PROV_SECURITY_FACTORY_H
