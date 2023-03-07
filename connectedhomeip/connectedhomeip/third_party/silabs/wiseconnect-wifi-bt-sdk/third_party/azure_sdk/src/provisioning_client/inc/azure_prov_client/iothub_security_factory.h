// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_SECURITY_FACTORY_H
#define IOTHUB_SECURITY_FACTORY_H

#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stddef.h>
#endif /* __cplusplus */

#include "umock_c/umock_c_prod.h"
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/buffer_.h"

#define IOTHUB_SECURITY_TYPE_VALUES \
    IOTHUB_SECURITY_TYPE_UNKNOWN,   \
    IOTHUB_SECURITY_TYPE_SAS,       \
    IOTHUB_SECURITY_TYPE_X509,      \
    IOTHUB_SECURITY_TYPE_HTTP_EDGE, \
    IOTHUB_SECURITY_TYPE_SYMMETRIC_KEY


MU_DEFINE_ENUM_WITHOUT_INVALID(IOTHUB_SECURITY_TYPE, IOTHUB_SECURITY_TYPE_VALUES);

MOCKABLE_FUNCTION(, int, iothub_security_init, IOTHUB_SECURITY_TYPE, sec_type);
MOCKABLE_FUNCTION(, void, iothub_security_deinit);

MOCKABLE_FUNCTION(, const void*, iothub_security_interface);
MOCKABLE_FUNCTION(, IOTHUB_SECURITY_TYPE, iothub_security_type);

// Symmetric key information
MOCKABLE_FUNCTION(, int, iothub_security_set_symmetric_key_info, const char*, registration_name, const char*, symmetric_key);
MOCKABLE_FUNCTION(, const char*, iothub_security_get_symmetric_key);
MOCKABLE_FUNCTION(, const char*, iothub_security_get_symm_registration_name);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // IOTHUB_SECURITY_FACTORY_H
