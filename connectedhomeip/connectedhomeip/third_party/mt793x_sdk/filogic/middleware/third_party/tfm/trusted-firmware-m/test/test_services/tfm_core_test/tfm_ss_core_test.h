/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_SS_CORE_TEST_H__
#define __TFM_SS_CORE_TEST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <limits.h>
#include "tfm_api.h"

/**
 * \brief Tests whether the initialisation of the service was successful.
 *
 * \param[in] in_vec    Array of psa_invec objects
 * \param[in] in_len    Number psa_invec objects in in_vec
 * \param[in] out_vec   Array of psa_outvec objects
 * \param[in] out_len   Number psa_outvec objects in out_vec
 *
 * \param[in] in_vec    Array of psa_invec objects
 * \param[in] in_len    Number psa_invec objects in in_vec
 * \param[in] out_vec   Array of psa_outvec objects
 * \param[in] out_len   Number psa_outvec objects in out_vec
 *
 * The function expects 0 in_vec objects.
 * The function expects 0 out_vec objects.
 *
 * \return Returns \ref CORE_TEST_ERRNO_SUCCESS on success, and
 *                 \ref CORE_TEST_ERRNO_SP_NOT_INITED on failure.
 */
psa_status_t spm_core_test_sfn_init_success(
                                    struct psa_invec *in_vec, size_t in_len,
                                    struct psa_outvec *out_vec, size_t out_len);

/**
 * \brief Tests what happens when a service calls itself directly.
 *
 * \param[in] in_vec    Array of psa_invec objects
 * \param[in] in_len    Number psa_invec objects in in_vec
 * \param[in] out_vec   Array of psa_outvec objects
 * \param[in] out_len   Number psa_outvec objects in out_vec
 *
 * The function expects 1 in_vec object:
 * in_vec[0].base: A buffer containing a pointer to an uint32_t value
 *                 containing the current depth of the call (the value of the
 *                 depth is 0 when first called).
 * in_vec[0].len:  The size of a pointer in bytes.

 * The function expects 0 out_vec objects.
 *
 * \return Returns \ref CORE_TEST_ERRNO_SUCCESS.
 */
psa_status_t spm_core_test_sfn_direct_recursion(
                                    struct psa_invec *in_vec, size_t in_len,
                                    struct psa_outvec *out_vec, size_t out_len);

/**
 * \brief Entry point for multiple test cases to be executed on the secure side.
 *
 * \param[in] in_vec    Array of psa_invec objects
 * \param[in] in_len    Number psa_invec objects in in_vec
 * \param[in] out_vec   Array of psa_outvec objects
 * \param[in] out_len   Number psa_outvec objects in out_vec
 *
 * The function expects at least 1 in_vec object:
 *
 * in_vec[0].base: A buffer containing a pointer to an uint32_t value
 *                 containing the testcase id to be executed.
 * in_vec[0].len:  The size of a pointer in bytes.
 *
 * The number of expected additional in_vecs and out_vecs is dependent on the id
 * of the test case. For details see the function implementation.
 *
 * \return Can return various error codes.
 */
psa_status_t spm_core_test_sfn(struct psa_invec *in_vec, size_t in_len,
                          struct psa_outvec *out_vec, size_t out_len);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_SS_CORE_TEST_H__ */
