/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PLATFORM_SP_MTK_H__
#define __PLATFORM_SP_MTK_H__

#include "tfm_platform_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief WORK AROUND PATCH of HW AES encrypt by GCPU
 *
 * \param[in]     in_vec     Pointer to in_vec array, which contains input
 *                           arguments for GCPU encrypt input
 * \param[in]     num_invec  Number of elements in in_vec array
 * \param[in,out] out_vec    Pointer out_vec array, which contains output data
 *                           of the pin service
 * \param[in]     num_outvec Number of elements in out_vec array
 *
 * \return Returns values as specified by the \ref enum tfm_platform_err_t
 */
enum tfm_platform_err_t
tfm_hal_aes_ecb_encrypt(psa_invec  *in_vec,  uint32_t num_invec,
                        psa_outvec *out_vec, uint32_t num_outvec);

/*!
 * \brief WORK AROUND PATCH of generate random number with TRNG
 *
 * \param[in]     in_vec     Pointer to in_vec array, no data to input TRNG
 * \param[in]     num_invec  Number of elements in in_vec array
 * \param[in,out] out_vec    Pointer out_vec array, which contains output data
 *                           of random number
 * \param[in]     num_outvec Number of elements in out_vec array
 *
 * \return Returns values as specified by the \ref enum tfm_platform_err_t
 */
enum tfm_platform_err_t
tfm_hal_trng_random_generate(psa_invec  *in_vec,  uint32_t num_invec,
                             psa_outvec *out_vec, uint32_t num_outvec);

#ifdef __cplusplus
}
#endif

#endif /* __PLATFORM_SP_MTK_H__ */
