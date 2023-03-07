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
#include "FreeRTOS.h"
#include "task.h"
#include "projdefs.h"
#include "test_pal_thread.h"
#include "test_pal_mem.h"
#include "test_pal_log.h"
#ifdef TZM
#include <arm_cmse.h>
#include "test_pal_mem_s.h"
#endif

struct ThreadStr {
    TaskHandle_t    taskHandle;     /* Thread handle */

    TaskHandle_t    parentTaskHandle;   /* Handle of the task who called
                         * Test_PalThreadCreate */

    void        *(*taskFunc)(void *);   /* thread function pointer */

    void        *arg;           /* Arguments received by thread
                         * function. */
};

/* FreeRTOS Priorities */
#define FREERTOS_HIGHEST_TASK_PRIORITY      (configMAX_PRIORITIES - 1)
#define FREERTOS_LOWEST_TASK_PRIORITY       tskIDLE_PRIORITY
#define FREERTOS_DEFAULT_TASK_PRIORITY      2
/* FREERTOS_DEFAULT_TASK_PRIORITY Depends on other tasks in the FreeRTOS
 *  environment. See FreeRTOSConfig.h */

#define FREERTOS_MINIMAL_STACK_SIZE_WORDS   configMINIMAL_STACK_SIZE

/******************************************************************************/
/*              Internal API                      */
/******************************************************************************/
void Test_PalThreadFunc(void *pvParameter)
{
    /* Calls task function */
    struct ThreadStr *threadStr = pvParameter;

    threadStr->taskFunc(threadStr->arg);

    /* Sends a notification for the parent task */
    if (threadStr->parentTaskHandle != NULL)
        xTaskNotifyGive(threadStr->parentTaskHandle);

    threadStr->taskHandle = NULL;

    /* suicide */
    vTaskDelete(NULL);
}

/******************************************************************************/
/* Note: When TrustZone-M is supported, it is required to ask whether the     */
/* caller is non-secure or secure in order to allocate a stack with the same  */
/* attribute                                                                  */
/******************************************************************************/

static void *PalMalloc(size_t size)
{
#ifndef TZM
    return Test_PalMalloc(size);
#else
    if (cmse_is_nsfptr(&PalMalloc)) /* A non-secure caller */
        return Test_PalMalloc(size);
    return Test_PalMalloc_s(size); /* A secure caller */
#endif
}

static void PalFree(void *pvAddress)
{
#ifndef TZM
    Test_PalFree(pvAddress);
#else
    if (cmse_is_nsfptr(&PalFree)) /* A non-secure caller */
        Test_PalFree(pvAddress);
    Test_PalFree_s(pvAddress); /* A secure caller */
#endif
}

/******************************************************************************/
/*              External API                      */
/******************************************************************************/
size_t Test_PalGetMinimalStackSize(void)
{
    return FREERTOS_MINIMAL_STACK_SIZE_WORDS*4;
}

/******************************************************************************/
uint32_t Test_PalGetHighestPriority(void)
{
    return FREERTOS_HIGHEST_TASK_PRIORITY;
}

/******************************************************************************/
uint32_t Test_PalGetLowestPriority(void)
{
    return FREERTOS_LOWEST_TASK_PRIORITY;
}

/******************************************************************************/
uint32_t Test_PalGetDefaultPriority(void)
{
    return FREERTOS_DEFAULT_TASK_PRIORITY;
}

/******************************************************************************/
ThreadHandle Test_PalThreadCreate(size_t stackSize,
                  void *(*threadFunc)(void *),
                  int priority, void *arg,
                  const char *threadName,
                  uint8_t nameLen, uint8_t dmaAble)
{
    char buff[8];
    size_t stackSizeInWords, minimalStackSize;
    struct ThreadStr *threadStr;
    (void)dmaAble;

    /* Checks argument validity */
    minimalStackSize = Test_PalGetMinimalStackSize();
    if (stackSize < minimalStackSize) {
        TEST_PRINTF_ERROR("Stack size is too small. Changed to"
                "minimal stack size in bytes: %d\n",
                minimalStackSize);
        stackSize = minimalStackSize;
    }
    /* Casting is used in order to avoid compilation warnings */
    if ((priority < (int)FREERTOS_LOWEST_TASK_PRIORITY) ||
        (priority > (int)FREERTOS_HIGHEST_TASK_PRIORITY)) {
        TEST_PRINTF_ERROR("Priority is not vaild\n");
        return NULL;
    }

#ifdef TZM
    /* Thread function must have the same security attribute as
     * TestAL's (either secure or non-secure) */
    if(cmse_is_nsfptr(threadFunc) != cmse_is_nsfptr(&Test_PalThreadCreate))
        return NULL;
#endif

    /* Allocates ThreadStr */
    threadStr = PalMalloc(sizeof(struct ThreadStr));
    if (threadStr == NULL) {
        TEST_PRINTF_ERROR("threadStr allocation failed\n");
        return NULL;
    }

    /* Initializes ThreadStr */
    threadStr->taskFunc = threadFunc;
    threadStr->arg = arg;

    memset(buff, 0, sizeof(buff));
    memcpy(buff, threadName, ((nameLen > 8) ? 8:nameLen));
    stackSizeInWords = (stackSize + 3) >> 2;

    /* Creates thread */
    if (xTaskCreate(Test_PalThreadFunc, buff, stackSizeInWords, threadStr,
            priority, &threadStr->taskHandle) == pdFAIL) {
        TEST_PRINTF_ERROR("thread creation failed\n");
        PalFree(threadStr);
        return NULL;
    }

    /* New task was created. Save current task handle for future
     * notifications */
    threadStr->parentTaskHandle = xTaskGetCurrentTaskHandle();
    return threadStr;
}

/******************************************************************************/
uint32_t Test_PalThreadJoin(ThreadHandle threadHandle, void **threadRet)
{
    struct ThreadStr *threadStr = (struct ThreadStr *)threadHandle;
    (void)threadRet;

    if(threadStr->taskHandle != NULL )
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

    return 0;
}

/******************************************************************************/
uint32_t Test_PalThreadDestroy(ThreadHandle threadHandle)
{
    /* Remember the value of the variable as it
    will be set to NULL. */
    struct ThreadStr *threadStr = (struct ThreadStr *)threadHandle;
    TaskHandle_t xTask = threadStr->taskHandle;

    vTaskSuspendAll();

    if(threadStr->taskHandle != NULL ) {
        /* The task is going to be deleted. Set the handle to NULL */
        threadStr->taskHandle = NULL;

        /* Delete using the copy of the handle. */
        vTaskDelete( xTask );
    }
    xTaskResumeAll();

    PalFree(threadStr);

    return 0;
}
