// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef PROV_TRANSPORT_H
#define PROV_TRANSPORT_H

#include "umock_c/umock_c_prod.h"
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_prov_client/prov_client_const.h"

#ifdef __cplusplus
extern "C" {
#include <cstdint>
#include <cstdlib>
#else
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#endif /* __cplusplus */

    struct PROV_DEVICE_TRANSPORT_PROVIDER_TAG;
    typedef struct PROV_DEVICE_TRANSPORT_PROVIDER_TAG PROV_DEVICE_TRANSPORT_PROVIDER;

    typedef void* PROV_DEVICE_TRANSPORT_HANDLE;

#define TRANSPORT_HSM_TYPE_VALUES   \
    TRANSPORT_HSM_TYPE_TPM,         \
    TRANSPORT_HSM_TYPE_X509,        \
    TRANSPORT_HSM_TYPE_SYMM_KEY

    MU_DEFINE_ENUM_WITHOUT_INVALID(TRANSPORT_HSM_TYPE, TRANSPORT_HSM_TYPE_VALUES);

#define PROV_DEVICE_TRANSPORT_RESULT_VALUES     \
    PROV_DEVICE_TRANSPORT_RESULT_OK,            \
    PROV_DEVICE_TRANSPORT_RESULT_UNAUTHORIZED,  \
    PROV_DEVICE_TRANSPORT_RESULT_ERROR

    MU_DEFINE_ENUM_WITHOUT_INVALID(PROV_DEVICE_TRANSPORT_RESULT, PROV_DEVICE_TRANSPORT_RESULT_VALUES);

    // Snippit from RFC 7231 https://tools.ietf.org/html/rfc7231
    // The value of this field can be either an HTTP - date or a number of
    // seconds to delay after the response is received.

    // Retry - After = HTTP - date / delay - seconds

    // A delay - seconds value is a non - negative decimal integer, representing
    // time in seconds.

    // delay - seconds = 1 * DIGIT
    // Two examples of its use are

    // Retry-After: Fri, 31 Dec 1999 23 : 59 : 59 GMT
    // Retry-After : 120
    static inline uint32_t parse_retry_after_value(const char* retry_after)
    {
        uint32_t result = PROV_GET_THROTTLE_TIME;
        if (retry_after != NULL)
        {
            // Is the retry after a number
            if (retry_after[0] >= 0x30 && retry_after[0] <= 0x39)
            {
                result = atol(retry_after);
                if (result < PROV_GET_THROTTLE_TIME || result > MAX_PROV_GET_THROTTLE_TIME)
                {
                    result = PROV_GET_THROTTLE_TIME;
                }
            }
            // Will need to parse the retry after for date information
        }
        return result;
    }
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // PROV_TRANSPORT
