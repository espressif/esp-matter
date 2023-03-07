/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include "tst_common.h"
#include "test_log.h"
#include "tests_phys_map.h"
#include "tst_common.h"

#ifdef CC_IOT
#include "tests_hw_access_iot.h"
#else
#include "tests_hw_access.h"
#endif

#ifdef CC_SUPPORT_FIPS
#define CC_REE_BASE  0x80000000 // same as shared/hw/ree_include/dx_reg_base_host.h DX_BASE_CC
#define CC_REE_AREA_LEN  0x100000
#define DX_HOST_GPR0_REG_OFFSET     0xA70UL   // taken from ree_include/dx_host.h

int testSetReeFipsError(uint32_t  reeError, CCFipsState_t expfipsState)
{
    uint32_t rc;
    CCFipsState_t fipsState;
    uint32_t maxRetries = 20;

    rc = TestMapRee();
    if (rc != 0) {
        return 1;
    }
    WRITE_REE_REG(DX_HOST_GPR0_REG_OFFSET, (unsigned int)(reeError));

    TestMunMapRee();

    TEST_PRINTF("waiting for fips stat to be %d\n", expfipsState);
    do {
        rc = CC_FipsStateGet(&fipsState, NULL);
        if (rc != 0) {
            return 1;
        }
        usleep(100); // wait 100 milisecond
        TEST_PRINTF("fips stat is %d\n", fipsState);
    } while ((fipsState != expfipsState) && (maxRetries-- > 0));
    if ((maxRetries == 0) && (fipsState != expfipsState)) {
        return 1;
    }
    TEST_PRINTF("doen OK\n");
    return 0;
}
#endif

/**
 * The function copies src buffer to dst with reversed bytes order.
 *
 * Note: Overlapping is not allowed, besides reversing of the buffer in place.
 *
 * @param dst
 * @param src
 * @param sizeInBytes
 */
int TST_MemCpyReversed( void* dst_ptr, void* src_ptr, unsigned int sizeInBytes)
{
    unsigned int i;
    unsigned char *dst, *src;

    src = (unsigned char *)src_ptr;
    dst = (unsigned char *)dst_ptr;

    if (((dst < src) && (dst+sizeInBytes > src)) ||
        ((src < dst) && (src+sizeInBytes > dst)))
        return -1;

    if (dst == src) {
        unsigned char tmp;
        for (i=0; i<sizeInBytes/2; i++) {
            tmp = dst[sizeInBytes-i-1];
            dst[sizeInBytes-i-1] = src[i];
            src[i] = tmp;
        }
    } else {
        for (i=0; i<sizeInBytes; i++) {
            dst[i] = src[sizeInBytes-i-1];
        }
    }

    return 0;
}



#ifdef CC_IOT

 void* Test_LibInit(void *params ){
    uint32_t rc;
    uint8_t             *pRc =NULL;
    LibInitArgs* threadArgs = (LibInitArgs*)params;

    pRc = (uint8_t *)malloc(sizeof(uint8_t));
    rc = CC_LibInit(threadArgs->rndContext_ptr, threadArgs->rndWorkBuff_ptr);
    *pRc =rc;
    return((void *)pRc);

}
int tests_CC_libInit(CCRndContext_t* rndContext_ptr, CCRndWorkBuff_t * rndWorkBuff_ptr, unsigned long* stackAddress){
    int rc = 0;
    void *res;
    pthread_t threadId;
    pthread_attr_t threadAttr;
    LibInitArgs threadArgs;
    threadArgs.rndContext_ptr=rndContext_ptr;
    threadArgs.rndWorkBuff_ptr=rndWorkBuff_ptr;
    rc = pthread_attr_init(&threadAttr);

    if (rc != 0) {
        return rc;
    }

    rc = pthread_attr_setstack(&threadAttr,  stackAddress, THREAD_STACK_SIZE);
    if (rc != 0) {
        return rc;
    }

    rc = pthread_create(&threadId, &threadAttr, Test_LibInit, (void *)&threadArgs);

    rc = pthread_join(threadId, (void**)&res);
    if (rc != 0) {
        return rc;
    }

    rc = pthread_attr_destroy(&threadAttr);

    return rc;
}

#endif
