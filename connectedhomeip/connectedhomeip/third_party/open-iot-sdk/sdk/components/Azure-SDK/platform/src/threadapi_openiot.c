/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/xlogging.h"
#include "cmsis_os2.h"

#include <stdlib.h>

typedef struct thread_context_s {
    THREAD_START_FUNC func;
    void *arg;
    osThreadId_t thread_id;
    osMessageQueueId_t queue;
    struct thread_context_s *next;
    struct thread_context_s *prev;
} thread_context_t;

static thread_context_t *threads_head = NULL;
static thread_context_t *threads_last = NULL;

/** Translate osStatus_t error code to THREADAPI_RESULT code */
static THREADAPI_RESULT os_status_to_threadapi_result(osStatus_t status)
{
    switch (status) {
        case osOK:
            return THREADAPI_OK;
        case osErrorParameter:
            return THREADAPI_INVALID_ARG;
        case osErrorNoMemory:
            return THREADAPI_NO_MEMORY;
        case osError:
        case osErrorISR:
        case osErrorResource:
        case osErrorTimeout:
            return THREADAPI_ERROR;
        default:
            LogError("Unknown osStatus_t=%d", (int)status);
            break;
    }

    return THREADAPI_ERROR;
}

/** Find the `thread_context` for the thread with ID `id`, or NULL on error (no threads executing/invalid id).
 * This is done by linear search using an unsorted linked list.
 */
static thread_context_t *get_current_thread_context(void)
{
    if (!threads_head) {
        return NULL;
    }

    osThreadId_t thread_id = osThreadGetId();
    if (!thread_id) {
        LogError("osThreadGetId failed");
        return thread_id;
    }

    thread_context_t *current = threads_head;
    while (current && current->thread_id != thread_id) {
        current = current->next;
    }

    return current;
}

/** Clear and destroy thread context */
static THREADAPI_RESULT destroy_thread_context(thread_context_t *context)
{
    if (!context) {
        LogError("Thread context is NULL");
        return THREADAPI_INVALID_ARG;
    }

    if (context->queue) {
        osStatus_t status = osMessageQueueDelete(context->queue);
        THREADAPI_RESULT res = os_status_to_threadapi_result(status);
        if (res != THREADAPI_OK) {
            LogError("osMessageQueueDelete failed %d", status);
            return res;
        }
    }

    if (context->prev) {
        context->prev->next = context->next;
    }

    free(context);

    return THREADAPI_OK;
}

/** Exit from current thread.
 * We need to use the message queue to pass exit result to thread join function.
 */
static THREADAPI_RESULT exit_thread(thread_context_t *context, int res)
{
    if (!context) {
        LogError("Thread context is NULL");
        return THREADAPI_INVALID_ARG;
    }

    osStatus_t status = osMessageQueuePut(context->queue, &res, 0, osWaitForever);
    THREADAPI_RESULT ret = os_status_to_threadapi_result(status);
    if (ret != THREADAPI_OK) {
        LogError("osMessageQueuePut failed %d", status);
        return ret;
    }

    osThreadExit();

    return ret;
}

/** Translate thread callback from Azure thread to CMSIS RTOS. */
static void thread_wrapper_callback(void *p_context)
{
    if (!p_context) {
        LogError("Thread context is NULL");
        return;
    }

    thread_context_t *context = (thread_context_t *)p_context;
    int res = (int)context->func(context->arg);
    if ((res = exit_thread(context, res)) != 0) {
        LogError("Thread termination failed %d", res);
    }
}

THREADAPI_RESULT ThreadAPI_Create(THREAD_HANDLE *threadHandle, THREAD_START_FUNC func, void *arg)
{
    if (!threadHandle || !func) {
        return THREADAPI_INVALID_ARG;
    }

    thread_context_t *context = calloc(1, sizeof(thread_context_t));
    if (!context) {
        LogError("Create thread contex failed");
        return THREADAPI_NO_MEMORY;
    }

    if (threads_last) {
        context->prev = threads_last;
        threads_last->next = context;
        threads_last = context;
    } else {
        threads_head = context;
        threads_last = context;
    }

    context->func = func;
    context->arg = arg;

    context->queue = osMessageQueueNew(1, sizeof(int), NULL);
    if (!(context->queue)) {
        LogError("Create message queue failed");
        destroy_thread_context(context);
        return THREADAPI_ERROR;
    }

    context->thread_id = osThreadNew(thread_wrapper_callback, context, NULL);
    if (!context->thread_id) {
        LogError("Create new thread failed");
        destroy_thread_context(context);
        return THREADAPI_ERROR;
    }

    *threadHandle = context;
    return THREADAPI_OK;
}

THREADAPI_RESULT ThreadAPI_Join(THREAD_HANDLE threadHandle, int *res)
{
    THREADAPI_RESULT ret;
    if (!threadHandle || !res) {
        return THREADAPI_INVALID_ARG;
    }

    thread_context_t *context = (thread_context_t *)threadHandle;
    osStatus_t status = osMessageQueueGet(context->queue, res, NULL, osWaitForever);
    ret = os_status_to_threadapi_result(status);

    if (ret == THREADAPI_OK) {
        ret = destroy_thread_context(context);
    }

    return ret;
}

void ThreadAPI_Exit(int res)
{
    int ret;

    thread_context_t *context = get_current_thread_context();
    if (!context) {
        LogError("Could not find context for current thread");
        return;
    }

    if ((ret = exit_thread(context, res)) != 0) {
        LogError("Exit current thread failed %d", ret);
    }
}

void ThreadAPI_Sleep(unsigned milliseconds)
{
    // Convert milliseconds to kernel ticks for delay.
    uint32_t ticks = (uint32_t)(osKernelGetTickFreq() // -> Hz
                                / 1000.0              // -> msec^-1
                                * milliseconds        // -> ticks
    );
    osStatus_t status = osDelay(ticks);
    if (status != osOK) {
        LogError("osDelay failed %d", status);
    }
}
