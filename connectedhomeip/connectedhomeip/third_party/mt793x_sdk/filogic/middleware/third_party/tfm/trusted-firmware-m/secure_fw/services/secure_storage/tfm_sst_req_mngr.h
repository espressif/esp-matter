/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_SST_REQ_MNGR_H__
#define __TFM_SST_REQ_MNGR_H__

#include <stddef.h>

#include "psa/client.h"
#include "psa/protected_storage.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Handles the set request.
 *
 * \param[in]  in_vec  Pointer to the input vector which contains the input
 *                     parameters.
 * \param[in]  in_len  Number of input parameters in the input vector.
 * \param[out] out_vec Pointer to the ouput vector which contains the output
 *                     parameters.
 * \param[in]  out_len Number of output parameters in the output vector.
 *
 * \return A status indicating the success/failure of the operation as specified
 *         in \ref psa_status_t
 */
psa_status_t tfm_sst_set_req(psa_invec *in_vec, size_t in_len,
                             psa_outvec *out_vec, size_t out_len);

/**
 * \brief Handles the get request.
 *
 * \param[in]  in_vec  Pointer to the input vector which contains the input
 *                     parameters.
 * \param[in]  in_len  Number of input parameters in the input vector.
 * \param[out] out_vec Pointer to the ouput vector which contains the output
 *                     parameters.
 * \param[in]  out_len Number of output parameters in the output vector.
 *
 * \return A status indicating the success/failure of the operation as specified
 *         in \ref psa_status_t
 */
psa_status_t tfm_sst_get_req(psa_invec *in_vec, size_t in_len,
                             psa_outvec *out_vec, size_t out_len);

/**
 * \brief Handles the get info request.
 *
 * \param[in]  in_vec  Pointer to the input vector which contains the input
 *                     parameters.
 * \param[in]  in_len  Number of input parameters in the input vector.
 * \param[out] out_vec Pointer to the ouput vector which contains the output
 *                     parameters.
 * \param[in]  out_len Number of output parameters in the output vector.
 *
 * \return A status indicating the success/failure of the operation as specified
 *         in \ref psa_status_t
 */
psa_status_t tfm_sst_get_info_req(psa_invec *in_vec, size_t in_len,
                                  psa_outvec *out_vec, size_t out_len);

/**
 * \brief Handles the remove request.
 *
 * \param[in]  in_vec  Pointer to the input vector which contains the input
 *                     parameters.
 * \param[in]  in_len  Number of input parameters in the input vector.
 * \param[out] out_vec Pointer to the ouput vector which contains the output
 *                     parameters.
 * \param[in]  out_len Number of output parameters in the output vector.
 *
 * \return A status indicating the success/failure of the operation as specified
 *         in \ref psa_status_t
 */
psa_status_t tfm_sst_remove_req(psa_invec *in_vec, size_t in_len,
                                psa_outvec *out_vec, size_t out_len);

/**
 * \brief Handles the get support request.
 *
 * \param[in]  in_vec  Pointer to the input vector which contains the input
 *                     parameters.
 * \param[in]  in_len  Number of input parameters in the input vector.
 * \param[out] out_vec Pointer to the ouput vector which contains the output
 *                     parameters.
 * \param[in]  out_len Number of output parameters in the output vector.
 *
 * \return A status indicating the success/failure of the operation as specified
 *         in \ref psa_status_t
 *
 */
psa_status_t tfm_sst_get_support_req(psa_invec *in_vec, size_t in_len,
                                     psa_outvec *out_vec, size_t out_len);

/**
 * \brief Takes an input buffer containing asset data and writes
 *        its contents to the client iovec
 *
 * \param[in]  in_data Pointer to the buffer data will read from.
 * \param[in]  size    The amount of data to read.
 *
 *
 */
void sst_req_mngr_write_asset_data(const uint8_t *in_data,
                                   uint32_t size);

/**
 * \brief Writes the asset data of a client iovec onto an output buffer
 *
 * \param[out] out_data  Pointer to the buffer data will be written to.
 * \param[in]  size      The amount of data to write.
 *
 * \return A status indicating the success/failure of the operation as specified
 *         in \ref psa_status_t
 *
 */
psa_status_t sst_req_mngr_read_asset_data(uint8_t *out_data, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_SST_REQ_MNGR_H__ */
