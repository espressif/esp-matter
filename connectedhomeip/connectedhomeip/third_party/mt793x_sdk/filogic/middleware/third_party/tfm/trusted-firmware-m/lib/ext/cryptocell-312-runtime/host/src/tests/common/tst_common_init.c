/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "tst_common.h"
#include "tst_common_init.h"
#include "test_log.h"

int tests_CC_libInit_wrap(CCRndContext_t* rndContext_ptr, CCRndWorkBuff_t * rndWorkBuff_ptr){
    uint32_t rc = 0;
    int threadRc;
    void *threadRet;
    ThreadHandle threadHandle;
    LibInitArgs params;
    params.rndContext_ptr=rndContext_ptr;
    params.rndWorkBuff_ptr=rndWorkBuff_ptr;

    threadHandle = Test_PalThreadCreate(THREAD_STACK_SIZE, (void *)Test_LibInit, &params, NULL, 0, true);
    if (threadHandle == NULL) {
        TEST_PRINTF_ERROR("Test_PalThreadCreate failed\n");
        return -1;
    }

    /* Wait till thread is complete before main continues */
    threadRc = Test_PalThreadJoin(threadHandle, &threadRet);
    if (threadRc != 0) {
        TEST_PRINTF_ERROR("Test_PalThreadJoin failed\n");
        return -1;
    }

    rc =*((uint32_t *)*&threadRet);

    threadRc = Test_PalThreadDestroy(threadHandle);
    if (threadRc != 0) {
        TEST_PRINTF_ERROR("pthread_attr_destroy failed\n");
    }

    return rc;
}
