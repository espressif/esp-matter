/*
 * Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cmsis_os2.h"

#include <inttypes.h>
#include <stdio.h>

// Thread running PSA services
void psa_thread(void *argument);

int main()
{
    printf("In main()\r\n");

    // Initialize CMSIS RTOS v2 and create a thread for PSA services
    osStatus_t os_status = osKernelInitialize();
    if (os_status != osOK) {
        printf("osKernelInitialize failed: %d\r\n", os_status);
        return -1;
    }
    osThreadId_t tid = osThreadNew(psa_thread, NULL, NULL);
    if (tid != NULL) {
        printf("Thread ID for PSA services: 0x%" PRIxPTR "\r\n", tid);
    } else {
        printf("Failed to create thread\r\n");
        return -1;
    }
    osKernelState_t os_state = osKernelGetState();
    if (os_state != osKernelReady) {
        printf("Kernel not ready %d\r\n", os_state);
        return -1;
    }

    printf("Starting kernel and threads\r\n");
    os_status = osKernelStart();
    if (os_status != osOK) {
        printf("Failed to start kernel: %d\r\n", os_status);
        return -1;
    }

    return 0;
}
