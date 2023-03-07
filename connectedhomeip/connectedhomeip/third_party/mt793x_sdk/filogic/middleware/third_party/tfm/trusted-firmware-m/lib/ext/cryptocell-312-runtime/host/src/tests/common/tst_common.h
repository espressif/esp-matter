/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

 /*
  Common  functions prototypes and definitions used in CRYS tests
  */

#ifndef __TST_COMMON_H__
#define __TST_COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#if (!defined CC_SW) && (!defined CC_IOT)
#include "cc_fips.h"
#endif
#include "cc_lib.h"

/**
 * The function copies src buffer to dst with reversed bytes order.
 *
 * Note: Overlapping is not allowed, besides reversing of the buffer in place.
 *
 * @param dst
 * @param src
 * @param sizeInBytes
 */
int TST_MemCpyReversed( void* dst, void* src, unsigned int sizeInBytes);


#ifndef CC_SW
#ifdef CC_IOT
#define TST_CC_LIB_INIT(rc, rndContext_ptr, rndWorkBuff_ptr)       rc = CC_LibInit(rndContext_ptr, rndWorkBuff_ptr)
int tests_CC_libInit(CCRndContext_t* rndContext_ptr, CCRndWorkBuff_t * rndWorkBuff_ptr, unsigned long* stackAddress);
void* Test_LibInit(void *params );

typedef struct LibInitArgs {
    CCRndContext_t * rndContext_ptr;
    CCRndWorkBuff_t * rndWorkBuff_ptr;
    //RC - should also be a void* for entropy context (currently saved inside rndContext)
} LibInitArgs;

#else
CCFipsKatContext_t  fipsCtx;
#ifdef CC_SUPPORT_FIPS
int testSetReeFipsError(uint32_t  reeError, CCFipsState_t expfipsState);
// set REE error value to be REE ok, same value as CC_FIPS_SYNC_REE_STATUS|CC_FIPS_SYNC_MODULE_OK
#define TST_CC_LIB_INIT(rc, rndContext_ptr, rndWorkBuff_ptr)       {\
    rc = CC_LibInit(rndContext_ptr, rndWorkBuff_ptr, false, &fipsCtx);\
    if (rc == 0) {\
        rc = testSetReeFipsError((0x4|0x0), CC_FIPS_STATE_NOT_SUPPORTED);\
    }\
}
#else
#define TST_CC_LIB_INIT(rc, rndContext_ptr, rndWorkBuff_ptr)       {\
    rc = CC_LibInit(rndContext_ptr, rndWorkBuff_ptr, false, &fipsCtx);\
}
#endif
#endif
#endif

#endif
