/*
 * t_cose_make_test_messages.h
 *
 * Copyright (c) 2019, Laurence Lundblade. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */

#ifndef __T_COSE_MAKE_TEST_MESSAGES__
#define __T_COSE_MAKE_TEST_MESSAGES__


#include <stdint.h>
#include <stdbool.h>
#include "qcbor.h"
#include "t_cose_common.h"
#include "t_cose_sign1_sign.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * \file t_cose_make_test_messages.h
 *
 * \brief Create a test \c COSE_Sign1 message for testing the verifier.
 *
 */


/**
 * Various flags to pass to t_cose_test_message_sign1_sign() to
 * make different types of test messages for testing verification
 */

/** Make test message with a bstr label, which is not allowed by
  * COSE */
#define T_COSE_TEST_PARAMETER_LABEL 0x80000000U

/** Format of the crit parameter is made invalid */
#define T_COSE_TEST_BAD_CRIT_PARAMETER   0x40000000

/** An extra parameter is added. It has nested structure to be sure
 *  such are skipped correctly */
#define T_COSE_TEST_EXTRA_PARAMETER 0x20000000

/** The protected parameters bucked is left out of the COSE_Sign1
 *  message entirely */
#define T_COSE_TEST_NO_PROTECTED_PARAMETERS 0x10000000

/** The unprotected parameters bucked is left out of the COSE_Sign1
 *  message entirely */
#define T_COSE_TEST_NO_UNPROTECTED_PARAMETERS 0x08000000

/** Simple not-well-formed CBOR is added to the unprotected parameters
 *  bucket */
#define T_COSE_TEST_NOT_WELL_FORMED_1 0x04000000

/** Not-well-formed CBOR nested in a map is added to the unprotected
 *  parameters bucket */
#define T_COSE_TEST_NOT_WELL_FORMED_2 0x02000000

/** The crit parameter lists several integer critical labels and the
 *  labeled parameters exists and they are not understood */
#define T_COSE_TEST_UNKNOWN_CRIT_UINT_PARAMETER 0x01000000

/** The crit parameter lists critical labels, but none of them
 *  occur */
#define T_COSE_TEST_CRIT_PARAMETER_EXIST 0x00800000

/** Exceed the limit on number of T_COSE_PARAMETER_LIST_MAX on number
 * of crit parameters this implementation can handle */
#define T_COSE_TEST_TOO_MANY_CRIT_PARAMETER_EXIST 0x00400000

/** One of the labels in the crit parameter is of the wrong type */
#define T_COSE_TEST_BAD_CRIT_LABEL 0x00200000

/** The crit parameter is in the unprotected bucket */
#define T_COSE_TEST_CRIT_NOT_PROTECTED 0x00100000

/** More than T_COSE_PARAMETER_LIST_MAX unknown parameters occured */
#define T_COSE_TEST_TOO_MANY_UNKNOWN 0x00080000

/** The crit parameter lists several text string critical labels and
 * the labeled parameters exists and they are not understood */
#define T_COSE_TEST_UNKNOWN_CRIT_TSTR_PARAMETER 0x00040000

/** One of each type of parameter the verify handles is added, plus
 *  some unknown parameters */
#define T_COSE_TEST_ALL_PARAMETERS 0x00020000

/** An invalid CBOR type is in the protected bucket */
#define T_COSE_TEST_BAD_PROTECTED 0x00010000

/** The unprotected header bucket is an array, not a map */
#define T_COSE_TEST_UNPROTECTED_NOT_MAP 0x00008000

/** A kid is added to the protected parameters and is thus a duplicate
 *  parameter in both protected and unprotected buckets */
#define T_COSE_TEST_KID_IN_PROTECTED 0x00004000

/** The integer CoAP content type is larger than UINT16_MAX, larger
 *  than it is allowed */
#define T_COSE_TEST_TOO_LARGE_CONTENT_TYPE 0x00002000

/** The protected parameters are not a complete map. Supposed to have
 *  1 item, but has zero */
#define T_COSE_TEST_UNCLOSED_PROTECTED 0x00001000

/** The content ID parameter occurs in both protected and unprotected
 *  bucket */
#define T_COSE_TEST_DUP_CONTENT_ID 0x00000800

/** The bstr wrapped protected parameters is zero length */
#define T_COSE_TEST_EMPTY_PROTECTED_PARAMETERS 0x00000400

/** The list of critical labels parameter is empty. This is not
 * allowed by COSE */
#define T_COSE_TEST_EMPTY_CRIT_PARAMETER 0x00000200

/** Exceed the limit on number of T_COSE_PARAMETER_LIST_MAX on number
 * of crit parameters this implementation can handle */
#define T_COSE_TEST_TOO_MANY_TSTR_CRIT_LABLELS 0x00000100


/**
 * Replica of t_cose_sign1_sign() with modifications to output various
 * good and bad messages for testing of t_cose_sign1_verify() .
 *
 * \c test_message_options is one of \c T_COSE_TEST_XXX
 */
enum t_cose_err_t
t_cose_test_message_sign1_sign(struct t_cose_sign1_sign_ctx *me,
                               uint32_t                      test_message_options,
                               struct q_useful_buf_c         payload,
                               struct q_useful_buf           out_buf,
                               struct q_useful_buf_c        *result);


#ifdef __cplusplus
}
#endif

#endif /* __T_COSE_MAKE_TEST_MESSAGES__ */
