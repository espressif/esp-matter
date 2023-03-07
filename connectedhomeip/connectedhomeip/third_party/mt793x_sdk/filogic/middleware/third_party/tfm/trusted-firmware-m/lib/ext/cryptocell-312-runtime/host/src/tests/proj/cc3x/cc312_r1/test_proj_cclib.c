/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>

#include "cc_lib.h"
#include "test_pal_thread.h"
#include "test_pal_log.h"

#define  THREAD_STACK_SIZE              (128*1024)

typedef struct LibInitArgs {
    void* p_rng;
    void* p_entropy;
    CCRndWorkBuff_t * rndWorkBuff_ptr;
} LibInitArgs;

uint32_t threadErr = 0;
static CCRndContext_t gRndContext_ptr;

static void* Test_LibInit(void *params ){
    uint32_t rc;
    LibInitArgs* threadArgs = (LibInitArgs*)params;
    gRndContext_ptr.rndState = threadArgs->p_rng;
    gRndContext_ptr.entropyCtx = threadArgs->p_entropy;

    rc = CC_LibInit(&gRndContext_ptr, threadArgs->rndWorkBuff_ptr);
    threadErr = rc;
    return((void *)threadErr);

}

int Test_Proj_CC_LibInit_Wrap(void* p_rng, void* p_entropy, CCRndWorkBuff_t * rndWorkBuff_ptr){
    uint32_t rc = 0;
    uint32_t priority = Test_PalGetDefaultPriority();
    int threadRc;
    ThreadHandle threadHandle;
    LibInitArgs params;

    params.p_rng = p_rng;
    params.p_entropy = p_entropy;
    params.rndWorkBuff_ptr = rndWorkBuff_ptr;

    threadHandle = Test_PalThreadCreate(THREAD_STACK_SIZE, Test_LibInit,
                    priority, &params, NULL, 0, true);

    if (threadHandle == NULL) {
        TEST_PRINTF_ERROR("Test_PalThreadCreate failed\n");
        return -1;
    }

    /* Wait till thread is complete before main continues */
    threadRc = Test_PalThreadJoin(threadHandle, (void *)&rc);
    if (threadRc != 0) {
        TEST_PRINTF_ERROR("Test_PalThreadJoin failed\n");
        return -1;
    }

    threadRc = Test_PalThreadDestroy(threadHandle);
    if (threadRc != 0) {
        TEST_PRINTF_ERROR("Test_PalThreadDestroy failed\n");
    }

    return rc;
}
