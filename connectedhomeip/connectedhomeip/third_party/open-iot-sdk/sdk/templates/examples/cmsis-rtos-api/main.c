/*
 * Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cmsis_os2.h"

#include <inttypes.h>
#include <stdio.h>

osMutexId_t gMutex;

static void thread_A(void *argument)
{
    (void)argument;
    for (int i = 0; i < 5; ++i) {
        if ((osMutexAcquire(gMutex, 10)) != osOK) {
            printf("Failed to acquire mutex in Thread A!\r\n");
            return;
        }

        printf("Message from Thread A\r\n");

        if ((osMutexRelease(gMutex)) != osOK) {
            printf("Failed to release mutex in Thread A!\r\n");
        }
        osDelay(500);
    }
}

static void thread_B(void *argument)
{
    (void)argument;
    for (int i = 0; i < 5; ++i) {
        if ((osMutexAcquire(gMutex, 10)) != osOK) {
            printf("Failed to acquire mutex in Thread B!\r\n");
            return;
        }

        printf("Message from Thread B\r\n");

        if ((osMutexRelease(gMutex)) != osOK) {
            printf("Failed to release mutex in Thread B!\r\n");
        }
        osDelay(500);
    }
}

int main()
{
    printf("Inside main()\r\n");

    osStatus_t ret = osKernelInitialize();
    if (ret != osOK) {
        printf("osKernelInitialize failed: %d\r\n", ret);
        return -1;
    }

    static const osMutexAttr_t mutex_attr = {
        "myThreadMutex", // human readable mutex name
        0U,              // attr_bits
        NULL,            // memory for control block (if NULL use dynamic allocation)
        0U               // size for control block (if 0 use dynamic allocation)
    };
    gMutex = osMutexNew(&mutex_attr);
    if (gMutex == NULL) {
        printf("Failed to create mutex!\r\n");
        return -1;
    }

    static const osThreadAttr_t thread_attr = {
        .stack_size = 1024 // Allocate our threads with enough stack for printf
    };
    osThreadId_t tid_A = osThreadNew(thread_A, NULL, &thread_attr);
    if (tid_A != NULL) {
        printf("Thread A: ID = 0x%" PRIxPTR "\r\n", (unsigned int)tid_A);
    } else {
        printf("Failed to create thread\r\n");
        return -1;
    }

    osThreadId_t tid_B = osThreadNew(thread_B, NULL, &thread_attr);
    if (tid_B != NULL) {
        printf("Thread B: ID = 0x%" PRIxPTR "\r\n", (unsigned int)tid_B);
    } else {
        printf("Failed to create thread\r\n");
        return -1;
    }

    osKernelState_t state = osKernelGetState();
    if (state == osKernelReady) {
        printf("Starting kernel\r\n");
        ret = osKernelStart();
        if (ret != osOK) {
            printf("Failed to start kernel: %d\r\n", ret);
            return -1;
        }
    } else {
        printf("Kernel not ready: %d\r\n", state);
        return -1;
    }

    return 0;
}
