/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "qcbor_ns_tests.h"
#include <stdio.h>
#include "lib/ext/qcbor/test/run_tests.h"

/*
 * The QCBOR test cases are defined in: lib/ext/qcbor/test/run_test.c
 *
 * There are two types of configuration to execute test cases:
 *  - All tests: Just pass an array containing a NULL pointer to RunTest():
 *               qcbor_test_cases_all[]. This is the default configuration.
 *
 *  - Selected tests: qcbor_test_cases[] array contains the name of all tests.
 *                    To execute only selected test cases, just remove/comment
 *                    out the unwanted tests from the array and pass it to
 *                    RunTests(). This configuration disables all tests with a
 *                    long execution time to speed up development.
 */

/* Define test suite for QCBOR library */
static void tfm_qcbor_test_7001(struct test_result_t *ret);

static struct test_t qcbor_regression_test[] = {
    {&tfm_qcbor_test_7001, "TFM_QCBOR_TEST_7001",
     "Regression test of QCBOR library", {TEST_PASSED} },
};

/* To execute only selected test cases, then remove unwanted ones from the array
 * and pass it to RunTests().
 * Not static to avoid compiler warning due to non-usage
 */
const char *qcbor_test_cases[] = {
    "ParseMapAsArrayTest",
    "AllocAllStringsTest",
    "IndefiniteLengthNestTest",
    "NestedMapTestIndefLen",
    "ParseSimpleTest",
    "EncodeRawTest",
    "RTICResultsTest",
    "MapEncodeTest",
    "ArrayNestingTest1",
    "ArrayNestingTest2",
    "ArrayNestingTest3",
    "EncodeDateTest",
    "SimpleValuesTest1",
    "IntegerValuesTest1",
    "AllAddMethodsTest",
    "ParseTooDeepArrayTest",
    "ComprehensiveInputTest",
    "ParseMapTest",
    "IndefiniteLengthArrayMapTest",
    "BasicEncodeTest",
    "NestedMapTest",
    "BignumParseTest",
    "OptTagParseTest",
    "DateParseTest",
    "ShortBufferParseTest2",
    "ShortBufferParseTest",
    "ParseDeepArrayTest",
    "SimpleArrayTest",
    "IntegerValuesParseTest",
    "MemPoolTest",
    "IndefiniteLengthStringTest", /**/
    "HalfPrecisionDecodeBasicTests",
    "DoubleAsSmallestTest",
    "HalfPrecisionAgainstRFCCodeTest",
    "BstrWrapTest",
    "BstrWrapErrorTest",
    "BstrWrapNestTest", /**/
    "CoseSign1TBSTest",
    "StringDecoderModeFailTest",
    /* "BigComprehensiveInputTest", */ /* Takes too long to execute */
    "EncodeErrorTests",
    "UBUTest_CopyUtil",
    "UOBTest_NonAdversarial",
    "TestBasicSanity",
    "UOBTest_BoundaryConditionsTest",
    "UBMacroConversionsTest",
    "UBUtilTests",
    "UIBTest_IntegerFormat",
};

/* To execute all test cases, then pass this array to RunTests()
 * Test cases are defined in: lib/ext/qcbor/test/run_tests.c
 */
const static char *qcbor_test_cases_all[] = {
    0, /* Indicates to run all enabled test case */
};

/**
 * \brief Print QCBOR test results per test case
 *
 * \param[in]  szString  String to print out
 * \param[in]  ctx       Where to print
 */

static void fputs_wrapper(const char *string, void *out_ctx, int new_line)
{
    (void)out_ctx;
    (void)new_line;
    /*
     * To get test result per test case, change
     * the preprocessor '#if 0' to '#if 1'.
     */
# if 0
    TEST_LOG("%s\r\n", string);
#else
    (void)string;
#endif
}

static void tfm_qcbor_test_7001(struct test_result_t *ret)
{
    int32_t test_failed_cnt = 0;

    test_failed_cnt = RunTestsQCBOR(qcbor_test_cases_all, fputs_wrapper,
                                    NULL, NULL);
    if (test_failed_cnt != 0) {
        TEST_FAIL("QCBOR test failed");
        return;
    }

    ret->val = TEST_PASSED;
}

void
register_testsuite_ns_qcbor(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(qcbor_regression_test) /
                 sizeof(qcbor_regression_test[0]));

    set_testsuite("QCBOR regression test"
                  "(TFM_QCBOR_TEST_7XXX)",
                  qcbor_regression_test, list_size, p_test_suite);
}
