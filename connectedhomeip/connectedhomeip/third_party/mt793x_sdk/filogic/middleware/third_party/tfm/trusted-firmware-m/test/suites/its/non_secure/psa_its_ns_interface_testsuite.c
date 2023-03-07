/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "its_ns_tests.h"
#include "test/framework/test_framework_helpers.h"
#include "../its_tests_common.h"

static struct test_t psa_its_ns_tests[] = {
    {&tfm_its_test_common_001, "TFM_ITS_TEST_1001",
     "Set interface"},
    {&tfm_its_test_common_002, "TFM_ITS_TEST_1002",
     "Set interface with create flags"},
    {&tfm_its_test_common_003, "TFM_ITS_TEST_1003",
     "Set interface with NULL data pointer"},
    {&tfm_its_test_common_004, "TFM_ITS_TEST_1004",
     "Set interface with write once UID"},
    {&tfm_its_test_common_005, "TFM_ITS_TEST_1005",
     "Get interface with valid data"},
    {&tfm_its_test_common_006, "TFM_ITS_TEST_1006",
     "Get interface with zero data length"},
    {&tfm_its_test_common_007, "TFM_ITS_TEST_1007",
     "Get interface with invalid UIDs"},
    {&tfm_its_test_common_008, "TFM_ITS_TEST_1008",
     "Get interface with invalid data lengths and offsets"},
    {&tfm_its_test_common_009, "TFM_ITS_TEST_1009",
     "Get interface with NULL data pointer"},
    {&tfm_its_test_common_010, "TFM_ITS_TEST_1010",
     "Get info interface with write once UID"},
    {&tfm_its_test_common_011, "TFM_ITS_TEST_1011",
     "Get info interface with valid UID"},
    {&tfm_its_test_common_012, "TFM_ITS_TEST_1012",
     "Get info interface with invalid UIDs"},
    {&tfm_its_test_common_013, "TFM_ITS_TEST_1013",
     "Remove interface with valid UID"},
    {&tfm_its_test_common_014, "TFM_ITS_TEST_1014",
     "Remove interface with write once UID"},
    {&tfm_its_test_common_015, "TFM_ITS_TEST_1015",
     "Remove interface with invalid UID"},
    {&tfm_its_test_common_016, "TFM_ITS_TEST_1016",
     "Block compaction after remove"},
    {&tfm_its_test_common_017, "TFM_ITS_TEST_1017",
     "Multiple partial gets"},
    {&tfm_its_test_common_018, "TFM_ITS_TEST_1018",
     "Multiple sets to same UID from same thread"},
    {&tfm_its_test_common_019, "TFM_ITS_TEST_1019",
     "Set, get and remove interface with different asset sizes"},
};

void register_testsuite_ns_psa_its_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(psa_its_ns_tests) / sizeof(psa_its_ns_tests[0]));

    set_testsuite("PSA internal trusted storage NS interface tests "
                 "(TFM_ITS_TEST_1XXX)",
                  psa_its_ns_tests, list_size, p_test_suite);

#ifdef ITS_SHOW_FLASH_WARNING
    TEST_LOG("\r\n**WARNING** The ITS regression tests reduce the life of the "
             "flash memory as they write/erase multiple times the memory. \r\n"
             "Please, set the ITS_RAM_FS flag to use RAM instead of flash."
             "\r\n\r\n");
#endif
}
