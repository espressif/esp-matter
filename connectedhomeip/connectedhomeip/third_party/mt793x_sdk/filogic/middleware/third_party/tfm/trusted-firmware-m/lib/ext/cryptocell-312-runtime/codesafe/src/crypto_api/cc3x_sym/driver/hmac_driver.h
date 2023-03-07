/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef  _HMAC_DRIVER_H
#define  _HMAC_DRIVER_H

#include "driver_defs.h"

/******************************************************************************
*               TYPE DEFINITIONS
******************************************************************************/

/* The context data-base used by the Hmac functions on the low level */
typedef struct HmacContext {
    uint32_t valid_tag;
    /* Key XOR opad result */
    uint8_t KeyXorOpadBuff[CC_HMAC_SHA2_1024BIT_KEY_SIZE_IN_BYTES];
    /* The operation mode */
    CCHashOperationMode_t mode;
    /* The user HASH context - required for operating the HASH described below */
    CCHashUserContext_t HashUserContext;
} HmacContext_t;


#endif /* _HMAC_DRIVER_H */

