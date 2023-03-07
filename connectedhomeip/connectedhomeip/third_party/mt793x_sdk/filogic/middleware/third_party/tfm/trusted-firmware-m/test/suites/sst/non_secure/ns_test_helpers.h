/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __NS_TEST_HELPERS_H__
#define __NS_TEST_HELPERS_H__

#include "test/framework/test_framework.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONCAT_(x, y) x ## y
#define CONCAT(x, y) CONCAT_(x, y)

/**
 * Several tests use a buffer to read back data from an asset. This buffer is
 * larger than the size of the asset data by PADDING_SIZE bytes. This allows
 * us to ensure that the only the expected data is read back and that it is read
 * back correctly.
 *
 * For example if the buffer and asset are as follows:
 * Buffer - "XXXXXXXXXXXX", Asset data - "AAAA"
 *
 * Then a correct and successful read would give this result: "XXXXAAAAXXXX"
 * (Assuming a PADDING_SIZE of 8)
 */
#define BUFFER_SIZE 24
#define PADDING_SIZE 8
#define HALF_PADDING_SIZE 4

#define BUFFER_PLUS_PADDING_SIZE (BUFFER_SIZE + PADDING_SIZE)
#define BUFFER_PLUS_HALF_PADDING_SIZE (BUFFER_SIZE + HALF_PADDING_SIZE)

/**
 * \brief Expands to the prototype of a test function.
 *
 * \param[in] test_name  Name of the test function
 */
#define TFM_SST_TEST_PROTO(test_name) \
    static void test_name(struct test_result_t *ret)

/**
 * \brief Expands to the standard name of a test function.
 *
 * \param[in] test_num  Identification number of the test
 */
#define TFM_SST_TEST_NAME(test_num) CONCAT(tfm_sst_test_, test_num)

/**
 * \brief Expands to the standard name of a task function.
 *
 * \param[in] test_num  Identification number of the task
 */
#define TFM_SST_TASK_NAME(test_num) CONCAT(TFM_SST_TEST_NAME(test_num), _task)

/**
 * \brief Expands to a test function declaration.
 *
 * \param[in] test_num  Identification number of the test
 */
#define TFM_SST_TEST(test_num) TFM_SST_TEST_PROTO(TFM_SST_TEST_NAME(test_num))

/**
 * \brief Expands to a task function declaration.
 *
 * \param[in] test_num  Identification number of the task
 */
#define TFM_SST_TASK(test_num) TFM_SST_TEST_PROTO(TFM_SST_TASK_NAME(test_num))

/**
 * \brief Defines a single-threaded SST NS test function and declares the
 *        corresponding task function.
 *
 * \param[in] test_num     Identification number of the test
 * \param[in] thread_name  Name of the thread in which to run the test
 */
#define TFM_SST_NS_TEST(test_num, thread_name)                           \
    TFM_SST_TASK(test_num);                                              \
    TFM_SST_TEST(test_num)                                               \
    {                                                                    \
        tfm_sst_run_test(thread_name, ret, TFM_SST_TASK_NAME(test_num)); \
    }                                                                    \
    TFM_SST_TASK(test_num)

/* The type of a test function */
typedef void test_func_t(struct test_result_t *ret);

/**
 * \brief Executes the given test function from the specified thread context.
 *
 * \param[in]  thread_name  Name of the thread to be created for test
 * \param[out] ret          Result of the test
 * \param[in]  test_func    Test function to be run in the new thread
 */
void tfm_sst_run_test(const char *thread_name, struct test_result_t *ret,
                      test_func_t *test_func);

#ifdef __cplusplus
}
#endif

#endif /* __NS_TEST_HELPERS_H__ */
