/*******************************************************************************
* The confidential and proprietary information contained in this file may      *
* only be used by a person authorised under and to the extent permitted        *
* by a subsisting licensing agreement from ARM Limited or its affiliates.      *
*   (C) COPYRIGHT [2001-2017] ARM Limited or its affiliates.                   *
*       ALL RIGHTS RESERVED                                                    *
* This entire notice must be reproduced on all copies of this file             *
* and copies of this file may only be made by a person if such person is       *
* permitted to do so under the terms of a subsisting license agreement         *
* from ARM Limited or its affiliates.                                          *
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "test_pal_thread.h"
#include "test_pal_mem.h"
#include "test_pal_log.h"


struct ThreadStr {
    pthread_t   threadId;   /* Thread ID */

    void        *(*threadFunc)(void *); /* thread function pointer */

    void        *arg;       /* Arguments received by thread
                     * function. */

    void        *stackAddrToFree; /* Stack Address that needs to be
                       * freed. If NULL, stack is not
                       * dma-able and was allocated by
                       * the OS. */

    uint8_t     isJoined;   /* Indicates whether pthread_join
                     * was called or not. This function is
                     * necessary for cleaning up any
                     * resources associated with the thread.
                     * Can be called only once. */

    uint8_t     isCompleted;    /* Indicates whether the thread finished
                     * to run. */
};

/* minimum stack size is 1MB */
#define LINUX_MINIMAL_STACK_SIZE_BYTES  0x100000
/* stack alignment to page size */
#define STACK_ALIGN         (sysconf(_SC_PAGESIZE))

/******************************************************************************/
void *Test_PalThreadFunc(void *ctx)
{
    struct ThreadStr *threadStr = ctx;
    void *ret = NULL;

    /* The thread is cancelable */
    if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)) {
        TEST_PRINTF_ERROR("pthread_setcancelstate failed\n");
        goto exit;
    }

    /* The thread can be cancelled at any time. */
    if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL)) {
        TEST_PRINTF_ERROR("pthread_setcanceltype failed\n");
        goto exit;
    }

    ret = threadStr->threadFunc(threadStr->arg);
    threadStr->isCompleted = true;

exit:
    pthread_exit(ret);
    /*Should not reach here. The line below prevents a compilation warning*/
    return ret;
}

/******************************************************************************/
size_t Test_PalGetMinimalStackSize(void)
{
    return LINUX_MINIMAL_STACK_SIZE_BYTES;
}

/******************************************************************************/
uint32_t Test_PalGetHighestPriority(void)
{
    return 0;
}

/******************************************************************************/
uint32_t Test_PalGetLowestPriority(void)
{
    return 0;
}

/******************************************************************************/
uint32_t Test_PalGetDefaultPriority(void)
{
    return 0;
}

/******************************************************************************/
ThreadHandle Test_PalThreadCreate(size_t stackSize,
                  void *(*threadFunc)(void *),
                  int priority, void *arg,
                  const char *threadName,
                  uint8_t nameLen, uint8_t dmaAble)
{
    (void)threadName;
    (void)nameLen;
    (void)priority;
    struct ThreadStr *threadStr = NULL;
    void *stackAddr = NULL;
    pthread_attr_t attr;

    /* Checks argument validity */
    if (stackSize < LINUX_MINIMAL_STACK_SIZE_BYTES) {
        TEST_PRINTF_ERROR("Stack size is too small. Changed to "
                "minimal stack size in bytes: 0x%08lx\n",
                LINUX_MINIMAL_STACK_SIZE_BYTES);
        stackSize = LINUX_MINIMAL_STACK_SIZE_BYTES;
    }

    /* Allocates ThreadStr */
    threadStr = Test_PalMalloc(sizeof(struct ThreadStr));
    if (threadStr == NULL) {
        TEST_PRINTF_ERROR("threadStr allocation failed\n");
        goto error;
    }

    /* Initializes thread attribute */
    if (pthread_attr_init(&attr) != 0) {
        TEST_PRINTF_ERROR("pthread_attr_init failed\n");
        goto error_freeStr;
    }

    /* Initializes ThreadStr */
    threadStr->threadFunc = threadFunc;
    threadStr->arg = arg;
    threadStr->stackAddrToFree = NULL;
    threadStr->isJoined = false;
    threadStr->isCompleted = false;

    /* Allocates thread stack */
    if (dmaAble) {
        /* Allocate the stack + extra for page alignment */
        threadStr->stackAddrToFree = Test_PalDMAContigBufferAlloc(
                stackSize + LINUX_MINIMAL_STACK_SIZE_BYTES);
        if (threadStr->stackAddrToFree == NULL) {
            TEST_PRINTF_ERROR("thread stack allocation failed\n");
            goto error_freeStr;
        }
        /* Align stack size to be multiple of page size. */
        stackAddr = (void *)(threadStr->stackAddrToFree +
                    STACK_ALIGN - 1);
        stackAddr = (void *)((size_t)stackAddr & (-STACK_ALIGN));

        if (pthread_attr_setstack(&attr, stackAddr, stackSize) != 0) {
            TEST_PRINTF_ERROR("pthread_attr_setstack failed\n");
            goto error_freeAll;
        }
    }
    else {
        if (pthread_attr_setstacksize(&attr, stackSize) != 0) {
            TEST_PRINTF_ERROR("pthread_attr_setstacksize failed\n");
            goto error_freeStr;
        }
    }

    /* Creates thread */
    if (pthread_create(&threadStr->threadId, &attr,
               Test_PalThreadFunc, threadStr)) {
        TEST_PRINTF_ERROR("pthread_create failed\n");
        goto error_freeAll;
    }

    /* Cannot free thread stack and context in case destroy attribute
     * fails as the thread is already running */
    if (pthread_attr_destroy(&attr))
        TEST_PRINTF_ERROR("pthread_attr_destroy failed\n");

    return threadStr;

error_freeAll:
    if (dmaAble)
        Test_PalDMAContigBufferFree(threadStr->stackAddrToFree);
error_freeStr:
    Test_PalFree(threadStr);
error:
    return NULL;
}

/******************************************************************************/
uint32_t Test_PalThreadJoin(ThreadHandle threadHandle, void **threadRet)
{
    struct ThreadStr *threadStr = (struct ThreadStr *)threadHandle;

    /* Calling to pthread_join. threadRet is not changed. *threadRet is
       Changed and can be set to NULL */
    if (pthread_join(threadStr->threadId, threadRet)) {
        TEST_PRINTF_ERROR("pthread_join failed\n");
        return 1;
    }

    threadStr->isJoined = true;
    return 0;
}

/******************************************************************************/
uint32_t Test_PalThreadDestroy(ThreadHandle threadHandle)
{
    struct ThreadStr *threadStr = (struct ThreadStr *)threadHandle;

    if (threadStr->isCompleted == false) {
        if (pthread_cancel(threadStr->threadId)) {
            TEST_PRINTF_ERROR("pthread_cancel failed\n");
            return 1;
        }
        threadStr->isCompleted = true;
    }

    /* pthread_join is necessary for cleaning up any resources associated
     * with the thread. Can be called only once */
    if (threadStr->isJoined == false) {
        /* Cannot return because thread is already cancelled.
         * Must continue freeing resources */
        if (pthread_join(threadStr->threadId, NULL)) {
            TEST_PRINTF_ERROR("pthread_join failed\n");
        }
        threadStr->isJoined = true;
    }

    /* Frees stack only in case its Dma-able */
    if (threadStr->stackAddrToFree != NULL)
        Test_PalDMAContigBufferFree(threadStr->stackAddrToFree);

    Test_PalFree(threadStr);
    return 0;
}
