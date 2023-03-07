// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef HMAC_H
#define HMAC_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#include "azure_c_shared_utility/sha.h"
#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif

    MOCKABLE_FUNCTION(, int, hmac, SHAversion, whichSha, const unsigned char *, text, int, text_len,
    const unsigned char *, key, int, key_len,
    uint8_t, digest[USHAMaxHashSize]);

#ifdef __cplusplus
}
#endif

#endif /* HMAC_H */
