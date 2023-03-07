/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_SS_CORE_TEST_2_H__
#define __TFM_SS_CORE_TEST_2_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <limits.h>

#include "tfm_api.h"

/**
 * \brief A minimal test service to be called from another service.
 *
 * \param[in] in_vec    Array of psa_invec objects
 * \param[in] in_len    Number psa_invec objects in in_vec
 * \param[in] out_vec   Array of psa_outvec objects
 * \param[in] out_len   Number psa_outvec objects in out_vec
 *
 * The function expects 0 in_vec objects.
 * The function expects 0 out_vec objects.
 *
 * \return Returns \ref TFM_SUCCESS.
 */
psa_status_t spm_core_test_2_slave_service(
                                    struct psa_invec *in_vec, size_t in_len,
                                    struct psa_outvec *out_vec, size_t out_len);


/**
 * \brief Bitwise inverts the buffer received as input.
 *
 * \param[in] in_vec    Array of psa_invec objects
 * \param[in] in_len    Number psa_invec objects in in_vec
 * \param[in] out_vec   Array of psa_outvec objects
 * \param[in] out_len   Number psa_outvec objects in out_vec
 *
 * The function expects 2 in_vec objects:
 * in_vec[0].base: A pointer to the buffer containing the data to be inverted
 * in_vec[0].len:  The length of the buffer
 * in_vec[1].base: A pointer to an int32_t object
 * in_vec[1].len:  The size of int32_t object
 *
 * The function expects 1 out_vec object:
 * out_vec[0].base: A pointer to the buffer to put the result to
 * out_vec[0].len:  The length of the buffer
 *
 * \return Returns \ref TFM_SUCCESS on success, TFM_PARTITION_BUSY otherwise.
 */
psa_status_t spm_core_test_2_sfn_invert(struct psa_invec *in_vec, size_t in_len,
                                   struct psa_outvec *out_vec, size_t out_len);

/**
 * \brief A minimal test secure function to be called from another partition.
 *
 * Checks the functionality of querying the client ID of the caller service.
 *
 * \param[in] in_vec    Array of psa_invec objects
 * \param[in] in_len    Number psa_invec objects in in_vec
 * \param[in] out_vec   Array of psa_outvec objects
 * \param[in] out_len   Number psa_outvec objects in out_vec
 *
 * The function expects 0 in_vec objects.
 * The function expects 0 out_vec objects.
 *
 * \return Returns \ref TFM_SUCCESS on success, \ref CORE_TEST_ERRNO_TEST_FAULT
 *         othervise.
 */
    psa_status_t spm_core_test_2_check_caller_client_id(
            struct psa_invec *in_vec, size_t in_len, struct psa_outvec *out_vec,
            size_t out_len);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_SS_CORE_TEST_2_H__ */
