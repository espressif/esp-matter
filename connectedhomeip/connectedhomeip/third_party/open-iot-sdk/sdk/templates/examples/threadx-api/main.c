/*
 * Copyright (c) 2021-2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "tx_api.h"

#include <inttypes.h>
#include <stdio.h>

#define STACK_SIZE_BYTES 0x400

static TX_THREAD thread_a_cb;
static TX_THREAD thread_b_cb;
static uint8_t thread_a_stack[STACK_SIZE_BYTES];
static uint8_t thread_b_stack[STACK_SIZE_BYTES];

static TX_MUTEX mutex;

static void thread_A(ULONG argument)
{
    (void)argument;

    const ULONG delay = 500;

    for (int i = 0; i < 5; ++i) {
        if (tx_mutex_get(&mutex, TX_WAIT_FOREVER) == TX_SUCCESS) {
            printf("Message from Thread A\r\n");
        }
        tx_mutex_put(&mutex);

        tx_thread_sleep(delay);
    }
}

static void thread_B(ULONG argument)
{
    (void)argument;

    const ULONG delay = 500;

    for (int i = 0; i < 5; ++i) {
        if (tx_mutex_get(&mutex, TX_WAIT_FOREVER) == TX_SUCCESS) {
            printf("Message from Thread B\r\n");
        }
        tx_mutex_put(&mutex);

        tx_thread_sleep(delay);
    }
}

/* Create RTOS objects. This is called by ThreadX as part of the kernel
 * initialization process. */
void tx_application_define(void *first_unused_memory)
{
    (void)first_unused_memory;

    UINT ret;

    printf("Inside tx_application_define()\r\n");

    ret = tx_mutex_create(&mutex, "Mutex", TX_NO_INHERIT);
    if (ret != TX_SUCCESS) {
        printf("Error: Failed to create mutex\r\n");
        return;
    }

    printf("Creating threads\r\n");

    ret = tx_thread_create(&thread_a_cb,     /* thread_ptr */
                           "ThreadA",        /* name_ptr */
                           thread_A,         /* entry_function */
                           0ul,              /* entry_input */
                           &thread_a_stack,  /* stack_start */
                           STACK_SIZE_BYTES, /* stack_size */
                           2u,               /* priority */
                           2u,               /* preempt_threshold */
                           TX_NO_TIME_SLICE, /* time_slice */
                           TX_AUTO_START);   /* auto_start */
    if (ret != TX_SUCCESS) {
        printf("Error: Failed to create Thread A - tx_thread_create() returned %d\r\n", ret);
        return;
    }
    printf("Thread A ID 0x%" PRIxPTR "\r\n", (uintptr_t)&thread_a_cb);

    ret = tx_thread_create(&thread_b_cb,     /* thread_ptr */
                           "ThreadB",        /* name_ptr */
                           thread_B,         /* entry_function */
                           0ul,              /* entry_input */
                           &thread_b_stack,  /* stack_start */
                           STACK_SIZE_BYTES, /* stack_size */
                           2u,               /* priority */
                           2u,               /* preempt_threshold */
                           TX_NO_TIME_SLICE, /* time_slice */
                           TX_AUTO_START);   /* auto_start */
    if (ret != TX_SUCCESS) {
        printf("Error: Failed to create Thread B - tx_thread_create() returned %d\r\n", ret);
        return;
    }
    printf("Thread B ID 0x%" PRIxPTR "\r\n", (uintptr_t)&thread_b_cb);
}

int main()
{
    printf("Inside main()\r\n");

    /* Initialize and start ThreadX kernel */
    tx_kernel_enter();
}
