/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "iotsdk/ip_network_api.h"

#include "arch/sys_arch.h"
#include "lwip_stack.h"

#ifndef IOTSDK_LWIP_INPUT_TASK_THREAD_PRIORITY
#define IOTSDK_LWIP_INPUT_TASK_THREAD_PRIORITY osPriorityAboveNormal
#endif

typedef void (*lwip_thread_fn)(void *arg);
extern sys_thread_t sys_thread_new(const char *name, lwip_thread_fn function, void *arg, int stacksize, int prio);

static sys_thread_t net_thread = 0;

osStatus_t start_network_task(network_state_callback_t network_state_callback, uint32_t stack_size)
{
    if (net_thread) {
        return osError;
    }

    net_thread =
        sys_thread_new(NULL, lwip_task, network_state_callback, stack_size, IOTSDK_LWIP_INPUT_TASK_THREAD_PRIORITY);

    if (!net_thread) {
        return osError;
    }

    return osOK;
}
