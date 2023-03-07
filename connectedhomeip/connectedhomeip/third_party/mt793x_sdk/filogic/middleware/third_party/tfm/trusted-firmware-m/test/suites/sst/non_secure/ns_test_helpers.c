/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "ns_test_helpers.h"

#include "os_wrapper/thread.h"
#include "os_wrapper/semaphore.h"

#include "tfm_nspm_api.h"

#define SST_TEST_TASK_STACK_SIZE (768)

struct test_task_t {
    test_func_t *func;
    struct test_result_t *ret;
};

static void *test_semaphore;

/**
 * \brief Executes the supplied test task and then releases the test semaphore.
 *
 * \param[in,out] arg  Pointer to the test task. Must be a pointer to a
 *                     struct test_task_t
 */
static void test_task_runner(void *arg)
{
    struct test_task_t *test = arg;

#ifdef TFM_NS_CLIENT_IDENTIFICATION
    tfm_nspm_register_client_id();
#endif /* TFM_NS_CLIENT_IDENTIFICATION */

    /* Call the test function */
    test->func(test->ret);

    /* Release the semaphore to unblock the parent thread */
    os_wrapper_semaphore_release(test_semaphore);

    /* Signal to the RTOS that the thread is finished */
    os_wrapper_thread_exit();
}

void tfm_sst_run_test(const char *thread_name, struct test_result_t *ret,
                      test_func_t *test_func)
{
    void *current_thread_handle;
    uint32_t current_thread_priority;
    uint32_t err;
    void *thread;
    struct test_task_t test_task = { .func = test_func, .ret = ret };

    /* Create a binary semaphore with initial count of 0 tokens available */
    test_semaphore = os_wrapper_semaphore_create(1, 0, "sst_tests_sema");
    if (!test_semaphore) {
        TEST_FAIL("Semaphore creation failed");
        return;
    }

    current_thread_handle = os_wrapper_thread_get_handle();
    if (!current_thread_handle) {
        os_wrapper_semaphore_delete(test_semaphore);
        TEST_FAIL("Failed to get current thread ID");
        return;
    }

    err = os_wrapper_thread_get_priority(current_thread_handle,
                                         &current_thread_priority);
    if (err == OS_WRAPPER_ERROR) {
        os_wrapper_semaphore_delete(test_semaphore);
        TEST_FAIL("Failed to get current thread priority");
        return;
    }

    /* Start test thread */
    thread = os_wrapper_thread_new(thread_name, SST_TEST_TASK_STACK_SIZE,
                                   test_task_runner, &test_task,
                                   current_thread_priority);
    if (!thread) {
        os_wrapper_semaphore_delete(test_semaphore);
        TEST_FAIL("Failed to create test thread");
        return;
    }

    /* Signal semaphore, wait indefinitely until unblocked by child thread */
    err = os_wrapper_semaphore_acquire(test_semaphore, OS_WRAPPER_WAIT_FOREVER);

    /* At this point, it means the binary semaphore has been released by the
     * test and re-acquired by this thread, so just finally release it and
     * delete it
     */
    os_wrapper_semaphore_release(test_semaphore);

    os_wrapper_semaphore_delete(test_semaphore);
}
