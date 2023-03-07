// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef PLATFORM_H
#define PLATFORM_H

#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/xio.h"
#include "umock_c/umock_c_prod.h"

#define GUID_LENGTH 64

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    #define PLATFORM_INFO_OPTION_VALUES       \
        PLATFORM_INFO_OPTION_DEFAULT,         \
        PLATFORM_INFO_OPTION_RETRIEVE_SQM

    MU_DEFINE_ENUM(PLATFORM_INFO_OPTION, PLATFORM_INFO_OPTION_VALUES);

    MOCKABLE_FUNCTION(, int, platform_init);
    MOCKABLE_FUNCTION(, void, platform_deinit);
    MOCKABLE_FUNCTION(, const IO_INTERFACE_DESCRIPTION*, platform_get_default_tlsio);
    MOCKABLE_FUNCTION(, STRING_HANDLE, platform_get_platform_info, PLATFORM_INFO_OPTION, options);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PLATFORM_H */
