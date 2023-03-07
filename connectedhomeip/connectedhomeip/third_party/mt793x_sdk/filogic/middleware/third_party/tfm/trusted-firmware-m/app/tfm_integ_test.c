/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "tfm_api.h"
#include "cmsis_os2.h"
#include "tfm_nspm_api.h"

#include "tfm_integ_test.h"
#include "test/framework/test_framework_integ_test.h"
#ifdef TFM_PSA_API
#include "psa_manifest/sid.h"
#endif

#ifdef TEST_FRAMEWORK_S
#include \
  "test/test_services/tfm_secure_client_service/tfm_secure_client_service_api.h"
#endif

#ifdef CORE_TEST_INTERACTIVE
#include "test/test_services/tfm_core_test/core_test_defs.h"
#include "tfm_veneers.h"

#define TRY_SFN(fn, ...) \
    do { \
        enum tfm_status_e res = (enum tfm_status_e) fn(__VA_ARGS__); \
        switch(res) { \
            case TFM_SUCCESS: \
                LOG_MSG("Secure call to " #fn "(" #__VA_ARGS__") successful!");\
                break; \
            case TFM_ERROR_SECURE_DOMAIN_LOCKED: \
                LOG_MSG("Secure call to " #fn "(" #__VA_ARGS__") failed, " \
                                                           "S domain locked!");\
                break; \
            default: \
                LOG_MSG("Secure call to " #fn "(" #__VA_ARGS__") failed, " \
                                                                   "generic!");\
        } \
    } while(0)

#ifdef TFM_PSA_API
static psa_status_t psa_test_common(uint32_t sid, uint32_t version,
                                    const psa_invec *in_vecs, size_t in_len,
                                    psa_outvec *out_vecs, size_t out_len)
{
    psa_handle_t handle;
    psa_status_t status;

    handle = psa_connect(sid, version);
    if (handle <= 0) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    status = psa_call(handle, PSA_IPC_CALL, in_vecs, in_len, out_vecs, out_len);
    if (status < 0) {
        status = CORE_TEST_ERRNO_UNEXPECTED_CORE_BEHAVIOUR;
    }

    psa_close(handle);
    return status;
}
#endif /* TFM_PSA_API */

/**
 * \brief secure_decrement_ns_lock_1
 *
 */
void secure_decrement_ns_lock_1(void)
{
#ifndef TFM_PSA_API
    uint32_t testcase_id = CORE_TEST_ID_BLOCK;
    psa_invec in_vec = {&testcase_id, sizeof(testcase_id)};

    TRY_SFN(tfm_spm_core_test_sfn_veneer, &in_vec, 1, NULL, 0);
#else
    psa_status_t err;

    err = psa_test_common(SPM_CORE_TEST_BLOCK_SID,
                          SPM_CORE_TEST_BLOCK_VERSION,
                          NULL, 0, NULL, 0);
    if (err != PSA_SUCCESS) {
        LOG_MSG("Secure call to sfn block failed, generic!");
    }
#endif
}

/**
 * \brief secure_decrement_ns_lock_2
 *
 */
void secure_decrement_ns_lock_2(void)
{
#ifndef TFM_PSA_API
    uint32_t testcase_id = CORE_TEST_ID_BLOCK;
    psa_invec in_vec = {&testcase_id, sizeof(testcase_id)};

    TRY_SFN(tfm_spm_core_test_sfn_veneer, &in_vec, 1, NULL, 0);
#else
    psa_status_t err;

    err = psa_test_common(SPM_CORE_TEST_BLOCK_SID,
                          SPM_CORE_TEST_BLOCK_VERSION,
                          NULL, 0, NULL, 0);
    if (err != PSA_SUCCESS) {
        LOG_MSG("Secure call to sfn block failed, generic!");
    }
#endif
}
/**
 * \brief Test definition for the RTX - TFM integration tests
 *        scenarios
 */
enum test_type {
    TEST_TYPE_1 = 1, /*!< Sequential test: single task using the NS lock to
                          access TFM */
    TEST_TYPE_2,     /*!< Priority test: high priority tries to preempt TFM,
                          gets delayed */
    TEST_TYPE_3,     /*!< Priority inversion: classical scenario with high
                          priority task waiting on lower priority task
                          undefinitely if NS lock is configured without priority
                          inheritance */
    TEST_TYPE_4,     /*!< non-NS lock: like sequential, but doesn't use any NS
                          lock mechanism */
    TEST_TYPE_5,     /*!< non-NS lock, core locked: high priority tries to
                          overcome the NS lock but finds TFM core locked by
                          lower priority task and fails */
    TEST_TYPE_6      /*!< Like TEST_TYPE_2, but the high priority task has now a
                          timeout to acquire the NS lock. The timeout will
                          expire only if TFM Core is built with the
                          de-prioritization disabled */
};

static const osThreadAttr_t tattr_seq = {
    .name = "seq_task",
    .stack_size = 1024U,
    .attr_bits = osThreadJoinable,
    .tz_module = 1,
};
static const osThreadAttr_t tattr_mid = {
    .name = "mid_task",
    .stack_size = 512U,
    .attr_bits = osThreadJoinable,
    .tz_module = 0,
    .priority = osPriorityAboveNormal
};
static const osThreadAttr_t tattr_pri = {
    .name = "pri_task",
    .stack_size = 1024U,
    .attr_bits = osThreadJoinable,
    .tz_module = 1,
    .priority = osPriorityHigh
};

/**
 * \brief Mutex id, NS lock
 */
static osMutexId_t  mutex_id;

/**
 * \brief Mutex properties, NS lock
 */
static const osMutexAttr_t mattr_ns_lock = {
    .name = "ns_lock",
    //.attr_bits = osMutexPrioInherit
};

/**
 * \brief TFM NS lock options
 *
 * \details Options used while acquiring the NS lock
 */
struct tfm_ns_lock_options
{
    bool use_ns_lock;
    uint32_t timeout;
};

/**
 * \brief tfm_service_request
 *
 * \details This function is used to request a TFM service in thread mode.
 *          Optionally uses the NS lock and specifies a timeout for obtaining
 *          the NS lock.
 */
static void tfm_service_request(void(*fn)(void),
                                struct tfm_ns_lock_options *ns_lock_options_p)
{
    osStatus_t result;

    char buffer[80];

#define LOG_MSG_THREAD(MSG_THREAD) \
  do { \
      sprintf(buffer,"%s [%s]", MSG_THREAD, osThreadGetName(osThreadGetId())); \
      LOG_MSG(buffer); \
  } \
  while(0)

    LOG_MSG_THREAD("Trying to acquire the TFM core from NS");

    if (ns_lock_options_p->use_ns_lock) {
        result = osMutexAcquire(mutex_id,0);
        if (result == osOK) {
            LOG_MSG_THREAD("NS Lock: acquired");
            /* Add a delay here just to let the pri_task try to
             * acquire the NS lock before seq_task enters secure world
             */
            if (!strcmp(osThreadGetName(osThreadGetId()),"seq_task")) {
                osDelay(100U);
            }
            fn();
            LOG_MSG_THREAD("NS Lock: releasing...");
            osMutexRelease(mutex_id);
        } else {

            if (ns_lock_options_p->timeout == osWaitForever) {
                LOG_MSG_THREAD("Failed to acquire NS lock, keep waiting");
            } else {
                LOG_MSG_THREAD("Failed to acquire NS lock, wait with timeout");
            }

            result = osMutexAcquire(mutex_id,ns_lock_options_p->timeout);
            if (result == osOK) {
                LOG_MSG_THREAD("NS Lock: acquired");
                fn();
                LOG_MSG_THREAD("NS Lock: releasing...");
                osMutexRelease(mutex_id);
            } else if (result == osErrorTimeout) {
                LOG_MSG_THREAD("NS Lock: failed to acquire, timeout expired");
            } else {
                LOG_MSG_THREAD("NS Lock: unexpected failure trying to acquire");
            }
        }
    } else {
        /* Add a delay here to let the seq_task (which always uses the NS lock)
         * enter secure world before the pri_task (which can try to overcome the
         * NS lock in test scenario 5)
         */
        if (!strcmp(osThreadGetName(osThreadGetId()),"pri_task")) {
            osDelay(100U);
        }
        fn();
    }
}

/**
 * \brief Non-blocking test thread
 *
 */
__attribute__((noreturn))
static void mid_task(void *argument)
{
    osThreadId_t thread_id_pri;
    osThreadState_t thread_pri_state;
    uint32_t idx;

#ifdef TFM_NS_CLIENT_IDENTIFICATION
    tfm_nspm_register_client_id();
#endif /* TFM_NS_CLIENT_IDENTIFICATION */

    thread_id_pri = *((osThreadId_t *)argument);

    /* go to sleep */
    osDelay(100U);

    thread_pri_state = osThreadGetState(thread_id_pri);

    if (thread_pri_state == osThreadBlocked) {
        LOG_MSG("Running [mid_task] while [pri_task] is blocked");
    } else if (thread_pri_state == osThreadTerminated) {
        LOG_MSG("Running [mid_task] while [pri_task] is terminated");
    } else {
        LOG_MSG("Running [mid_task]");
    }

    /* Do non TFM related, non blocking, operations */
    for (idx=0; idx<0x3ffffff; idx++) {
    }

    LOG_MSG("Exiting [mid_task]");

    osThreadExit();
}

/**
 * \brief Priority test thread
 *
 */
__attribute__((noreturn))
static void pri_task(void *argument)
{
#ifdef TFM_NS_CLIENT_IDENTIFICATION
    tfm_nspm_register_client_id();
#endif /* TFM_NS_CLIENT_IDENTIFICATION */

    /* go to sleep */
    osDelay(100U);

    /* After wake up, try to get hold of the NS lock */
    tfm_service_request(secure_decrement_ns_lock_2,
                            (struct tfm_ns_lock_options *)argument);

    osThreadExit();
}

/**
 * \brief Sequential test thread
 *
 */
__attribute__((noreturn))
static void seq_task(void *argument)
{
    osThreadId_t thread_id, thread_id_mid;
    enum test_type test_type;

    /* By default, use NS lock and wait forever if busy, i.e. until unblocked */
    struct tfm_ns_lock_options ns_lock_opt =
                                  {.use_ns_lock=true, .timeout=osWaitForever};
    struct tfm_ns_lock_options ns_lock_opt_pri =
                                  {.use_ns_lock=true, .timeout=osWaitForever};

#ifdef TFM_NS_CLIENT_IDENTIFICATION
    tfm_nspm_register_client_id();
#endif /* TFM_NS_CLIENT_IDENTIFICATION */

    test_type = *((enum test_type *)argument);

    if (test_type == TEST_TYPE_1) {
        LOG_MSG("Scenario 1 - Sequential");
    } else if (test_type == TEST_TYPE_2) {
        LOG_MSG("Scenario 2 - Priority");
        thread_id = osThreadNew(pri_task, &ns_lock_opt_pri, &tattr_pri);
    } else if (test_type == TEST_TYPE_3) {
        LOG_MSG("Scenario 3 - Priority inversion");
        thread_id = osThreadNew(pri_task, &ns_lock_opt_pri, &tattr_pri);
        thread_id_mid = osThreadNew(mid_task, &thread_id, &tattr_mid);
    } else if (test_type == TEST_TYPE_4) {
        LOG_MSG("Scenario 4 - non-NS lock");
        ns_lock_opt.use_ns_lock = false;
    } else if (test_type == TEST_TYPE_5) {
        LOG_MSG("Scenario 5 - non-NS lock, core locked");
        ns_lock_opt_pri.use_ns_lock = false;
        thread_id = osThreadNew(pri_task, &ns_lock_opt_pri, &tattr_pri);
    } else if (test_type == TEST_TYPE_6) {
        LOG_MSG("Scenario 6 - Core prioritization effects on NS world");
        ns_lock_opt_pri.timeout = 0x10000; /* timed_wait for NS lock */
        thread_id = osThreadNew(pri_task, &ns_lock_opt_pri, &tattr_pri);
    } else {
        LOG_MSG("Scenario not supported");
        osThreadExit();
    }

    /* Try to acquire the NS lock */
    tfm_service_request(secure_decrement_ns_lock_1, &ns_lock_opt);

    if (test_type == TEST_TYPE_1) {
        LOG_MSG("Scenario 1 - test finished\n");
    } else if (test_type == TEST_TYPE_2) {
        osThreadJoin(thread_id);
        LOG_MSG("Scenario 2 - test finished\n");
    } else if (test_type == TEST_TYPE_3) {
        osThreadJoin(thread_id);
        osThreadJoin(thread_id_mid);
        LOG_MSG("Scenario 3 - test finished\n");
    } else if (test_type == TEST_TYPE_4) {
        LOG_MSG("Scenario 4 - test finished\n");
    } else if (test_type == TEST_TYPE_5) {
        osThreadJoin(thread_id);
        LOG_MSG("Scenario 5 - test finished\n");
    } else if (test_type == TEST_TYPE_6) {
        osThreadJoin(thread_id);
        LOG_MSG("Scenario 6 - test finished\n");
    }

    osThreadExit();
}

/**
 * \brief Execute the interactive tets cases
 *
 */
void execute_ns_interactive_tests(void)
{
    uint8_t idx;

    osThreadId_t thread_id;

    /* Test type list */
    enum test_type test_type[] = {TEST_TYPE_1, TEST_TYPE_2, TEST_TYPE_3,
                                  TEST_TYPE_4, TEST_TYPE_5, TEST_TYPE_6};

    /* Create the NS lock -- shared among testing scenarios */
    mutex_id = osMutexNew(&mattr_ns_lock);

    /* Loop in the test list */
    for (idx=0; idx<sizeof(test_type); idx++) {
        /* Spawn the main thread */
        thread_id = osThreadNew(seq_task, &test_type[idx], &tattr_seq);

        /* Wait for it to finish before moving to the next scenario */
        osThreadJoin(thread_id);
    }
}
#endif /* CORE_TEST_INTERACTIVE */

#if defined(TEST_FRAMEWORK_NS) || defined(TEST_FRAMEWORK_S)
/**
 * \brief Services test thread
 *
 */
__attribute__((noreturn))
void test_app(void *argument)
{
    UNUSED_VARIABLE(argument);

#ifdef TEST_FRAMEWORK_S
    /* FIXME: The non-secure audit log test currently relies on the fact that
     * the audit log secure test is run first. However the Non-secure tests
     * represent simpler and more common test cases which would make more sense
     * to be run first. Therefore if this dependency is removed the execution
     * order of these test classes should be reversed. */
    tfm_secure_client_run_tests();
#endif
#ifdef TEST_FRAMEWORK_NS
    tfm_non_secure_client_run_tests();
#endif
    /* End of test */
    for (;;) {
    }
}
#endif /* TEST_FRAMEWORK_NS OR TEST_FRAMEWORK_S */
