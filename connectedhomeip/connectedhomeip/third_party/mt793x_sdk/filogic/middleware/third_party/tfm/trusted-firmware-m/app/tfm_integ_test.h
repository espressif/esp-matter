/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "cmsis_compiler.h"

#ifndef __TFM_INTEG_TEST_H__
#define __TFM_INTEG_TEST_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Avoids the semihosting issue */
#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__asm("  .global __ARM_use_no_argv\n");
#endif

/**
 * \brief Simple macro to mark UNUSED variables
 *
 */
#define UNUSED_VARIABLE(X) ((void)(X))

#ifdef TEST_FRAMEWORK_NS
/**
 * \brief Main test application for the RTX-TFM core
 *        integration tests
 *
 */
void test_app(void *argument);
#endif /* TEST_FRAMEWORK_NS */

/**
 * \brief Execute the interactive test cases (button push)
 *
 */
void execute_ns_interactive_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_INTEG_TEST_H__ */
