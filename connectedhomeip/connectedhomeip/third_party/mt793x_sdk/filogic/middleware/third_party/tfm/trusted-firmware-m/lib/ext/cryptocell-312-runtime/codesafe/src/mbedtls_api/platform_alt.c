/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf printf
#define mbedtls_calloc calloc
#define mbedtls_free   free
#endif

#include "mbedtls/platform.h"

#include <pthread.h>
#include "tests_phys_map.h"
#include "tests_hw_access_iot.h"
#include "tst_common.h"
#include "cc_lib.h"

void* Test_LibInit(void *params ){
   uint32_t rc;
   CClibRetCode_t *pRc =NULL;
   LibInitArgs* threadArgs = (LibInitArgs*)params;

   pRc = (CClibRetCode_t *)malloc(sizeof(CClibRetCode_t));
   if( pRc == NULL){
       return((void *)pRc);
   }
   rc = CC_LibInit(threadArgs->rndContext_ptr, threadArgs->rndWorkBuff_ptr);
   *pRc =rc;

   return((void *)pRc);
}

int tests_CC_libInit(CCRndContext_t* rndContext_ptr, CCRndWorkBuff_t * rndWorkBuff_ptr, unsigned long* stackAddress){
    uint32_t rc = 0;
    void *res;
    pthread_t threadId;
    pthread_attr_t threadAttr;
    LibInitArgs threadArgs;

    threadArgs.rndContext_ptr=rndContext_ptr;
    threadArgs.rndWorkBuff_ptr=rndWorkBuff_ptr;

    rc = pthread_attr_init(&threadAttr);
    mbedtls_printf( "\n'pthread_attr_init' result = %d \n", rc );
    if (rc != 0) {
        goto EndThread_2;
    }

    rc = pthread_attr_setstack(&threadAttr,  stackAddress, THREAD_STACK_SIZE);
    mbedtls_printf( "\n'pthread_attr_setstack' result = %d \n", rc );
    if (rc != 0) {
        goto EndThread;
    }

    rc = pthread_create(&threadId, &threadAttr, Test_LibInit, (void *)&threadArgs);
    mbedtls_printf( "\n'pthread_create' result = %d \n", rc );
    if (rc != 0) {
        goto EndThread;
    }
    rc = pthread_join(threadId, (void**)&res);
    mbedtls_printf( "\n'pthread_join' result = %d \n", rc );
    if (rc != 0 || res == NULL) {
        goto EndThread;
    }

    rc = *((int *)*&res);
    mbedtls_printf( "\n'res' result = %d \n", rc );

EndThread:
    pthread_attr_destroy(&threadAttr);
    free(res);
EndThread_2:
    return rc;
}

//uint32_t platform_start(void)
int mbedtls_platform_setup( mbedtls_platform_context *ctx )
{
   uint32_t rc = 0;
   zynqPlatTestContext_t *pPlatTestCtx = NULL;

   rc = initPlatform();

   mbedtls_printf( "\n'initPlatform' result = %d \n", rc );

   if (rc != 0){
       return rc;
   }

   /* set pointer to user context */
   pPlatTestCtx = (zynqPlatTestContext_t *)ctx;

   pPlatTestCtx->rndContext_ptr = (CCRndContext_t *)g_testHwRndCtxBaseAddr;
   pPlatTestCtx->rndWorkBuff_ptr = (CCRndWorkBuff_t *)(g_testHwRndWorkBuffBaseAddr);
   pPlatTestCtx->rndContext_ptr->rndState = (void *)(g_testHwRndCtxBaseAddr+sizeof(CCRndContext_t));
   pPlatTestCtx->rndContext_ptr->entropyCtx = (void *)(g_testHwRndCtxBaseAddr+sizeof(CCRndContext_t)+sizeof(CCRndState_t));
   rc = tests_CC_libInit(pPlatTestCtx->rndContext_ptr, pPlatTestCtx->rndWorkBuff_ptr, (unsigned long*)g_testHwUserStackBaseAddr);

   mbedtls_printf( "\n'tests_CC_libInit' result = %d \n", rc );

   return rc;
}

//void platform_stop(void)
void mbedtls_platform_teardown( mbedtls_platform_context *ctx )
{
    zynqPlatTestContext_t *pPlatTestCtx = NULL;

    pPlatTestCtx = (zynqPlatTestContext_t *)ctx;

    CC_LibFini(pPlatTestCtx->rndContext_ptr);
    freePlatform();
}







