/*
 * attest_token_test_values.h
 *
 * Copyright (c) 2019, Laurence Lundblade.
 * Copyright (c) 2019, Arm Limited.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */

#ifndef __ATTEST_TOKEN_TEST_VALUES_H__
#define __ATTEST_TOKEN_TEST_VALUES_H__

/**
 * \file attest_token_test_values.h
 *
 * \brief Expected values for test suite.
 *
 * This is a bunch of made up values for hard-coded test cases for
 * attestation tokens.
 *
 * There are four possible test configurations for testing each claim:
 *
 *  1. No checking at all. \c TOKEN_TEST_REQUIRE_XXX is false and
 *  TOKEN_TEST_VALUE_XXX is not given.
 *
 *  2. Check for presence only. \c TOKEN_TEST_REQUIRE_XXX is true and
 *  TOKEN_TEST_VALUE_XXX is not given.
 *
 *  3. Check value if it is present, but it is not required to be
 *  present. \c TOKEN_TEST_REQUIRE_XXX is false and \c
 *  TOKEN_TEST_VALUE_XXX is given.
 *
 *  4. Must be present and of specific value. \c
 *  TOKEN_TEST_REQUIRE_XXX is true and \c TOKEN_TEST_VALUE_XXX is
 *  given.
 *
 * TOKEN_TEST_VALUE_XXX is not given as follows:
 *  - #define text strings as \c NULL
 *  - #define binary strings as \c NULL_Q_USEFUL_BUF_C
 *  - #define the integer value as \c INT32_MAX
 *
 * It is assumed that the expected value for any test will never be
 * any of these.
 *
 * Individual test can also be made to return values that are not
 * fixed at compile time by defining them to be a function and
 * implementing the funciton.  Here are examples for the three types:
 *
 *      struct q_useful_buf_c get_expected_nonce(void);
 *      #define TOKEN_TEST_VALUE_NONCE get_expected_nonce()
 *
 *      const char *get_expected_hw_version(void);
 *      #define TOKEN_TEST_VALUE_HW_VERSION get_expected_hw_version()
 *
 *      uint32_t get_expected_client_id(void);
 *      #define TOKEN_TEST_VALUE_CLIENT_ID get_expected_client_id()
 *
 * The initialization value for byte strings uses a compound literal
 * to create the \c ptr and \c len for a \c struct \c q_useful_buf_c.
 * They are a bit ugly, but they work and setting up this way allows
 * the literal value to be replaced by a function call for dynamic
 * expected values.
 *
 * The first part of the compound literal is the value of the
 * bytes. The second is an integer that is the length, the number of
 * bytes. They length must be the number of bytes in the first.
 */

/* The 64 byte special option-packed nonce where option flags
 * are packed in at the start. Binary. */
#define TOKEN_TEST_NONCE_BYTES \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#define TOKEN_TEST_VALUE_NONCE \
    (struct q_useful_buf_c) {\
      (uint8_t[]){TOKEN_TEST_NONCE_BYTES},\
        64\
    }
#define TOKEN_TEST_REQUIRE_NONCE true /* Mandatory claim */

/* A 32 byte mostly random value. Binary. Value not checked */
#define TOKEN_TEST_VALUE_UEID NULL_Q_USEFUL_BUF_C

/* A 32 byte mostly random value. Binary.
 *    platform/ext/common/template/tfm_initial_attestation_key_material.c
 */
/*
#define TOKEN_TEST_VALUE_UEID \
    (struct q_useful_buf_c) {\
        (uint8_t[]){ \
            0x01, \
            0xfa, 0x58, 0x75, 0x5f, 0x65, 0x86, 0x27, 0xce, \
            0x54, 0x60, 0xf2, 0x9b, 0x75, 0x29, 0x67, 0x13, \
            0x24, 0x8c, 0xae, 0x7a, 0xd9, 0xe2, 0x98, 0x4b, \
            0x90, 0x28, 0x0e, 0xfc, 0xbc, 0xb5, 0x02, 0x48  \
        },\
        33\
    }
*/
#define TOKEN_TEST_REQUIRE_UEID true /* Mandatory claim */

/* A 32 byte mostly random value. Binary.
 *    platform/ext/common/template/attest_hal.c
 */
#define TOKEN_TEST_VALUE_BOOT_SEED \
    (struct q_useful_buf_c) {\
        (uint8_t[]){ \
            0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, \
            0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, \
            0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, \
            0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF  \
        },\
        32\
    }
#define TOKEN_TEST_REQUIRE_BOOT_SEED true /* Mandatory claim */

/* A text string in EAN 13 format
 *    platform/ext/common/template/attest_hal.c
 */
#define TOKEN_TEST_VALUE_HW_VERSION "060456527282910010" /* Hard-coded value */
#define TOKEN_TEST_REQUIRE_HW_VERSION false /* Optional claim */

/* A 32 byte mostly random value. Binary.
 *    platform/ext/common/template/attest_hal.c
 */
#define TOKEN_TEST_VALUE_IMPLEMENTATION_ID \
    (struct q_useful_buf_c) {\
        (uint8_t[]){ \
            0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, \
            0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, \
            0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, \
            0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD  \
        },\
        32\
    }
#define TOKEN_TEST_REQUIRE_IMPLEMENTATION_ID true /* Mandatory claim */

/* A small unsigned integer
 *    platform/ext/common/template/attest_hal.c
 */
#define TOKEN_TEST_VALUE_SECURITY_LIFECYCLE  INT32_MAX /* Value not checked */
#define TOKEN_TEST_REQUIRE_SECURITY_LIFECYCLE true /* Mandatory claim */

/* An integer (can be positive or negative */
#define TOKEN_TEST_VALUE_CLIENT_ID 0 /* Invalid value to trigger check */
#define TOKEN_TEST_REQUIRE_CLIENT_ID true /* Mandatory claim */

/* Text string naming the profile definition:
 *    platform/ext/common/template/attest_hal.c
 */
#define TOKEN_TEST_VALUE_PROFILE_DEFINITION  "PSA_IOT_PROFILE_1"
#define TOKEN_TEST_REQUIRE_PROFILE_DEFINITION false /* Optional field */

/* Text string with verification URL or similar
 *    platform/ext/common/template/attest_hal.c
 */
#define TOKEN_TEST_VALUE_ORIGINATION "www.trustedfirmware.org"
#define TOKEN_TEST_REQUIRE_ORIGINATION false /* Optional field */

/**
 * \c TOKEN_TEST_REQUIRED_NUM_SWC can be either 0, 1, 2 or \c
 * INT32_MAX
 *
 * 0 -- No SW components are required, but if there is 1, its values
 * must compare to SWC1 correctly and if there are 2, the first must
 * compare to SWC1 and the second to SWC2.
 *
 * 1 -- At least one SW component is required and it must compare
 * correctly to SWC1. If a second one is present its values will also
 * be checked.
 *
 * 2 -- Two SW components are required and their values must compare
 * correctly.
 *
 * INT32_MAX -- No checking of the SW components of any sort is
 * performed.
 *
 * Note that attest_token_decode() checks for the presence of the the
 * EAT_CBOR_ARM_LABEL_NO_SW_COMPONENTS CBOR data item for the case of
 * no SW components and gives an error if it is absent.
 */
#define TOKEN_TEST_REQUIRED_NUM_SWC 0

/* Text string */
#define TOKEN_TEST_VALUE_SWC1_MEASUREMENT_TYPE NULL /* Value not checked */
#define TOKEN_TEST_REQUIRE_SWC1_MEASUREMENT_TYPE false /* Optional field */

/* A 32 byte mostly random value. Binary. Value not checked */
#define TOKEN_TEST_VALUE_SWC1_MEASUREMENT_VAL NULL_Q_USEFUL_BUF_C
/*
#define TOKEN_TEST_VALUE_SWC1_MEASUREMENT_VAL \
    (struct q_useful_buf_c) {\
        (uint8_t[]){ \
            0x51, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08  \
        },\
        32\
    }
*/
#define TOKEN_TEST_REQUIRE_SWC1_MEASUREMENT_VAL true /* Mandatory field */


/* Text string */
#define TOKEN_TEST_VALUE_SWC1_VERSION NULL /* Value not checked */
/* This field must be mandatory to ensure PSA compliance -
 * based on PSA Security Model document.
 */
#define TOKEN_TEST_REQUIRE_SWC1_VERSION true /* Mandatory field */

/* A 32 byte mostly random value. Binary. Value not checked */
#define TOKEN_TEST_VALUE_SWC1_SIGNER_ID NULL_Q_USEFUL_BUF_C
/*
#define TOKEN_TEST_VALUE_SWC1_SIGNER_ID \
    (struct q_useful_buf_c) {\
        (uint8_t[]){ \
            0x61, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08  \
        },\
        32\
    }
*/
/* This field must be mandatory to ensure PSA compliance -
 * based on PSA Security Model document.
 */
#define TOKEN_TEST_REQUIRE_SWC1_SIGNER_ID true /* Mandatory field */

/* Text string */
#define TOKEN_TEST_VALUE_SWC1_MEASUREMENT_DESC "SHA256" /* Hard-coded value */
#define TOKEN_TEST_REQUIRE_SWC1_MEASUREMENT_DESC false /* Optional field */

/* Text string */
#define TOKEN_TEST_VALUE_SWC2_MEASUREMENT_TYPE NULL /* Value not checked */
#define TOKEN_TEST_REQUIRE_SWC2_MEASUREMENT_TYPE false /* Optional field */

/* A 32 byte mostly random value. Binary. Value not checked */
#define TOKEN_TEST_VALUE_SWC2_MEASUREMENT_VAL NULL_Q_USEFUL_BUF_C
/*
#define TOKEN_TEST_VALUE_SWC2_MEASUREMENT_VAL \
    (struct q_useful_buf_c) {\
        (uint8_t[]){ \
            0x71, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08  \
        },\
        32\
    }
*/
#define TOKEN_TEST_REQUIRE_SWC2_MEASUREMENT_VAL true /* Mandatory field */


/* Text string */
#define TOKEN_TEST_VALUE_SWC2_VERSION NULL /* Value not checked */
/* This field must be mandatory to ensure PSA compliance -
 * based on PSA Security Model document.
 */
#define TOKEN_TEST_REQUIRE_SWC2_VERSION true /* Mandatory field */

/* A 32 byte mostly random value. Binary. Value not checked */
#define TOKEN_TEST_VALUE_SWC2_SIGNER_ID NULL_Q_USEFUL_BUF_C
/*
#define TOKEN_TEST_VALUE_SWC2_SIGNER_ID \
    (struct q_useful_buf_c) {\
        (uint8_t[]){ \
            0x81, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08  \
        },\
        32\
    }
*/
/* This field must be mandatory to ensure PSA compliance -
 * based on PSA Security Model document.
 */
#define TOKEN_TEST_REQUIRE_SWC2_SIGNER_ID true /* Mandatory field */

/* Text string */
#define TOKEN_TEST_VALUE_SWC2_MEASUREMENT_DESC "SHA256" /* Hard-coded value */
#define TOKEN_TEST_REQUIRE_SWC2_MEASUREMENT_DESC false /* Optional field */

/* Attest token maximum size, there are also platform dependent values
 * defined in region_defs.h
 */
#define ATTEST_TOKEN_MAX_SIZE  0x250

#endif /* __ATTEST_TOKEN_TEST_VALUES_H__ */
