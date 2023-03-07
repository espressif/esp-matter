/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "ipc_ns_tests.h"
#include "psa/client.h"
#include "test/framework/test_framework_helpers.h"
#ifdef TFM_PSA_API
#include "psa_manifest/sid.h"
#endif

/* List of tests */
static void tfm_ipc_test_1001(struct test_result_t *ret);
static void tfm_ipc_test_1002(struct test_result_t *ret);
static void tfm_ipc_test_1003(struct test_result_t *ret);
static void tfm_ipc_test_1004(struct test_result_t *ret);
static void tfm_ipc_test_1005(struct test_result_t *ret);
static void tfm_ipc_test_1006(struct test_result_t *ret);

#ifdef TFM_IPC_ISOLATION_2_TEST_READ_ONLY_MEM
static void tfm_ipc_test_1007(struct test_result_t *ret);
#endif

#ifdef TFM_IPC_ISOLATION_2_APP_ACCESS_PSA_MEM
static void tfm_ipc_test_1008(struct test_result_t *ret);
#endif

#ifdef TFM_IPC_ISOLATION_2_MEM_CHECK
static void tfm_ipc_test_1009(struct test_result_t *ret);
#endif

static void tfm_ipc_test_1010(struct test_result_t *ret);

static struct test_t ipc_veneers_tests[] = {
    {&tfm_ipc_test_1001, "TFM_IPC_TEST_1001",
     "Get PSA framework version", {TEST_PASSED}},
    {&tfm_ipc_test_1002, "TFM_IPC_TEST_1002",
     "Get version of an RoT Service", {TEST_PASSED}},
    {&tfm_ipc_test_1003, "TFM_IPC_TEST_1003",
     "Connect to an RoT Service", {TEST_PASSED}},
    {&tfm_ipc_test_1004, "TFM_IPC_TEST_1004",
     "Call an RoT Service", {TEST_PASSED}},
    {&tfm_ipc_test_1005, "TFM_IPC_TEST_1005",
     "Call IPC_INIT_BASIC_TEST service", {TEST_PASSED}},
    {&tfm_ipc_test_1006, "TFM_IPC_TEST_1006",
     "Call PSA RoT access APP RoT memory test service", {TEST_PASSED}},
#ifdef TFM_IPC_ISOLATION_2_TEST_READ_ONLY_MEM
    {&tfm_ipc_test_1007, "TFM_IPC_TEST_1007",
     "Call PSA RoT access APP RoT readonly memory test service", {TEST_PASSED}},
#endif
#ifdef TFM_IPC_ISOLATION_2_APP_ACCESS_PSA_MEM
    {&tfm_ipc_test_1008, "TFM_IPC_TEST_1008",
     "Call APP RoT access PSA RoT memory test service", {TEST_PASSED}},
#endif
#ifdef TFM_IPC_ISOLATION_2_MEM_CHECK
    {&tfm_ipc_test_1009, "TFM_IPC_TEST_1009",
     "Call APP RoT memory check test service", {TEST_PASSED}},
#endif
    {&tfm_ipc_test_1010, "TFM_IPC_TEST_1010",
     "Test psa_call with the status of PSA_ERROR_PROGRAMMER_ERROR", {TEST_PASSED}},
};

void register_testsuite_ns_ipc_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(ipc_veneers_tests) / sizeof(ipc_veneers_tests[0]));

    set_testsuite("IPC non-secure interface test (TFM_IPC_TEST_1XXX)",
                  ipc_veneers_tests, list_size, p_test_suite);
}

/**
 * \brief Retrieve the version of the PSA Framework API.
 *
 * \note This is a functional test only and doesn't
 *       mean to test all possible combinations of
 *       input parameters and return values.
 */
static void tfm_ipc_test_1001(struct test_result_t *ret)
{
    uint32_t version;

    version = psa_framework_version();
    if (version == PSA_FRAMEWORK_VERSION) {
        TEST_LOG("The version of the PSA Framework API is %d.\r\n", version);
    } else {
        TEST_FAIL("The version of the PSA Framework API is not valid!\r\n");
        return;
    }
}

/**
 * \brief Retrieve the version of an RoT Service.
 */
static void tfm_ipc_test_1002(struct test_result_t *ret)
{
    uint32_t version;

    version = psa_version(IPC_SERVICE_TEST_BASIC_SID);
    if (version == PSA_VERSION_NONE) {
        TEST_FAIL("RoT Service is not implemented or caller is not authorized" \
                  "to access it!\r\n");
        return;
    } else {
        /* Valid version number */
        TEST_LOG("The service version is %d.\r\n", version);
    }
    ret->val = TEST_PASSED;
}

/**
 * \brief Connect to an RoT Service by its SID.
 */
static void tfm_ipc_test_1003(struct test_result_t *ret)
{
    psa_handle_t handle;

    handle = psa_connect(IPC_SERVICE_TEST_BASIC_SID,
                         IPC_SERVICE_TEST_BASIC_VERSION);
    if (handle > 0) {
        TEST_LOG("Connect success!\r\n");
    } else {
        TEST_FAIL("The RoT Service has refused the connection!\r\n");
        return;
    }
    psa_close(handle);
    ret->val = TEST_PASSED;
}

/**
 * \brief Call an RoT Service.
 */
static void tfm_ipc_test_1004(struct test_result_t *ret)
{
    char str1[] = "str1";
    char str2[] = "str2";
    char str3[128], str4[128];
    struct psa_invec invecs[2] = {{str1, sizeof(str1)/sizeof(char)},
                                  {str2, sizeof(str2)/sizeof(char)}};
    struct psa_outvec outvecs[2] = {{str3, sizeof(str3)/sizeof(char)},
                                    {str4, sizeof(str4)/sizeof(char)}};
    psa_handle_t handle;
    psa_status_t status;
    uint32_t version;

    version = psa_version(IPC_SERVICE_TEST_BASIC_SID);
    TEST_LOG("TFM service support version is %d.\r\n", version);
    handle = psa_connect(IPC_SERVICE_TEST_BASIC_SID,
                         IPC_SERVICE_TEST_BASIC_VERSION);
    status = psa_call(handle, PSA_IPC_CALL, invecs, 2, outvecs, 2);
    if (status >= 0) {
        TEST_LOG("psa_call is successful!\r\n");
    } else {
        TEST_FAIL("psa_call is failed!\r\n");
        return;
    }

    TEST_LOG("outvec1 is: %s\r\n", outvecs[0].base);
    TEST_LOG("outvec2 is: %s\r\n", outvecs[1].base);
    psa_close(handle);
    ret->val = TEST_PASSED;
}

/**
 * \brief Call IPC_CLIENT_TEST_BASIC_SID RoT Service to run the IPC basic test.
 */
static void tfm_ipc_test_1005(struct test_result_t *ret)
{
    psa_handle_t handle;
    psa_status_t status;
    int test_result;
    struct psa_outvec outvecs[1] = {{&test_result, sizeof(test_result)}};

    handle = psa_connect(IPC_CLIENT_TEST_BASIC_SID,
                         IPC_CLIENT_TEST_BASIC_VERSION);
    if (handle > 0) {
        TEST_LOG("Connect success!");
    } else {
        TEST_LOG("The RoT Service has refused the connection!");
        ret->val = TEST_FAILED;
        return;
    }

    status = psa_call(handle, PSA_IPC_CALL, NULL, 0, outvecs, 1);
    if (status >= 0) {
        TEST_LOG("Call success!");
        if (test_result > 0) {
            ret->val = TEST_PASSED;
        } else {
            ret->val = TEST_FAILED;
        }
    } else {
        TEST_LOG("Call failed!");
        ret->val = TEST_FAILED;
    }

    psa_close(handle);
}

/**
 * \brief Call IPC_CLIENT_TEST_PSA_ACCESS_APP_MEM_SID RoT Service
 *  to run the IPC PSA access APP mem test.
 */
static void tfm_ipc_test_1006(struct test_result_t *ret)
{
    psa_handle_t handle;
    psa_status_t status;
    int test_result;
    struct psa_outvec outvecs[1] = {{&test_result, sizeof(test_result)}};

    handle = psa_connect(IPC_CLIENT_TEST_PSA_ACCESS_APP_MEM_SID,
                         IPC_CLIENT_TEST_PSA_ACCESS_APP_MEM_VERSION);
    if (handle > 0) {
        TEST_LOG("Connect success!");
    } else {
        TEST_LOG("The RoT Service has refused the connection!");
        ret->val = TEST_FAILED;
        return;
    }

    status = psa_call(handle, PSA_IPC_CALL, NULL, 0, outvecs, 1);
    if (status >= 0) {
        TEST_LOG("Call success!");
        if (test_result > 0) {
            ret->val = TEST_PASSED;
        } else {
            ret->val = TEST_FAILED;
        }
    } else {
        TEST_LOG("Call failed!");
        ret->val = TEST_FAILED;
    }

    psa_close(handle);
}

#ifdef TFM_IPC_ISOLATION_2_TEST_READ_ONLY_MEM
/**
 * \brief Call IPC_CLIENT_TEST_PSA_ACCESS_APP_READ_ONLY_MEM_SID RoT Service
 *  to run the IPC PSA access APP readonly mem test.
 */
static void tfm_ipc_test_1007(struct test_result_t *ret)
{
    psa_handle_t handle;
    int test_result;
    struct psa_outvec outvecs[1] = {{&test_result, sizeof(test_result)}};

    handle = psa_connect(IPC_CLIENT_TEST_PSA_ACCESS_APP_READ_ONLY_MEM_SID,
                         IPC_CLIENT_TEST_PSA_ACCESS_APP_READ_ONLY_MEM_VERSION);
    if (handle > 0) {
        TEST_LOG("Connect success!");
    } else {
        TEST_LOG("The RoT Service has refused the connection!");
        ret->val = TEST_FAILED;
        return;
    }

    psa_call(handle, PSA_IPC_CALL, NULL, 0, outvecs, 1);

    /* The system should panic in psa_call. If runs here, the test fails. */
    ret->val = TEST_FAILED;
    psa_close(handle);
}
#endif

#ifdef TFM_IPC_ISOLATION_2_APP_ACCESS_PSA_MEM
/**
 * \brief Call IPC_CLIENT_TEST_APP_ACCESS_PSA_MEM_SID RoT Service
 *  to run the IPC APP access PSA mem test.
 */
static void tfm_ipc_test_1008(struct test_result_t *ret)
{
    psa_handle_t handle;
    int test_result;
    struct psa_outvec outvecs[1] = {{&test_result, sizeof(test_result)}};

    handle = psa_connect(IPC_CLIENT_TEST_APP_ACCESS_PSA_MEM_SID,
                         IPC_CLIENT_TEST_APP_ACCESS_PSA_MEM_VERSION);
    if (handle > 0) {
        TEST_LOG("Connect success!");
    } else {
        TEST_LOG("The RoT Service has refused the connection!");
        ret->val = TEST_FAILED;
        return;
    }

    psa_call(handle, PSA_IPC_CALL, NULL, 0, outvecs, 1);

    /* The system should panic in psa_call. If runs here, the test fails. */
    ret->val = TEST_FAILED;
    psa_close(handle);
}
#endif

#ifdef TFM_IPC_ISOLATION_2_MEM_CHECK
/**
 * \brief Call IPC_CLIENT_TEST_MEM_CHECK_SID RoT Service
 *  to run the IPC mem check test.
 */
static void tfm_ipc_test_1009(struct test_result_t *ret)
{
    psa_handle_t handle;
    int test_result;
    struct psa_outvec outvecs[1] = {{&test_result, sizeof(test_result)}};

    handle = psa_connect(IPC_CLIENT_TEST_MEM_CHECK_SID,
                         IPC_CLIENT_TEST_MEM_CHECK_VERSION);
    if (handle > 0) {
        TEST_LOG("Connect success!");
    } else {
        TEST_LOG("The RoT Service has refused the connection!");
        ret->val = TEST_FAILED;
        return;
    }

    psa_call(handle, PSA_IPC_CALL, NULL, 0, outvecs, 1);

    /* The system should panic in psa_call. If runs here, the test fails. */
    ret->val = TEST_FAILED;
    psa_close(handle);
}
#endif

/**
 * \brief Call IPC_SERVICE_TEST_CLIENT_PREGRAMMER_ERROR RoT Service to
 *  test psa_call with the status of PSA_ERROR_PROGRAMMER_ERROR.
 */
static void tfm_ipc_test_1010(struct test_result_t *ret)
{
    psa_handle_t handle;
    psa_status_t status;
    handle = psa_connect(IPC_SERVICE_TEST_CLIENT_PROGRAMMER_ERROR_SID,
                         IPC_SERVICE_TEST_CLIENT_PROGRAMMER_ERROR_VERSION);
    if (handle > 0) {
        TEST_LOG("Connect success!\r\n");
    } else {
        TEST_LOG("The RoT Service has refused the connection!\r\n");
        ret->val = TEST_FAILED;
        return;
    }
    status = psa_call(handle, PSA_IPC_CALL, NULL, 0, NULL, 0);
    if (status == PSA_ERROR_PROGRAMMER_ERROR) {
        TEST_LOG("The first time call success!\r\n");
    } else {
        TEST_LOG("The first time call failed!\r\n");
        ret->val = TEST_FAILED;
    }
    status = psa_call(handle, PSA_IPC_CALL, NULL, 0, NULL, 0);
    if (status == PSA_ERROR_PROGRAMMER_ERROR) {
        TEST_LOG("The second time call success!\r\n");
    } else {
        TEST_LOG("The second time call failed!\r\n");
        ret->val = TEST_FAILED;
    }

    psa_close(handle);
}
