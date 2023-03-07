/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __ITS_TESTS_COMMON_H__
#define __ITS_TESTS_COMMON_H__

#include "test/framework/test_framework_helpers.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Test UIDs */
#define WRITE_ONCE_UID  1U /* Cannot be modified or deleted once created */
#define TEST_UID_1      2U
#define TEST_UID_2      3U
#define TEST_UID_3      4U

/* Invalid values */
#define INVALID_UID              0U
#define INVALID_DATA_LEN         UINT32_MAX
#define INVALID_OFFSET           UINT32_MAX
#define INVALID_FLAG             (1U << 31)

/* Write once data */
#define WRITE_ONCE_DATA          "THE_FIVE_BOXING_WIZARDS_JUMP_QUICKLY"
#define WRITE_ONCE_DATA_SIZE     (sizeof(WRITE_ONCE_DATA) - 1)
#define WRITE_ONCE_READ_DATA     "############################################"
#define WRITE_ONCE_RESULT_DATA   ("####" WRITE_ONCE_DATA "####")

#define WRITE_DATA               "THEQUICKBROWNFOXJUMPSOVERALAZYDOG"
#define WRITE_DATA_SIZE          (sizeof(WRITE_DATA) - 1)
#define READ_DATA                "_________________________________________"
#define RESULT_DATA              ("____" WRITE_DATA "____")
#define OFFSET_READ_DATA         "HEQUICKBROWNFOXJUMPSOVERALAZYDOG"
#define OFFSET_RESULT_DATA       ("____" OFFSET_READ_DATA "_____")

/**
 * Several tests use a buffer to read back data from an asset. This buffer is
 * larger than the size of the asset data by PADDING_SIZE bytes. This allows
 * us to ensure that only the expected data is read back and that it is read
 * back correctly.
 *
 * For example if the buffer and asset are as follows:
 * Buffer - "XXXXXXXXXXXX", Asset data - "AAAA"
 *
 * Then a correct and successful read would give this result: "XXXXAAAAXXXX"
 * (Assuming a PADDING_SIZE of 8)
 */
#define BUFFER_SIZE                   24
#define PADDING_SIZE                  8
#define HALF_PADDING_SIZE             4
#define BUFFER_PLUS_PADDING_SIZE      (BUFFER_SIZE + PADDING_SIZE)
#define BUFFER_PLUS_HALF_PADDING_SIZE (BUFFER_SIZE + HALF_PADDING_SIZE)

/**
 * \brief Tests set function with:
 *        - Valid UID, no data, no flags
 *        - Invalid UID, no data, no flags
 *
 * \param[out] ret  Test result
 */
void tfm_its_test_common_001(struct test_result_t *ret);

/**
 * \brief Tests set function with:
 *        - Zero create flags
 *        - Valid create flags (with previously created UID)
 *        - Invalid create flags
 *
 * \param[out] ret  Test result
 */
void tfm_its_test_common_002(struct test_result_t *ret);

/**
 * \brief Tests set function with:
 *        - NULL data pointer and zero data length
 *
 * \param[out] ret  Test result
 *
 * \note A request with a null data pointer and data length not equal to zero is
 *       treated as a secure violation. TF-M framework will reject such requests
 *       and not return to the caller so this case is not tested here.
 *
 */
void tfm_its_test_common_003(struct test_result_t *ret);

/**
 * \brief Tests set function with:
 *        - Write once UID that has already been created
 *
 * \param[out] ret  Test result
 */
void tfm_its_test_common_004(struct test_result_t *ret);

/**
 * \brief Tests get function with:
 *        - Valid data, zero offset
 *        - Valid data, non-zero offset
 *
 * \param[out] ret  Test result
 */
void tfm_its_test_common_005(struct test_result_t *ret);

/**
 * \brief Tests get function with:
 *        - Zero data length, zero offset
 *        - Zero data length, non-zero offset
 *
 * \param[out] ret  Test result
 */
void tfm_its_test_common_006(struct test_result_t *ret);

/**
 * \brief Tests get function with:
 *        - Unset UID
 *        - Invalid UID
 *
 * \param[out] ret  Test result
 */
void tfm_its_test_common_007(struct test_result_t *ret);

/**
 * \brief Tests get function with:
 *        - Offset greater than UID length
 *        - Data length greater than UID length
 *        - Data length + offset greater than UID length
 *
 * \param[out] ret  Test result
 */
void tfm_its_test_common_008(struct test_result_t *ret);

/**
 * \brief Tests get function with:
 *        - NULL data pointer and zero data length
 *
 * \param[out] ret  Test result
 *
 * \note A request with a null data pointer and data length not equal to zero is
 *       treated as a secure violation. TF-M framework will reject such requests
 *       and not return to the caller so this case is not tested here.
 *
 */
void tfm_its_test_common_009(struct test_result_t *ret);

/**
 * \brief Tests get info function with:
 *        - Write once UID
 *
 * \param[out] ret  Test result
 */
void tfm_its_test_common_010(struct test_result_t *ret);

/**
 * \brief Tests get info function with:
 *        - Valid UID
 *
 * \param[out] ret  Test result
 */
void tfm_its_test_common_011(struct test_result_t *ret);

/**
 * \brief Tests get info function with:
 *        - Unset UID
 *        - Invalid UID
 *
 * \param[out] ret  Test result
 */
void tfm_its_test_common_012(struct test_result_t *ret);

/**
 * \brief Tests remove function with:
 *        - Valid UID
 *
 * \param[out] ret  Test result
 */
void tfm_its_test_common_013(struct test_result_t *ret);

/**
 * \brief Tests remove function with:
 *        - Write once UID
 *
 * \param[out] ret  Test result
 */
void tfm_its_test_common_014(struct test_result_t *ret);

/**
 * \brief Tests remove function with:
 *        - Invalid UID
 *
 * \param[out] ret  Test result
 */
void tfm_its_test_common_015(struct test_result_t *ret);

/**
 * \brief Tests data block compact feature.
 *        Set UID 1 to locate it at the beginning of the block. Then set UID 2
 *        to be located after UID 1 and remove UID 1. UID 2 will be compacted to
 *        the beginning of the block. This test verifies that the compaction
 *        works correctly by reading back UID 2.
 *
 * \param[out] ret  Test result
 */
void tfm_its_test_common_016(struct test_result_t *ret);

/**
 * \brief Tests set and multiple partial gets.
 *
 * \param[out] ret  Test result
 */
void tfm_its_test_common_017(struct test_result_t *ret);

/**
 * \brief Tests multiple sets to the same UID.
 *
 * \param[out] ret  Test result
 */
void tfm_its_test_common_018(struct test_result_t *ret);

/**
 * \brief Tests set, get_info, get and remove function with:
 *        - Valid UID's, No Flags
 *        - Data length of different asset sizes
 *
 * \param[out] ret  Test result
 */
void tfm_its_test_common_019(struct test_result_t *ret);

#ifdef __cplusplus
}
#endif

#endif /* __ITS_TESTS_COMMON_H__ */
