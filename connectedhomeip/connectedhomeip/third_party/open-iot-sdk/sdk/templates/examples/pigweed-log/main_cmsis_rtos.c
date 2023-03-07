/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cmsis_os2.h"
#include "hal/serial_api.h"

#include <inttypes.h>

#define PW_LOG_MODULE_NAME "main"
#include "pw_log/log.h"

static mdh_serial_t *my_serial = NULL;

extern mdh_serial_t *get_example_serial();

void serial_setup()
{
    my_serial = get_example_serial();
    mdh_serial_set_baud(my_serial, 115200);
}

static void thread_A(void *argument)
{
    (void)argument;
    PW_LOG_DEBUG("Debug log: %s", "Details about the execution");
    PW_LOG_INFO("Info log: %s", "What is going on");
    PW_LOG_WARN("Warning log: %s", "When something unexpected happen");
    PW_LOG_ERROR("Error log: %s", "In case of serious error");
}

int main()
{
    serial_setup();

    osStatus_t ret = osKernelInitialize();
    if (ret != osOK) {
        return -1;
    }
    // Note: initialization of the logging library is done after the kernel initialization
    // as it requires a mutex.
    pw_log_mdh_init(my_serial);

    osThreadId_t tid_A = osThreadNew(thread_A, NULL, NULL);
    if (tid_A != NULL) {
        PW_LOG_INFO("Thread A: ID = 0x%p", tid_A);
    } else {
        PW_LOG_ERROR("Failed to create thread");
        return -1;
    }

    osKernelState_t state = osKernelGetState();
    if (state == osKernelReady) {
        PW_LOG_INFO("Starting kernel");
        ret = osKernelStart();
        if (ret != osOK) {
            PW_LOG_ERROR("Failed to start kernel: %d", ret);
            return -1;
        }
    } else {
        PW_LOG_ERROR("Kernel not ready: %d", state);
        return -1;
    }

    return 0;
}
