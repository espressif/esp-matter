/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CORE_TEST_DEFS_H__
#define __CORE_TEST_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <limits.h>
#include "tfm_api.h"
#include "tfm_core.h"

/* These definitions are used in symbols, only digits are permitted */
#define CORE_TEST_ID_NS_THREAD            1001
#define CORE_TEST_ID_CHECK_INIT           1003
#define CORE_TEST_ID_RECURSION            1004
#define CORE_TEST_ID_BUFFER_CHECK         1007
#define CORE_TEST_ID_SS_TO_SS             1008
#define CORE_TEST_ID_SS_TO_SS_BUFFER      1010
#define CORE_TEST_ID_PERIPHERAL_ACCESS    1012
#define CORE_TEST_ID_GET_CALLER_CLIENT_ID 1013
#define CORE_TEST_ID_SPM_REQUEST          1014
#define CORE_TEST_ID_IOVEC_SANITIZATION   1015
#define CORE_TEST_ID_OUTVEC_WRITE         1016
#define CORE_TEST_ID_SECURE_IRQ           1017
#define CORE_TEST_ID_BLOCK                2001

enum irq_test_scenario_t {
    IRQ_TEST_SCENARIO_NONE,
    IRQ_TEST_SCENARIO_1,
    IRQ_TEST_SCENARIO_2,
    IRQ_TEST_SCENARIO_3,
    IRQ_TEST_SCENARIO_4,
    IRQ_TEST_SCENARIO_5,
};

struct irq_test_execution_data_t {
    volatile int32_t timer0_triggered;
    volatile int32_t timer1_triggered;
};

/* Use lower 16 bits in return value for error code, upper 16 for line number
 * in test service
 */
#define CORE_TEST_RETURN_ERROR(x) return (((__LINE__) << 16) | x)
#define CORE_TEST_ERROR_GET_EXTRA(x) (x >> 16)
#define CORE_TEST_ERROR_GET_CODE(x) (x & 0xFFFF)

enum core_test_errno_t {
    CORE_TEST_ERRNO_TEST_NOT_SUPPORTED         = -13,
    CORE_TEST_ERRNO_SP_NOT_INITED              = -12,
    CORE_TEST_ERRNO_UNEXPECTED_CORE_BEHAVIOUR  = -11,
    CORE_TEST_ERRNO_SP_RECURSION_NOT_REJECTED  = -10,
    CORE_TEST_ERRNO_INVALID_BUFFER             = -9,
    CORE_TEST_ERRNO_SLAVE_SP_CALL_FAILURE      = -8,
    CORE_TEST_ERRNO_SLAVE_SP_BUFFER_FAILURE    = -7,
    CORE_TEST_ERRNO_FIRST_CALL_FAILED          = -6,
    CORE_TEST_ERRNO_SECOND_CALL_FAILED         = -5,
    CORE_TEST_ERRNO_PERIPHERAL_ACCESS_FAILED   = -4,
    CORE_TEST_ERRNO_TEST_FAULT                 = -3,
    CORE_TEST_ERRNO_INVALID_TEST_ID            = -2,
    CORE_TEST_ERRNO_INVALID_PARAMETER          = -1,

    CORE_TEST_ERRNO_SUCCESS                    =  0,

    CORE_TEST_ERRNO_SUCCESS_2                  =  1,

    /* Following entry is only to ensure the error code of int size */
    CORE_TEST_ERRNO_FORCE_INT_SIZE = INT_MAX
};

#ifdef __cplusplus
}
#endif

#endif /* __CORE_TEST_DEFS_H__ */
