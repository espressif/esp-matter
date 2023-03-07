/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_MTK_CI_TEST_H__
#define __TFM_MTK_CI_TEST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tfm_api.h"

/**
 * \brief Excute undefine instruction to test TFM exception handler
 *
 * \param[in] in_vec    Array of psa_invec objects
 * \param[in] in_len    Number psa_invec objects in in_vec
 * \param[in] out_vec   Array of psa_outvec objects
 * \param[in] out_len   Number psa_outvec objects in out_vec
 *
 * The function expects platform core dump
 *
 * \return Can return various error codes.
 */
psa_status_t tfm_ci_exception_handle_test(psa_invec *in_vec, uint32_t num_invec, psa_outvec *out_vec, uint32_t num_outvec);

/**
 * \brief Check platform isolation configurations status
 *
 * \param[in] in_vec    Array of psa_invec objects
 * \param[in] in_len    Number psa_invec objects in in_vec
 * \param[in] out_vec   Array of psa_outvec objects
 * \param[in] out_len   Number psa_outvec objects in out_vec
 *
 * The function expects return the result of check
 *
 * \return Can return various error codes.
 */
psa_status_t tfm_ci_platform_isolation_test(psa_invec *in_vec, uint32_t num_invec, psa_outvec *out_vec, uint32_t num_outvec);

/**
 * \brief Init CI test service
 *
 * \param[in] in_vec    Array of psa_invec objects
 * \param[in] in_len    Number psa_invec objects in in_vec
 * \param[in] out_vec   Array of psa_outvec objects
 * \param[in] out_len   Number psa_outvec objects in out_vec
 *
 * The function do nothing & only return success
 *
 * \return Can return various error codes.
 */
int32_t tfm_CI_test_init(psa_invec *in_vec, uint32_t num_invec, psa_outvec *out_vec, uint32_t num_outvec);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_MTK_CI_TEST_H__ */
